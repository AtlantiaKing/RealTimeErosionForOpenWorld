#pragma once

#include <chrono>
#include <iostream>

namespace Erosion
{
    class Timer
    {
    public:
        void Start()
        {
            m_MeasureStart = std::chrono::high_resolution_clock::now();
        }
        void End()
        {
            auto end = std::chrono::high_resolution_clock::now();

            auto elapsed = end - m_MeasureStart;
            std::cout << "this took:\n";
            std::cout << std::chrono::duration_cast<std::chrono::milliseconds>(elapsed).count() << " milliseconds\n";
            std::cout << std::chrono::duration_cast<std::chrono::microseconds>(elapsed).count() << " microseconds\n";
            std::cout << std::chrono::duration_cast<std::chrono::nanoseconds>(elapsed).count() << " nanoseconds\n";
        }

    private:
        std::chrono::time_point<std::chrono::high_resolution_clock> m_MeasureStart{};
    };
}