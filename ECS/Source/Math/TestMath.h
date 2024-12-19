#pragma once

#pragma optimize("", off)

#include "FixedTypes.h"
#include "FixedLib/include/fixedmath/fixed_math.hpp"
#include "FixedLib/include/fixedmath/iostream.h"

#include <iostream>
#include <iomanip>
#include <cassert>
#include <chrono>

#define FIXEDMATH_ENABLE_SQRT_ABACUS_ALGO

using fixedmath::operator""_fix;

class TestMath
{
public:
    TestMath() = default;

    static int Test()
    {
        //Fixed numbers
        std::cout << "Testing TestMath" << std::endl;

        std::cout << std::setprecision(10) << "32767,32767: " << Fixed16_16::FromFixed(32767, 32767) << std::endl;
        std::cout << "-120,12: " << Fixed8_8::FromFixed(-120, 12) << std::endl;
        std::cout << "-0.5: " << Fixed8_8::FromFixed(0, -5) << std::endl;
        std::cout << "0.9: " << Fixed8_8::FromFixed(0, 9) << std::endl;

        Fixed16_16 fixed8 = Fixed16_16::FromFixed(1, 5015);
        assert(fixed8 == Fixed16_16::FromFixed(180, 18) / Fixed16_16::FromFixed(120, 0));
        fixed8 += Fixed16_16::FromFixed(10, 76);
        assert(fixed8 == Fixed16_16::FromFixed(12, 2615));

        //Vector2
        const Vector2 vecA = Vector2::Zero();
        assert(vecA.X == 0 && vecA.Y == 0);
        const Vector2 vecB = Vector2(5, 1);

        assert(vecB.X == 5);
        assert(vecB.Y == 1);
        assert(vecB.X > vecB.Y);
        assert(vecB.X >= vecB.Y);
        assert(vecB.Y < vecB.X);
        assert(vecB.Y <= vecB.X);

        const Vector2 vecC(0, 5, 1, 5);
        std::cout << "(0.5, 1.5):" << vecC << std::endl;
        assert(vecC.X != 1 && vecC.Y != 0);
        assert(vecC.X == Fixed16_16::FromFixed(0, 5));
        assert(vecC.Y == Fixed16_16::FromFixed(1, 5));

        const Vector2 vecD(Fixed16_16::FromFixed(10, 1), Fixed16_16::FromFixed(-2, 9));
        assert(vecD.X != vecD.Y);
        assert(vecD.X == Fixed16_16::FromFixed(10, 1));
        assert(vecD.Y == Fixed16_16::FromFixed(-2, 9));
        assert(vecD.Y != Fixed16_16::FromFixed(-2, 8));

        Vector2 vecE = vecC + vecD;
        std::cout << "(10.6, -1.4):" << vecE << std::endl;
        assert(vecE.X == Fixed16_16::FromFixed(10, 6));
        assert(vecE.Y == Fixed16_16::FromFixed(-1, 4));

        Vector2 vecF = vecC - vecD;
        std::cout << "(-9.6, 4.4):" << vecF << std::endl;
        assert(vecF.X == Fixed16_16::FromFixed(-9, 6));
        assert(vecF.Y == Fixed16_16::FromFixed(4, 4));

        Vector2 vecG = vecF / 2;
        std::cout << "(-4.8, 2.2):" << vecG << std::endl;
        assert(vecG.X == Fixed16_16::FromFixed(-4, 8));
        assert(vecG.Y == Fixed16_16::FromFixed(2, 2));

        Vector2 vecU (5, 2);
        Vector2 vecI (6, 3);

        vecU += vecI;
        assert(vecU == Vector2(11, 5));
        vecU -= Vector2(3, 3);
        ++vecU;
        vecU++;
        assert(vecU == Vector2(10, 4));

        std::cout << "All tests passed" << std::endl;

        std::cout << "\nTesting number addition speed" << std::endl;

        //Fixed point with rounding
        auto start = std::chrono::high_resolution_clock::now();

        Fixed16_16Rounding x1(0);
        Fixed16_16Rounding x2 = Fixed16_16Rounding(1) / Fixed16_16Rounding(1000);
        for (int i = 0; i < 1000000; ++i)
        {
            x1 += x2;
        }

        auto end = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double> elapsed_seconds = end - start;
        std::cout << std::setprecision(10) << "Time to add 1.000.000 fixed numbers (with rounding): " << elapsed_seconds.count() << " seconds, Offset: " << fpm::abs(x1 - 1000) << std::endl;

        //Fixed point - No rounding
        start = std::chrono::high_resolution_clock::now();

        Fixed16_16NoRounding y1(0);
        Fixed16_16NoRounding y2 = Fixed16_16NoRounding(1) / Fixed16_16NoRounding(1000);
        for (int i = 0; i < 1000000; ++i)
        {
            y1 += y2;
        }

        end = std::chrono::high_resolution_clock::now();
        elapsed_seconds = end - start;
        std::cout << std::setprecision(10) << "Time to add 1.000.000 fixed numbers (no rounding): " << elapsed_seconds.count() << " seconds, Offset: " << fpm::abs(y1 - 1000) << std::endl;

        //Floating point
        start = std::chrono::high_resolution_clock::now();

        float z1(0);
        float z2 = 0.001f;
        for (int i = 0; i < 1000000; ++i)
        {
            z1 += z2;
        }

        end = std::chrono::high_resolution_clock::now();
        elapsed_seconds = end - start;
        std::cout << std::setprecision(10) << "Time to add 1.000.000 floating point numbers: " << elapsed_seconds.count() << " seconds, Offset: " << std::abs(z1 - 1000) << std::endl;


        std::cout << "\nTesting number multiplication / division speed" << std::endl;

        //Fixed point with rounding
        start = std::chrono::high_resolution_clock::now();

        Fixed16_16Rounding a1 = Fixed16_16Rounding(1, 2);
        Fixed16_16Rounding a2 = Fixed16_16Rounding(1, 2);
        for (int i = 0; i < 1000000; ++i)
        {
            a1 *= a2;
            a1 /= a2;
        }

        end = std::chrono::high_resolution_clock::now();
        elapsed_seconds = end - start;
        std::cout << std::setprecision(10) << "Time to multiply and divide 1.000.000 fixed numbers (with rounding): " << elapsed_seconds.count() << " seconds, Offset: " << fpm::abs(a1 - a2) << std::endl;

        //Fixed point - No rounding
        start = std::chrono::high_resolution_clock::now();

        Fixed16_16NoRounding b1 = Fixed16_16NoRounding(1, 2);
        Fixed16_16NoRounding b2 = Fixed16_16NoRounding(1, 2);
        for (int i = 0; i < 1000000; ++i)
        {
            b1 *= b2;
            b1 /= b2;
        }

        end = std::chrono::high_resolution_clock::now();
        elapsed_seconds = end - start;
        std::cout << std::setprecision(10) << "Time to multiply and divide 1.000.000 fixed numbers (no rounding): " << elapsed_seconds.count() << " seconds, Offset: " << fpm::abs(b1 - b2) << std::endl;

        //Floating point
        start = std::chrono::high_resolution_clock::now();

        float c1(1.2);
        float c2 = 1.2f;
        for (int i = 0; i < 1000000; ++i)
        {
            c1 *= c2;
            c1 /= c2;
        }

        end = std::chrono::high_resolution_clock::now();
        elapsed_seconds = end - start;
        std::cout << std::setprecision(10) << "Time to multiply and divide 1.000.000 floating point numbers: " << elapsed_seconds.count() << " seconds, Offset: " << std::abs(c1 - c2) << std::endl;


        std::cout << "\nTesting fixed point vector2 addition speed" << std::endl;

        //Fixed point vector2
        start = std::chrono::high_resolution_clock::now();

        Vector2 v1 (0, 0);
        Vector2 v2 = Vector2(Fixed16_16(1) / Fixed16_16(1000), Fixed16_16(1) / Fixed16_16(1000));
        for (int i = 0; i < 1000000; ++i)
        {
            v1 += v2;
        }

        end = std::chrono::high_resolution_clock::now();
        elapsed_seconds = end - start;
        std::cout << std::setprecision(10) << "Time to add 1.000.000 fixed number vector2s (no rounding - default): " << elapsed_seconds.count() << " seconds, Offset: " << (v1 - Vector2(1000, 1000)).Positive() << std::endl;

        Vector2 v(-1000, 1000);
        std::cout << "Magnitude (1414,2135): " << v.Magnitude() << std::endl;

        Fixed8_8 f8 = Fixed8_8(30, 6);
        std::cout << f8 << std::endl;
        Fixed16_16 f16 = Fixed16_16(f8);
        std::cout << f16 << std::endl;

        fixedmath::fixed_t n1 = fixedmath::fixed_t(10000000) + fixedmath::fixed_t(0.00001f);
        std::cout << n1 << std::endl;
        std::cout << fixedmath::sqrt(n1) << std::endl;




        ///////////////
        //Comparison with fixedmath lib
        //Fixed numbers
        std::cout << "Testing TestMath" << std::endl;

        Fixed vvv = 65537_fix;

        Fixed fixed81 = 1.5015_fix;
        assert(fixed81 == 180.18_fix / 120.0_fix);
        fixed81 += 10.76_fix;
        //assert(fixed81 == 12.2615_fix);

        std::cout << "\nTesting number addition speed" << std::endl;

        //Fixed point with rounding
        auto start1 = std::chrono::high_resolution_clock::now();

        Fixed x11(0);
        Fixed x21 = 1.0_fix / 1000.0_fix;
        for (int i = 0; i < 1000000; ++i)
        {
            x11 += x21;
        }

        auto end1 = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double> elapsed_seconds1 = end1 - start1;
        std::cout << std::setprecision(10) << "Time to add 1.000.000 fixed lib: " << elapsed_seconds1.count() << " seconds, Offset: " << fixedmath::abs(x11 - 1000.0_fix) << std::endl;


        std::cout << "\nTesting number multiplication / division speed" << std::endl;

        //Fixed point with rounding
        start1 = std::chrono::high_resolution_clock::now();

        Fixed a11 = 1.2_fix;
        Fixed a21 = 1.2_fix;
        for (int i = 0; i < 1000000; ++i)
        {
            a11 *= a21;
            a11 /= a21;
        }

        end1 = std::chrono::high_resolution_clock::now();
        elapsed_seconds1 = end1 - start1;
        std::cout << std::setprecision(10) << "Time to multiply and divide 1.000.000 fixed lib: " << elapsed_seconds1.count() << " seconds, Offset: " << fixedmath::abs(a11 - a21) << std::endl;


        //Comparing square root
        std::cout << "\nTesting square root" << std::endl;

        //Fixed lib
        start1 = std::chrono::high_resolution_clock::now();

        Fixed g = 1000.5498_fix;
        Fixed h;
        for (int i = 0; i < 1000000; ++i)
        {
            h = fixedmath::detail::sqrt_std_math(g);
        }

        end1 = std::chrono::high_resolution_clock::now();
        elapsed_seconds1 = end1 - start1;
        std::cout << std::setprecision(10) << "Time to square root 1.000.000 fixed lib: " << elapsed_seconds1.count() << " seconds, Result: " << h << std::endl;

        //FPM
        start1 = std::chrono::high_resolution_clock::now();

        Fixed16_16 u = Fixed16_16::FromFixed(1000, 5498);
        Fixed16_16 o;
        for (int i = 0; i < 1000000; ++i)
        {
            //o = Fixed16_16::from_raw_value(fpm::sqrt(u.raw_value()));
            o = fpm::sqrt(u);
        }

        end1 = std::chrono::high_resolution_clock::now();
        elapsed_seconds1 = end1 - start1;
        std::cout << std::setprecision(10) << "Time to square root 1.000.000 fpm: " << elapsed_seconds1.count() << " seconds, Result: " << o << std::endl;


        //Float sqrt
        //Fixed lib
        start1 = std::chrono::high_resolution_clock::now();

        float b = 1000.5498f;
        float l;
        for (int i = 0; i < 1000000; ++i)
        {
            l = std::sqrt(b);
        }

        end1 = std::chrono::high_resolution_clock::now();
        elapsed_seconds1 = end1 - start1;
        std::cout << std::setprecision(10) << "Time to square root 1.000.000 float: " << elapsed_seconds1.count() << " seconds, Result: " << l << std::endl;


        return 0;
    }
};

#pragma optimize("", on)