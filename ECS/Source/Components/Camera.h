#pragma once
#include "../Math/FixedTypes.h"

struct Camera
{
    Fixed16_16 Width, Height;
    Fixed16_16 ZoomLevel;
    Vector2 Position;

    Fixed16_16 Left, Right, Top, Bottom;

    Camera() : Width(0), Height(0), ZoomLevel(0), Position(0, 0), Left(0), Right(0), Top(0), Bottom(0) { }

    Camera(Fixed16_16 width, Fixed16_16 height, Fixed16_16 zoomLevel = Fixed16_16(1))
        : Width(width), Height(height), ZoomLevel(zoomLevel), Position(0, 0), Left(0), Right(0), Bottom(0), Top(0)
    {
        UpdateView();
    }

    void SetPosition(Vector2 position)
    {
        Position = position;
        UpdateView();
    }

    void SetPosition(Fixed16_16 x, Fixed16_16 y)
    {
        Position.X = x;
        Position.Y = y;
        UpdateView();
    }

    void SetX(Fixed16_16 x)
    {
        Position.X = x;
    }

    void SetY(Fixed16_16 y)
    {
        Position.Y = y;
    }

    void Move(Vector2 amount)
    {
        Position += amount;
        UpdateView();
    }

    void Move(Fixed16_16 x, Fixed16_16 y)
    {
        Position.X += x;
        Position.Y += y;
        UpdateView();
    }

    void SetZoom(Fixed16_16 newZoomLevel)
    {
        ZoomLevel = newZoomLevel;
        UpdateView();
    }

    void SetZoom(Fixed16_16 newZoomLevel, Fixed16_16 minZoom, Fixed16_16 maxZoom)
    {
        ZoomLevel = newZoomLevel;

        if (ZoomLevel < minZoom)
        {
            ZoomLevel = minZoom;
        }
        else if (ZoomLevel > maxZoom)
        {
            ZoomLevel = maxZoom;
        }

        UpdateView();
    }

    void Zoom(Fixed16_16 deltaZoom)
    {
        ZoomLevel += deltaZoom;
        UpdateView();
    }

    void Zoom(Fixed16_16 deltaZoom, Fixed16_16 minZoom, Fixed16_16 maxZoom)
    {
        ZoomLevel += deltaZoom;

        if (ZoomLevel < minZoom)
        {
            ZoomLevel = minZoom;
        }
        else if (ZoomLevel > maxZoom)
        {
            ZoomLevel = maxZoom;
        }

        UpdateView();
    }

private:
    void UpdateView()
    {
        //Calculate the boundaries based on position and zoom
        Fixed16_16 halfWidth = Width / Fixed16_16(2) / ZoomLevel;
        Fixed16_16 halfHeight = Height / Fixed16_16(2) / ZoomLevel;

        Left = Position.X - halfWidth;
        Right = Position.X + halfWidth;
        Bottom = Position.Y - halfHeight;
        Top = Position.Y + halfHeight;
    }
};