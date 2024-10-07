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
        assert(vecU == Vector2(8, 2));


        std::cout << "All tests passed" << std::endl;
        return 0;
    }
};