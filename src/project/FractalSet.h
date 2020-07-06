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

    void Draw();

    virtual void Start(const sf::FloatRect &rect) = 0;
    const std::string &GetName() const noexcept { return m_name; }

    void SetComputeIteration() noexcept;

protected:
    std::string m_name;
    size_t m_computeIterations;
    sf::VertexArray m_vertexArray;
};