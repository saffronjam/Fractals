#pragma once

#include <thread>
#include <mutex>
#include <atomic>
#include <condition_variable>

#include <SFML/Graphics/Image.hpp>
#include <SFML/Graphics/Sprite.hpp>
#include <SIMD/Kit.h>

#include "Camera.h"

class FractalSet
{
public:
    FractalSet(const std::string &name);
    ~FractalSet();

    void Draw();

    virtual void Start(const std::pair<sf::Vector2f, sf::Vector2f> &viewport) = 0;
    const std::string &GetName() const noexcept { return m_name; }

    void SetComputeIteration() noexcept;

protected:
    std::string m_name;
    size_t m_computeIterations;
    int *m_fractalArray;

private:
    sf::VertexArray m_vertexArray;
};