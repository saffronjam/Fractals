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
}

FractalSet::~FractalSet()
{
    delete[] m_fractalArray;
}

void FractalSet::Draw()
{
    for (int y = 0; y < Window::GetHeight(); y++)
    {
        for (int x = 0; x < Window::GetWidth(); x++)
        {
            int i = m_fractalArray[y * Window::GetWidth() + x];
            float n = static_cast<float>(i);
            float a = 0.1f;
            sf::Uint8 r = static_cast<sf::Uint8>(255.0f * (0.5f * std::sin(a * n) + 0.5f));
            sf::Uint8 g = static_cast<sf::Uint8>(255.0f * (0.5f * std::sin(a * n + 2.094f) + 0.5f));
            sf::Uint8 b = static_cast<sf::Uint8>(255.0f * (0.5f * std::sin(a * n + 4.188f) + 0.5f));
            m_vertexArray[i].color = sf::Color(r, g, b);
        }
    }
    Window::Draw(m_vertexArray);
    // Camera::Draw(m_vertexArray);
}