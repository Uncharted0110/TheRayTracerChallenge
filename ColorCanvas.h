#ifndef COLORCANVAS_H
#define COLORCANVAS_H

#include <iostream>
#include <vector>
#include <string>

class Color {
public:
    float red, green, blue;

    Color();
    Color(float r, float g, float b);

    Color operator + (const Color& other) const;
    Color operator-(const Color& other) const;
    Color operator*(const Color& other) const;
    Color operator*(float scalar) const;
    void PrintColor();
};

class Canvas {
public:
    int width, height;
    std::vector<std::vector<Color>> pixels;

    Canvas(int w, int h);
    void Write_Pixel(int x, int y, Color c);
    Color Pixel_At(int x, int y);
    void SetBackground(Color c);
};

class PPM {
public:
    int width, height;
    const int maxColorValue = 255;

    PPM(Canvas c, std::string filename); // Changed to string by value
    void Canvas_to_PPM(Canvas c, const std::string filename);
};

float Clamp_0_to_1(float& val); // Changed to pass by value
Color Format_RGB(Color c);
Color MultiplyColor(Color a, Color b);
Color MultiplyColorFactor(Color a, float factor);
bool ColorEquality(Color a, Color b);

#endif // !COLORCANVAS_H
