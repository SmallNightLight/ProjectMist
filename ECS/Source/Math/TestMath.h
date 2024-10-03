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

        std::cout << "Testing TestMath" << std::endl;

        std::cout  << std::setprecision(10)<< "1: " << GetFixed16_16(32767, 32767) << std::endl;
        std::cout << "2: " << Fixed8_8::FromFixed(120, 120) << std::endl;
        std::cout << "3: " << GetFixed16_16(16) << std::endl;

        std::cout << "3: " << Fixed16_16::from_fixed_point<100000>(100000) << std::endl;




        std::cout << "I1: " << static_cast<int>(1.1f) << std::endl;
        std::cout << "I2: " << static_cast<int>(1.5f) << std::endl;
        std::cout << "I3: " << static_cast<int>(1.9f) << std::endl;
        std::cout << "I4: " << static_cast<int>(1.999f) << std::endl;
        std::cout << "I5: " << static_cast<int>(2.0000f) << std::endl;

        std::cout << "T1: " << (1 << 16) << std::endl;
        std::cout << "T1: " << IntPow(3,3) << std::endl;
        std::cout << "T1: " << IntPow(-3,3) << std::endl;
        std::cout << "T1: " << GetDigits(254) << std::endl;
        std::cout << "T1: " << GetDigits(-254) << std::endl;



        Fixed16_16 fixed8 {1.6};
        assert(fixed8 == (Fixed16_16{3.2} / 2));
        fixed8 += Fixed16_16{1};
        assert(fixed8 == Fixed16_16{2.6});

        std::cout << "All tests passed" << std::endl;
        return 0;
    }
};