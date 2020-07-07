#pragma once

#include <thread>
#include <mutex>
#include <atomic>
#include <condition_variable>
#include <complex>

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
    virtual sf::Vector2f TranslatePoint(const sf::Vector2f &point, int iterations = 1) = 0;
    void ReconstructImage();

    const std::string &GetName() const noexcept { return m_name; }

    void SetComputeIteration(size_t iterations) noexcept;

protected:
    std::string m_name;
    size_t m_computeIterations;
    int *m_fractalArray;

private:
    sf::VertexArray m_vertexArray;

    std::array<double, 500> m_rSinLookup;
    std::array<double, 500> m_gSinLookup;
    std::array<double, 500> m_bSinLookup;
};