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
        GreyScale,
        Rainbow
    };

protected:
    struct Worker;

public:
    explicit FractalSet(std::string name);
    virtual ~FractalSet();

    virtual void Update();
    virtual void Draw();

    void MarkForImageRecompute() noexcept;
    void MarkForImageReconstruct() noexcept;
    void AddWorker(Worker *worker);

    [[nodiscard]] const std::string &GetName() const noexcept { return m_name; }

    void SetSimBox(const std::pair<sf::Vector2f, sf::Vector2f> &box);
    void SetComputeIterationCount(size_t iterations) noexcept;
    void SetPalette(Palette palette) noexcept;

private:
    void RecomputeImage();
    void ReconstructImage();

protected:
    std::string m_name;

    size_t m_computeIterations;
    std::vector<Worker *> m_workers;
    std::atomic<size_t> m_nWorkerComplete;

    int m_simWidth;
    int m_simHeight;
    std::pair<sf::Vector2f, sf::Vector2f> m_simBox;

private:
    struct TransitionColor
    {
        float r;
        float g;
        float b;
        float a;
    };

    sf::VertexArray m_vertexArray;
    int *m_fractalArray;

    // Marks with true if the image should be recomputed/reconstructed this frame
    bool m_recomputeImage;
    bool m_reconstructImage;

    // Animate palette change
    Palette m_desiredPalette;
    sf::Image m_currentPalette;
    std::array<TransitionColor, 256> m_colorsStart;
    std::array<TransitionColor, 256> m_colorsCurrent;
    float m_colorTransitionTimer;
    float m_colorTransitionDuration;
    std::vector<sf::Image> m_palettes;

    static constexpr int PaletteWidth = 256;

protected:
    struct Worker
    {
        virtual ~Worker() = default;
        virtual void Compute() = 0;

        std::atomic<size_t> *nWorkerComplete = nullptr;

        int *fractalArray;
        int simWidth = 0;

        sf::Vector2<double> imageTL = {0.0f, 0.0f};
        sf::Vector2<double> imageBR = {0.0f, 0.0f};
        sf::Vector2<double> fractalTL = {0.0f, 0.0f};
        sf::Vector2<double> fractalBR = {0.0f, 0.0f};

        size_t iterations = 0;
        bool alive = true;

        std::thread thread;
        std::condition_variable cvStart;
        std::mutex mutex;

    };
};