#pragma once

#include <array>
#include <cmath>
#include <random>


namespace vob::rng
{
    namespace detail
    {
        float smoothstep(float a_a0, float a_a1, float a_t)
        {
            return a_a0 + (3 * a_t * a_t - 2 * a_t * a_t * a_t) * (a_a1 - a_a0);
        }

        std::array<float, 2> random_gradient(int a_x, int a_y, std::uint32_t a_seed)
        {
            std::mt19937 gen{ a_seed };
            gen = std::mt19937{ gen() + a_y + a_seed };
            gen = std::mt19937{ gen() + a_y + a_seed };
            std::uniform_real_distribution<float> dis{ 0.0f, 2 * 3.1415926535f };

            float r = dis(gen);

            return { std::sin(r), std::cos(r) };
        }

        float dot_grid_gradient(int a_x0, int a_y0, float a_x, float a_y, std::uint32_t a_seed)
        {
            auto gradient = random_gradient(a_x0, a_y0, a_seed);
            float dx = a_x - static_cast<float>(a_x0);
            float dy = a_y - static_cast<float>(a_y0);
            
            return dx * gradient[0] + dy * gradient[1];
        }
    }

    float perlin(float a_x, float a_y, std::uint32_t a_seed = 0)
    {
        int x0 = static_cast<int>(a_x);
        int x1 = x0 + 1;
        int y0 = static_cast<int>(a_y);
        int y1 = y0 + 1;


        float dx = a_x - static_cast<float>(x0);
        float dy = a_y - static_cast<float>(y0);

        float n0 = detail::dot_grid_gradient(x0, y0, a_x, a_y, a_seed);
        float n1 = detail::dot_grid_gradient(x0 + 1, y0, a_x, a_y, a_seed);
        float l0 = detail::smoothstep(n0, n1, dx);

        float n2 = detail::dot_grid_gradient(x0, y0 + 1, a_x, a_y, a_seed);
        float n3 = detail::dot_grid_gradient(x0 + 1, y0 + 1, a_x, a_y, a_seed);
        float l1 = detail::smoothstep(n2, n3, dx);

        return detail::smoothstep(l0, l1, dy);
    }
}