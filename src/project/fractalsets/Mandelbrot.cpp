#include "Mandelbrot.h"
#include <condition_variable>
#include <atomic>
#include <complex>
#include <cstdlib>
#include <immintrin.h>

Mandelbrot::Mandelbrot()
    : FractalSet("Mandelbrot"),
      m_workers(32)
{
    for (auto &worker : m_workers)
    {
        worker.alive = true;
        worker.vertexArray = &m_vertexArray;
        worker.screenWidth = Window::GetWidth();
        worker.thread = std::thread(&Mandelbrot::Worker::Compute, &worker);
    }
}

void Mandelbrot::Start(const sf::FloatRect &rect)
{
    int nSectionWidth = rect.width / m_workers.size();
    double dFractalWidth = Window::GetWidth() / static_cast<double>(m_workers.size());
    m_nWorkerComplete = 0;

    for (size_t i = 0; i < m_workers.size(); i++)
    {
        m_workers[i].vertexArrayTL = sf::Vector2i(rect.left + nSectionWidth * i, rect.top);
        m_workers[i].vertexArrayBR = sf::Vector2i(rect.left + nSectionWidth * (i + 1), rect.top + rect.height);
        m_workers[i].fractalTL = sf::Vector2i(dFractalWidth * i, 0);
        m_workers[i].fractalBR = sf::Vector2i(dFractalWidth * (i + 1), Window::GetHeight());
        m_workers[i].iterations = m_computeIterations;

        std::unique_lock<std::mutex> lm(m_workers[i].mutex);
        m_workers[i].cvStart.notify_one();
    }

    while (m_nWorkerComplete < m_workers.size()) // Wait for all workers to complete
    {
    }
}

void Mandelbrot::Worker::Compute()
{
    while (alive)
    {
        std::unique_lock<std::mutex> lm(mutex);
        cvStart.wait(lm);

        double x_scale = (fractalBR.x - fractalTL.x) / (double(vertexArrayBR.x) - double(vertexArrayTL.x));
        double y_scale = (fractalBR.y - fractalTL.y) / (double(vertexArrayBR.y) - double(vertexArrayTL.y));

        double y_pos = fractalTL.y;

        int y_offset = 0;
        int row_size = screenWidth;

        int x, y;

        SIMDd _a, _b, _two, _four, _mask1;
        SIMDd _zr, _zi, _zr2, _zi2, _cr, _ci;
        SIMDd _x_pos_offsets, _x_pos, _x_scale, _x_jump;
        SIMDi _one, _c, _n, _iterations, _mask2;

        _one = _mm256_set1_epi64x(1);
        _two = _mm256_set1_pd(2.0);
        _four = _mm256_set1_pd(4.0);
        _iterations = _mm256_set1_epi64x(iterations);

        _x_scale = _mm256_set1_pd(x_scale);
        _x_jump = _mm256_set1_pd(x_scale * 4);
        _x_pos_offsets = _mm256_set_pd(0, 1, 2, 3);
        _x_pos_offsets = _mm256_mul_pd(_x_pos_offsets, _x_scale);

        for (y = vertexArrayTL.y; y < vertexArrayBR.y; y++)
        {
            // Reset x_position
            _a = _mm256_set1_pd(fractalTL.x);
            _x_pos = _mm256_add_pd(_a, _x_pos_offsets);

            _ci = _mm256_set1_pd(y_pos);

            for (x = vertexArrayTL.x; x < vertexArrayBR.x; x += 4)
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

                (*vertexArray)[y_offset + x + 0] = static_cast<int>(_n.m256i_i64[3]);
                (*vertexArray)[y_offset + x + 1] = static_cast<int>(_n.m256i_i64[2]);
                (*vertexArray)[y_offset + x + 2] = static_cast<int>(_n.m256i_i64[1]);
                (*vertexArray)[y_offset + x + 3] = static_cast<int>(_n.m256i_i64[0]);
                _x_pos = _mm256_add_pd(_x_pos, _x_jump);
            }

            y_pos += y_scale;
            y_offset += row_size;
        }
        m_nWorkerComplete++;
    }
}