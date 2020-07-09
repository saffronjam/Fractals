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
#include "HSVColor.h"

class FractalSet
{
public:
    enum Palette
    {
        Fiery,
        UV,
        GreyScale
    };

protected:
    struct Worker;

public:
    FractalSet(const std::string &name);
    ~FractalSet();

    void Draw();

    void Start(const std::pair<sf::Vector2f, sf::Vector2f> &viewport);
    void ReconstructImage();
    void AddWorker(Worker *worker);

    virtual sf::Vector2f TranslatePoint(const sf::Vector2f &point, int iterations = 1) = 0;

    const std::string &GetName() const noexcept { return m_name; }

    void SetComputeIteration(size_t iterations) noexcept;
    void SetPalette(Palette palette) noexcept { m_currentPalette = palette; }

protected:
    std::string m_name;
    static std::atomic<size_t> m_nWorkerComplete;
    static int m_simWidth;
    static int m_simHeight;

private:
    Palette m_currentPalette;

    std::vector<Worker *> m_workers;

    size_t m_computeIterations;
    sf::VertexArray m_vertexArray;
    int *m_fractalArray;

    std::vector<sf::Image> m_palettes;

protected:
    struct Worker
    {
        int *fractalArray;
        size_t nWorkers = 0;

        sf::Vector2<double> imageTL = {0.0f, 0.0f};
        sf::Vector2<double> imageBR = {0.0f, 0.0f};
        sf::Vector2<double> fractalTL = {0.0f, 0.0f};
        sf::Vector2<double> fractalBR = {0.0f, 0.0f};

        size_t iterations = 0;
        bool alive = true;

        std::thread thread;
        std::condition_variable cvStart;
        std::mutex mutex;

        virtual void Compute() = 0;
    };
};