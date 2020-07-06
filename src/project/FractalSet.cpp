#include "FractalSet.h"

FractalSet::FractalSet(const std::string &name)
    : m_name(name),
      m_computeIterations(64),
      m_vertexArray(sf::PrimitiveType::Points, Window::GetWidth() * Window::GetHeight())
{
}

void FractalSet::Draw()
{
    Camera::Draw(m_vertexArray);
}
