#pragma once

#include "FPM/math.hpp"

template<class T, typename IntegerType, typename FractionType>
struct  FixedVector2
{
      T X;
      T Y;

      FixedVector2() : X(T(0)), Y(T(0)) { };
      FixedVector2(const T& x, const T& y) : X(x), Y(y) { }
      constexpr inline FixedVector2(IntegerType integer1, FractionType fraction1, IntegerType integer2, FractionType fraction2) : X(T(integer1, fraction1)), Y(T(integer2, fraction2)) { };

      //Basic math operations
      FixedVector2 operator+(const FixedVector2& other) const
      {
            return FixedVector2(X + other.X, Y + other.Y);
      }

      FixedVector2 operator-(const FixedVector2& other) const
      {
            return FixedVector2(X - other.X, Y - other.Y);
      }

      FixedVector2 operator*(const T& scalar) const
      {
            return FixedVector2(X * scalar, Y * scalar);
      }

      T Dot(const FixedVector2& other) const
      {
            return X * other.X + Y * other.Y;
      }

      T Magnitude() const
      {
            return fpm::sqrt(X * X + Y * Y);
      }

      FixedVector2 Normalize() const
      {
            T magnitude = Magnitude();
            if (magnitude == T(0))
            {
                  throw std::runtime_error("Cannot normalize a zero vector");
            }
            return FixedVector2(X / magnitude, Y / magnitude);
      }

      friend std::ostream& operator<<(std::ostream& os, const FixedVector2& vector)
      {
            os << "(" << vector.X << ", " << vector.Y << ")";
            return os;
      }

      //Comparison operators
      bool operator==(const FixedVector2& other) const
      {
            return X == other.X && Y == other.Y;
      }

      bool operator!=(const FixedVector2& other) const
      {
            return !(*this == other);
      }

      static FixedVector2 Zero()
      {
            return FixedVector2(T(0), T(0));
      }
};