#include "Mandelbrot.h"

std::atomic<size_t> Mandelbrot::m_nWorkerComplete = 0;

Mandelbrot::Mandelbrot()
    : FractalSet("Mandelbrot"),
      m_workers(32)
{
    for (auto &worker : m_workers)
    {
        worker.alive = true;
        worker.fractalArray = m_fractalArray;
        worker.nWorkers = m_workers.size();
        worker.screenWidth = Window::GetWidth();
        worker.thread = std::thread(&Mandelbrot::Worker::Compute, &worker);
    }
}

void Mandelbrot::Start(const std::pair<sf::Vector2f, sf::Vector2f> &viewport)
{
    double imageSectionWidth = static_cast<double>(Window::GetWidth()) / static_cast<double>(m_workers.size());
    double fractalSectionWidth = static_cast<double>(viewport.second.x - viewport.first.x) / static_cast<double>(m_workers.size());
    m_nWorkerComplete = 0;

    for (size_t i = 0; i < m_workers.size(); i++)
    {
        m_workers[i].imageTL = sf::Vector2<double>(imageSectionWidth * i, 0.0f);
        m_workers[i].imageBR = sf::Vector2<double>(imageSectionWidth * (i + 1), Window::GetHeight());
        m_workers[i].fractalTL = sf::Vector2<double>(viewport.first.x + static_cast<double>(fractalSectionWidth * i), viewport.first.y);
        m_workers[i].fractalBR = sf::Vector2<double>(viewport.first.x + static_cast<double>(fractalSectionWidth * (i + 1)), viewport.second.y);
        m_workers[i].iterations = m_computeIterations;

        std::unique_lock<std::mutex> lm(m_workers[i].mutex);
        m_workers[i].cvStart.notify_one();
    }

    while (m_nWorkerComplete < 1) // Wait for all workers to complete
    {
    }
}

void Mandelbrot::Worker::Compute()
{
    while (alive)
    {
        std::unique_lock<std::mutex> lm(mutex);
        cvStart.wait(lm);

        double xScale = (fractalBR.x - fractalTL.x) / (imageBR.x - imageTL.x);
        double yScale = (fractalBR.y - fractalTL.y) / (imageBR.y - imageTL.y);

        double y_pos = fractalTL.y;

        int y_offset = 0;
        int row_size = screenWidth;

        int x, y;

        SIMD_Double _a, _b, _two, _four, _mask1;
        SIMD_Double _zr, _zi, _zr2, _zi2, _cr, _ci;
        SIMD_Double _x_pos_offsets, _x_pos, _x_scale, _x_jump;
        SIMD_Integer _one, _c, _n, _iterations, _mask2;

        _one = _mm256_set1_epi64x(1);
        _two = _mm256_set1_pd(2.0);
        _four = _mm256_set1_pd(4.0);
        _iterations = _mm256_set1_epi64x(iterations);

        _x_scale = _mm256_set1_pd(xScale);
        _x_jump = _mm256_set1_pd(xScale * 4.0);
        _x_pos_offsets = _mm256_set_pd(0.0, 1.0, 2.0, 3.0);
        _x_pos_offsets = _mm256_mul_pd(_x_pos_offsets, _x_scale);

        for (y = imageTL.y; y < imageBR.y; y++)
        {
            // Reset x_position
            _a = _mm256_set1_pd(fractalTL.x);
            _x_pos = _mm256_add_pd(_a, _x_pos_offsets);

            _ci = _mm256_set1_pd(y_pos);

            for (x = imageTL.x; x < imageBR.x; x += 4)
            {
                _cr = _x_pos;
                _zr = _mm256_setzero_pd();
                _zi = _mm256_setzero_pd();
                _n = _mm256_setzero_si256();

            repeat:
                _zr2 = _mm256_mul_pd(_zr, _zr);
                _zi2 = _mm256_mul_pd(_zi, _zi);
                _a = _mm256_sub_pd(_zr2, _zi2);
                _a = _mm256_add_pd(_a, _cr);
                _b = _mm256_mul_pd(_zr, _zi);
                _b = _mm256_fmadd_pd(_b, _two, _ci);
                _zr = _a;
                _zi = _b;
                _a = _mm256_add_pd(_zr2, _zi2);
                _mask1 = _mm256_cmp_pd(_a, _four, _CMP_LT_OQ);
                _mask2 = _mm256_cmpgt_epi64(_iterations, _n);
                _mask2 = _mm256_and_si256(_mask2, _mm256_castpd_si256(_mask1));
                _c = _mm256_and_si256(_one, _mask2); // Zero out ones where n < iterations
                _n = _mm256_add_epi64(_n, _c);       // n++ Increase all n
                if (_mm256_movemask_pd(_mm256_castsi256_pd(_mask2)) > 0)
                    goto repeat;

                fractalArray[y_offset + x + 0] = static_cast<int>(_n[3]);
                fractalArray[y_offset + x + 1] = static_cast<int>(_n[2]);
                fractalArray[y_offset + x + 2] = static_cast<int>(_n[1]);
                fractalArray[y_offset + x + 3] = static_cast<int>(_n[0]);

                _x_pos = _mm256_add_pd(_x_pos, _x_jump);
            }

            y_pos += yScale;
            y_offset += row_size;
        }

        m_nWorkerComplete++;
    }
}