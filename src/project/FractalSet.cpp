#include "FractalSet.h"

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
}

void FractalSet::Draw()
{
    Window::Draw(m_vertexArray);
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

void FractalSet::SetComputeIteration(size_t iterations) noexcept
{
    m_computeIterations = iterations;
    ReconstructImage();
}