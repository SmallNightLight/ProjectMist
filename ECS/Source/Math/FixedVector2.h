#pragma once

template<class T>
class FixedVector2
{
      T X;
      T Y;

      FixedVector2() : X(T(0)), Y(T(0)) { };
      FixedVector2(const T& x, const T& y) : X(x), Y(y) { }
};