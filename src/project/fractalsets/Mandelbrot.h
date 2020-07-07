#pragma once

#include "FractalSet.h"

class Mandelbrot : public FractalSet
{
    struct Worker;

public:
    Mandelbrot();

    void Start(const std::pair<sf::Vector2f, sf::Vector2f> &viewport) override;

private:
    std::vector<Worker> m_workers;
    static std::atomic<size_t> m_nWorkerComplete;

private:
    struct Worker
    {
        int *fractalArray;

        int screenWidth = 0;
        sf::Vector2i fractalImageSize = {0, 0};
        sf::Vector2i fractalTL = {0, 0};
        sf::Vector2i fractalBR = {0, 0};

        size_t iterations = 0;
        bool alive = true;

        std::thread thread;
        std::condition_variable cvStart;
        std::mutex mutex;

        void Compute();
    };
};