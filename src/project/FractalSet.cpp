#include "FractalSet.h"

std::atomic<size_t> FractalSet::m_nWorkerComplete = 0;

FractalSet::FractalSet(const std::string &name)
    : m_name(name),
      m_computeIterations(64),
      m_vertexArray(sf::PrimitiveType::Points, Window::GetWidth() * Window::GetHeight()),
      m_fractalArray(new int[Window::GetWidth() * Window::GetHeight()])
{
    for (size_t i = 0; i < m_vertexArray.getVertexCount(); i++)
    {
        m_vertexArray[i].position = sf::Vector2f(static_cast<float>(std::floor(i % Window::GetWidth())), static_cast<float>(std::floor(i / Window::GetWidth())));
    }

    float a = 0.1f;
    for (int i = 0; i < 500; i++)
    {
        m_rSinLookup[i] = std::sin(static_cast<float>(i) * a - 1.0f);
        m_gSinLookup[i] = std::sin(static_cast<float>(i) * a + 3.188f);
        m_bSinLookup[i] = std::sin(static_cast<float>(i) * a + 1.094f);
    }
}

FractalSet::~FractalSet()
{
    delete[] m_fractalArray;
    for (auto &worker : m_workers)
    {
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
    double imageSectionWidth = static_cast<double>(Window::GetWidth()) / static_cast<double>(m_workers.size());
    double fractalSectionWidth = static_cast<double>(viewport.second.x - viewport.first.x) / static_cast<double>(m_workers.size());
    m_nWorkerComplete = 0;

    for (size_t i = 0; i < m_workers.size(); i++)
    {
        m_workers[i]->imageTL = sf::Vector2<double>(imageSectionWidth * i, 0.0f);
        m_workers[i]->imageBR = sf::Vector2<double>(imageSectionWidth * (i + 1), Window::GetHeight());
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
    for (int y = 0; y < Window::GetHeight(); y++)
    {
        for (int x = 0; x < Window::GetWidth(); x++)
        {
            int i = m_fractalArray[y * Window::GetWidth() + x];
            float a = 0.1f;
            sf::Uint8 r = static_cast<sf::Uint8>(30.0f * (0.5f * m_rSinLookup[i] + 0.5f));
            sf::Uint8 g = static_cast<sf::Uint8>(30.0f * (0.5f * m_gSinLookup[i] + 0.5f));
            sf::Uint8 b = static_cast<sf::Uint8>(30.0f * (0.5f * m_bSinLookup[i] + 0.5f));
            m_vertexArray[y * Window::GetWidth() + x].color = {r, g, b};
        }
    }
}

void FractalSet::AddWorker(Worker *worker)
{
    worker->alive = true;
    worker->fractalArray = m_fractalArray;
    worker->nWorkers = m_workers.size();
    worker->screenWidth = Window::GetWidth();
    worker->thread = std::thread(&FractalSet::Worker::Compute, worker);
    m_workers.push_back(worker);
}

sf::Vector2f FractalSet::TranslatePoint(const sf::Vector2f &point, int iterations)
{
    std::complex<float> c(point.x, point.y);
    std::complex<float> z(0.0f, 0.0f);

    for (int n = 0; n < iterations && abs(z) < 2.0; n++)
        z = (z * z) + c;

    return sf::Vector2f(z.real(), z.imag());
}

void FractalSet::SetComputeIteration(size_t iterations) noexcept
{
    m_computeIterations = iterations;
    ReconstructImage();
}