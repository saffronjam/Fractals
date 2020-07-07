#include "Mandelbrot.h"

std::atomic<size_t> Mandelbrot::m_nWorkerComplete = 0;

Mandelbrot::Mandelbrot()
    : FractalSet("Mandelbrot"),
      m_workers(1)
{
    for (auto &worker : m_workers)
    {
        worker.alive = true;
        worker.fractalArray = m_fractalArray;
        worker.screenWidth = Window::GetWidth();
        worker.thread = std::thread(&Mandelbrot::Worker::Compute, &worker);
        worker.fractalImageSize = sf::Vector2i(Window::GetWidth() / static_cast<double>(m_workers.size()), Window::GetHeight());
    }
}

void Mandelbrot::Start(const std::pair<sf::Vector2f, sf::Vector2f> &viewport)
{
    int nSectionWidth = (viewport.first.x - viewport.second.x) / m_workers.size();
    m_nWorkerComplete = 0;

    for (size_t i = 0; i < m_workers.size(); i++)
    {
        m_workers[i].fractalTL = sf::Vector2i(viewport.first.x, viewport.first.y);
        m_workers[i].fractalBR = sf::Vector2i(viewport.second.x, viewport.second.y);
        m_workers[i].iterations = m_computeIterations;

        std::unique_lock<std::mutex> lm(m_workers[i].mutex);
        m_workers[i].cvStart.notify_one();
    }

    while (m_nWorkerComplete < m_workers.size()) // Wait for all workers to complete
    {
    }
}

#include <complex>

void Mandelbrot::Worker::Compute()
{
    while (alive)
    {
        std::unique_lock<std::mutex> lm(mutex);
        cvStart.wait(lm);

        // double x_scale = (fractalBR.x - fractalTL.x) / (double(vertexArrayBR.x) - double(vertexArrayTL.x));
        // double y_scale = (fractalBR.y - fractalTL.y) / (double(vertexArrayBR.y) - double(vertexArrayTL.y));

        // double y_pos = fractalTL.y;

        // int y_offset = 0;
        // int row_size = screenWidth;

        // int x, y;

        // SIMD_Double _a, _b, _two, _four, _mask1;
        // SIMD_Double _zr, _zi, _zr2, _zi2, _cr, _ci;
        // SIMD_Double _x_pos_offsets, _x_pos, _x_scale, _x_jump;
        // SIMD_Integer _one, _c, _n, _iterations, _mask2;

        // _one = _mm256_set1_epi64x(1);
        // _two = _mm256_set1_pd(2.0);
        // _four = _mm256_set1_pd(4.0);
        // _iterations = _mm256_set1_epi64x(iterations);

        // _x_scale = _mm256_set1_pd(x_scale);
        // _x_jump = _mm256_set1_pd(x_scale * 4);
        // _x_pos_offsets = _mm256_set_pd(0, 1, 2, 3);
        // _x_pos_offsets = _mm256_mul_pd(_x_pos_offsets, _x_scale);

        // for (y = vertexArrayTL.y; y < vertexArrayBR.y; y++)
        // {
        //     // Reset x_position
        //     _a = _mm256_set1_pd(fractalTL.x);
        //     _x_pos = _mm256_add_pd(_a, _x_pos_offsets);

        //     _ci = _mm256_set1_pd(y_pos);

        //     for (x = vertexArrayTL.x; x < vertexArrayBR.x; x += 4)
        //     {
        //         _cr = _x_pos;
        //         _zr = _mm256_setzero_pd();
        //         _zi = _mm256_setzero_pd();
        //         _n = _mm256_setzero_si256();

        //     repeat:
        //         _zr2 = _mm256_mul_pd(_zr, _zr);
        //         _zi2 = _mm256_mul_pd(_zi, _zi);
        //         _a = _mm256_sub_pd(_zr2, _zi2);
        //         _a = _mm256_add_pd(_a, _cr);
        //         _b = _mm256_mul_pd(_zr, _zi);
        //         _b = _mm256_fmadd_pd(_b, _two, _ci);
        //         _zr = _a;
        //         _zi = _b;
        //         _a = _mm256_add_pd(_zr2, _zi2);
        //         _mask1 = _mm256_cmp_pd(_a, _four, _CMP_LT_OQ);
        //         _mask2 = _mm256_cmpgt_epi64(_iterations, _n);
        //         _mask2 = _mm256_and_si256(_mask2, _mm256_castpd_si256(_mask1));
        //         _c = _mm256_and_si256(_one, _mask2); // Zero out ones where n < iterations
        //         _n = _mm256_add_epi64(_n, _c);       // n++ Increase all n
        //         if (_mm256_movemask_pd(_mm256_castsi256_pd(_mask2)) > 0)
        //             goto repeat;

        //         // (*vertexArray)[y_offset + x].color.r = static_cast<sf::Uint8>(fractalTL.x % 255);
        //         // (*vertexArray)[y_offset + x].color.g = static_cast<sf::Uint8>(fractalTL.x % 255);
        //         // (*vertexArray)[y_offset + x].color.b = static_cast<sf::Uint8>(fractalTL.x % 255);

        //         printf("r:%f, g:%f, b%f \n", &_n + sizeof(int) * 0, &_n + sizeof(int) * 1, &_n + sizeof(int) * 2);

        //         memcpy(&(*vertexArray)[y_offset + x + 0].color, &_n + sizeof(int) * 0, sizeof(int));
        //         memcpy(&(*vertexArray)[y_offset + x + 1].color, &_n + sizeof(int) * 1, sizeof(int));
        //         memcpy(&(*vertexArray)[y_offset + x + 2].color, &_n + sizeof(int) * 2, sizeof(int));

        //         (*vertexArray)[y_offset + x].color.a = static_cast<sf::Uint8>(255);
        //         _x_pos = _mm256_add_pd(_x_pos, _x_jump);
        //     }

        //     y_pos += y_scale;
        //     y_offset += row_size;
        // }

        double x_scale = (fractalBR.x - fractalTL.x) / (static_cast<double>(fractalImageSize.x));
        double y_scale = (fractalBR.y - fractalTL.y) / (static_cast<double>(fractalImageSize.y));

        for (int y = 0; y < fractalImageSize.y; y++)
        {
            for (int x = 0; x < fractalImageSize.x; x++)
            {
                std::complex<double> c(x * x_scale + fractalTL.x, y * y_scale + fractalTL.y);
                std::complex<double> z(0.0, 0.0);

                int n = 0;
                while (abs(z) < 2.0 && n < iterations)
                {
                    z = (z * z) + c;
                    n++;
                }

                // printf(" %d ", n);

                fractalArray[y * Window::GetWidth() + x] = n;
            }
        }

        m_nWorkerComplete++;
    }
}