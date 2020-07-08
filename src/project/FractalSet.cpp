#include "FractalSet.h"

std::atomic<size_t> FractalSet::m_nWorkerComplete = 0;
int FractalSet::m_simWidth(0);
int FractalSet::m_simHeight(0);

FractalSet::FractalSet(const std::string &name)
    : m_name(name),
      m_computeIterations(64),
      m_vertexArray(sf::PrimitiveType::Points, (Window::GetWidth() - 200) * Window::GetHeight()),
      m_fractalArray(new int[(Window::GetWidth() - 200) * Window::GetHeight()])
{
    m_simWidth = Window::GetWidth() - 200;
    m_simHeight = Window::GetHeight();

    m_colorPalette.loadFromFile("res/fractalPal.png");

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

void FractalSet::Draw()
{
    Window::Draw(m_vertexArray);
}

void FractalSet::Start(const std::pair<sf::Vector2f, sf::Vector2f> &viewport)
{
    double imageSectionWidth = static_cast<double>(m_simWidth) / static_cast<double>(m_workers.size());
    double fractalSectionWidth = static_cast<double>(viewport.second.x - viewport.first.x) / static_cast<double>(m_workers.size());
    m_nWorkerComplete = 0;

    for (size_t i = 0; i < m_workers.size(); i++)
    {
        m_workers[i]->imageTL = sf::Vector2<double>(imageSectionWidth * i, 0.0f);
        m_workers[i]->imageBR = sf::Vector2<double>(imageSectionWidth * (i + 1), m_simHeight);
        m_workers[i]->fractalTL = sf::Vector2<double>(viewport.first.x + static_cast<double>(fractalSectionWidth * i), viewport.first.y);
        m_workers[i]->fractalBR = sf::Vector2<double>(viewport.first.x + static_cast<double>(fractalSectionWidth * (i + 1)), viewport.second.y);
        m_workers[i]->iterations = m_computeIterations;

        std::unique_lock<std::mutex> lm(m_workers[i]->mutex);
        m_workers[i]->cvStart.notify_one();
    }

    while (m_nWorkerComplete < 1) // Wait for all workers to complete
    {
    }
}

void FractalSet::ReconstructImage()
{
    auto colorPal = m_colorPalette.getPixelsPtr();
    for (int y = 0; y < m_simHeight; y++)
    {
        for (int x = 0; x < m_simWidth; x++)
        {
            int i = m_fractalArray[y * m_simWidth + x];
            float offset = static_cast<float>(i) / static_cast<float>(m_computeIterations) * 256.0f;
            memcpy(&m_vertexArray[y * m_simWidth + x].color, &colorPal[static_cast<int>(offset) * 4], sizeof(sf::Uint8) * 3);
        }
    }
}

void FractalSet::AddWorker(Worker *worker)
{
    worker->alive = true;
    worker->fractalArray = m_fractalArray;
    worker->nWorkers = m_workers.size();
    worker->thread = std::thread(&FractalSet::Worker::Compute, worker);
    m_workers.push_back(worker);
}

void FractalSet::SetComputeIteration(size_t iterations) noexcept
{
    m_computeIterations = iterations;
    ReconstructImage();
}