#include "FractalSet.h"

std::atomic<size_t> FractalSet::m_nWorkerComplete = 0;
int FractalSet::m_simWidth(0);
int FractalSet::m_simHeight(0);
std::pair<sf::Vector2f, sf::Vector2f> FractalSet::m_simBox(vl::Null<>(), vl::Null<>());

FractalSet::FractalSet(const std::string &name)
        : m_name(name),
          m_desiredPalette(Fiery),
          m_reconstructImage(true),
          m_colorTransitionTimer(0.0f),
          m_colorTransitionDuration(0.7f),
          m_computeIterations(64),
          m_vertexArray(sf::PrimitiveType::Points, (Window::GetWidth() - 200) * Window::GetHeight()),
          m_fractalArray(new int[(Window::GetWidth() - 200) * Window::GetHeight()]),
          m_palettes(4)
{
    m_simWidth = Window::GetWidth() - 200;
    m_simHeight = Window::GetHeight();

    m_palettes[Fiery].loadFromFile("res/pals/fiery.png");
    m_palettes[UV].loadFromFile("res/pals/uv.png");
    m_palettes[GreyScale].loadFromFile("res/pals/greyscale.png");
    m_palettes[Rainbow].loadFromFile("res/pals/rainbow.png");

    m_currentPalette.create(256, 1, m_palettes[m_desiredPalette].getPixelsPtr());

    for (int i = 0; i < 256; i++)
    {
        const auto pix = m_currentPalette.getPixel(i, 0);
        m_colorsStart[i] = {(float) pix.r / 255.0f, (float) pix.g / 255.0f, (float) pix.b / 255.0f, (float) pix.a / 255.0f};
    }
    m_colorsCurrent = m_colorsStart;

    for (size_t i = 0; i < m_vertexArray.getVertexCount(); i++)
    {
        m_vertexArray[i].position = sf::Vector2f(static_cast<float>(std::floor(i % m_simWidth)), static_cast<float>(std::floor(i / m_simWidth)));
    }
}

FractalSet::~FractalSet()
{
    delete[] m_fractalArray;
    for (auto &worker : m_workers)
    {
        worker->alive = false;
        worker->cvStart.notify_all();
        if (worker->thread.joinable())
            worker->thread.join();
        delete worker;
        worker = nullptr;
    }
}

void FractalSet::Update()
{
    if (m_colorTransitionTimer <= m_colorTransitionDuration)
    {
        float delta = (std::sin((m_colorTransitionTimer / m_colorTransitionDuration) * PI<> - PI<> / 2.0f) + 1.0f) / 2.0f;
        for (int x = 0; x < 256; x++)
        {
            const auto pix = m_palettes[m_desiredPalette].getPixel(x, 0);
            TransitionColor goalColor = {(float) pix.r / 255.0f, (float) pix.g / 255.0f, (float) pix.b / 255.0f, (float) pix.a / 255.0f};
            const auto &startColor = m_colorsStart[x];
            auto &currentColor = m_colorsCurrent[x];
            currentColor.r = startColor.r + delta * (goalColor.r - startColor.r);
            currentColor.g = startColor.g + delta * (goalColor.g - startColor.g);
            currentColor.b = startColor.b + delta * (goalColor.b - startColor.b);
            m_currentPalette.setPixel(x, 0, {(sf::Uint8) (currentColor.r * 255.0f), (sf::Uint8) (currentColor.g * 255.0f), (sf::Uint8) (currentColor.b * 255.0f), (sf::Uint8) (currentColor.a * 255.0f)});
        }
        MarkImageForReconstruct();
        m_colorTransitionTimer += Clock::Delta().asSeconds();
    }
    ReconstructImage();
}

void FractalSet::Draw()
{
    Window::Draw(m_vertexArray);
}

void FractalSet::Start()
{
    double imageSectionWidth = static_cast<double>(m_simWidth) / static_cast<double>(m_workers.size());
    double fractalSectionWidth = static_cast<double>(m_simBox.second.x - m_simBox.first.x) / static_cast<double>(m_workers.size());
    m_nWorkerComplete = 0;

    for (size_t i = 0; i < m_workers.size(); i++)
    {
        m_workers[i]->imageTL = sf::Vector2<double>(imageSectionWidth * i, 0.0f);
        m_workers[i]->imageBR = sf::Vector2<double>(imageSectionWidth * (i + 1), m_simHeight);
        m_workers[i]->fractalTL = sf::Vector2<double>(m_simBox.first.x + static_cast<double>(fractalSectionWidth * i), m_simBox.first.y);
        m_workers[i]->fractalBR = sf::Vector2<double>(m_simBox.first.x + static_cast<double>(fractalSectionWidth * (i + 1)), m_simBox.second.y);
        m_workers[i]->iterations = m_computeIterations;

        std::unique_lock<std::mutex> lm(m_workers[i]->mutex);
        m_workers[i]->cvStart.notify_one();
    }

    while (m_nWorkerComplete < 1) // Wait for all workers to complete
    {
    }
}

void FractalSet::MarkImageForReconstruct()
{
    m_reconstructImage = true;
}

void FractalSet::AddWorker(Worker *worker)
{
    worker->alive = true;
    worker->fractalArray = m_fractalArray;
    worker->thread = std::thread(&FractalSet::Worker::Compute, worker);
    m_workers.push_back(worker);
}

void FractalSet::SetSimBox(const std::pair<sf::Vector2f, sf::Vector2f> &box)
{
    m_simBox = box;
}

void FractalSet::SetComputeIteration(size_t iterations) noexcept
{
    m_computeIterations = iterations;
    ReconstructImage();
}

void FractalSet::SetPalette(FractalSet::Palette palette) noexcept
{
    m_desiredPalette = palette;
    m_colorTransitionTimer = 0.0f;
    m_colorsStart = m_colorsCurrent;
}

void FractalSet::ReconstructImage()
{
    if(m_reconstructImage)
    {
        auto colorPal = m_currentPalette.getPixelsPtr();
        for (int y = 0; y < m_simHeight; y++)
        {
            for (int x = 0; x < m_simWidth; x++)
            {
                int i = m_fractalArray[y * m_simWidth + x];
                float offset = static_cast<float>(i) / static_cast<float>(m_computeIterations) * 256.0f;
                memcpy(&m_vertexArray[y * m_simWidth + x].color, &colorPal[static_cast<int>(offset) * 4], sizeof(sf::Uint8) * 3);
            }
        }
        m_reconstructImage = false;
    }
}