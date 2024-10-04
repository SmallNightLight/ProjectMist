#pragma once

#include "FixedTypes.h"
#include <iostream>
#include <iomanip>
#include <cassert>

class TestMath
{
public:
    TestMath() = default;

    static int Test()
    {
        std::cout << "Testing TestMath" << std::endl;

        std::cout << std::setprecision(10) << "32767,32767: " << Fixed16_16::FromFixed(32767, 32767) << std::endl;
        std::cout << "-120,12: " << Fixed8_8::FromFixed(-120, 12) << std::endl;
        std::cout << "-0.5: " << Fixed8_8::FromFixed(0, -5) << std::endl;
        std::cout << "0.9: " << Fixed8_8::FromFixed(0, 9) << std::endl;

        Fixed16_16 fixed8 = Fixed16_16::FromFixed(1, 5015);
        assert(fixed8 == Fixed16_16::FromFixed(180, 18) / Fixed16_16::FromFixed(120, 0));
        fixed8 += Fixed16_16::FromFixed(10, 76);
        assert(fixed8 == Fixed16_16::FromFixed(12, 2615));

        std::cout << "All tests passed" << std::endl;
        return 0;
    }
};