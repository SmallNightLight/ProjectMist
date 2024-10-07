#pragma once

#include "FPM/math.hpp"
#include "FPM/fixed.hpp"
#include "FixedTypes.h"
#include <type_traits>

template<class T, typename IntegerType, typename FractionType>
struct  FixedVector2
{
      T X;
      T Y;

      constexpr inline FixedVector2() : X(T(0)), Y(T(0)) { }
      constexpr inline FixedVector2(const T x, const T y) : X(x), Y(y) { }
      constexpr inline FixedVector2(const IntegerType x, const IntegerType y) : X(x), Y(y) { }
      constexpr inline FixedVector2(const IntegerType integer1, const FractionType fraction1, const IntegerType integer2, const FractionType fraction2) : X(T(integer1, fraction1)), Y(T(integer2, fraction2)) { }

      //Basic math operations (with Fi
      FixedVector2 operator+(const FixedVector2 other) const
      {
            return FixedVector2(X + other.X, Y + other.Y);
      }

      FixedVector2 operator-(const FixedVector2 other) const
      {
            return FixedVector2(X - other.X, Y - other.Y);
      }

      FixedVector2 operator*(const T scalar) const
      {
            return FixedVector2(X * scalar, Y * scalar);
      }

      FixedVector2 operator*(const IntegerType scalar) const
      {
            return FixedVector2(X * scalar, Y * scalar);
      }

      FixedVector2 operator/(const T scalar) const
      {
            return FixedVector2(X / scalar, Y / scalar);
      }

      FixedVector2 operator/(const IntegerType scalar) const
      {
            if (scalar == 0)
            {
                  throw std::runtime_error("Division by zero");
            }
            return FixedVector2(X / scalar, Y / scalar);
      }

      //Prefix / Postfix operators
      FixedVector2 operator++()
      {
            ++X;
            ++Y;
            return *this;
      }

      FixedVector2 operator++(int)
      {
            FixedVector2 temp = *this;
            ++(*this);
            return temp;
      }

      FixedVector2 operator--()
      {
            --X;
            --Y;
            return *this;
      }

      FixedVector2 operator--(int)
      {
            FixedVector2 temp = *this;
            --(*this);
            return temp;
      }

      //Compound assignment operators
      FixedVector2 operator+=(const FixedVector2 other)
      {
            X += other.X;
            Y += other.Y;
            return *this;
      }

      FixedVector2 operator-=(const FixedVector2 other)
      {
            X -= other.X;
            Y -= other.Y;
            return *this;
      }

      FixedVector2 operator*=(const T scalar)
      {
            X *= scalar;
            Y *= scalar;
            return *this;
      }

      FixedVector2 operator*=(const IntegerType scalar)
      {
            X *= scalar;
            Y *= scalar;
            return *this;
      }

      FixedVector2 operator/=(const T scalar)
      {
            X /= scalar;
            Y /= scalar;
            return *this;
      }

      FixedVector2 operator/=(const IntegerType scalar)
      {
            X /= scalar;
            Y /= scalar;
            return *this;
      }

      //Math functions
      T Dot(const FixedVector2 other) const
      {
            return X * other.X + Y * other.Y;
      }

      T Cross(const FixedVector2 other) const
      {
            return X * other.Y - Y * other.X;
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

      T Distance(const FixedVector2 other) const
      {
            T dx = X - other.X;
            T dy = Y - other.Y;
            return fpm::sqrt(dx * dx + dy * dy);
      }

      FixedVector2 ProjectOnto(const FixedVector2 other) const
      {
            T dotProduct = this->Dot(other);
            T otherMagnitudeSquared = other.X * other.X + other.Y * other.Y;
            return other * (dotProduct / otherMagnitudeSquared);
      }

      FixedVector2 Perpendicular() const
      {
            return FixedVector2(-Y, X);
      }

      FixedVector2 Rotate(const T angle) const
      {
            T cosAngle = fpm::cos(angle);
            T sinAngle = fpm::sin(angle);
            return FixedVector2(X * cosAngle - Y * sinAngle, X * sinAngle + Y * cosAngle);
      }

      T AngleBetween(const FixedVector2 other) const
      {
            T dotProduct = this->Dot(other);
            T magnitudes = this->Magnitude() * other.Magnitude();
            return fpm::acos(dotProduct / magnitudes);
      }

      FixedVector2 Reflect(const FixedVector2 normal) const
      {
            return *this - normal * (2 * this->Dot(normal));
      }

      FixedVector2 ClampMagnitude(const T maxMagnitude) const
      {
            T magnitude = Magnitude();
            if (magnitude > maxMagnitude)
            {
                  return Normalize() * maxMagnitude;
            }
            return *this;
      }

      FixedVector2 ClampMagnitudeMin(const T minMagnitude) const
      {
            T magnitude = Magnitude();
            if (magnitude < minMagnitude)
            {
                  return Normalize() * minMagnitude;
            }
            return *this;
      }

      FixedVector2 Lerp(const FixedVector2 other, const T t) const
      {
            return *this + (other - *this) * t;
      }

      //Stream functions
      friend std::ostream& operator<<(std::ostream& os, const FixedVector2& vector)
      {
            os << "(" << vector.X << ", " << vector.Y << ")";
            return os;
      }

      //Comparison operators
      bool operator==(const FixedVector2 other) const
      {
            return X == other.X && Y == other.Y;
      }

      bool operator!=(const FixedVector2 other) const
      {
            return !(*this == other);
      }

      bool operator<(const FixedVector2 other) const
      {
            return Magnitude() < other.Magnitude();
      }

      bool operator>(const FixedVector2 other) const
      {
            return Magnitude() > other.Magnitude();
      }

      bool operator<=(const FixedVector2 other) const
      {
            return Magnitude() <= other.Magnitude();
      }

      bool operator>=(const FixedVector2 other) const
      {
            return Magnitude() >= other.Magnitude();
      }

      static FixedVector2 Zero()
      {
            return FixedVector2(T(0), T(0));
      }
};