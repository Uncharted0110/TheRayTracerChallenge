#include <iostream>
#include <vector>
#include <fstream>
#include <algorithm>
#include "ColorCanvas.h"

Color::Color() {}

Color::Color(float r, float g, float b) : red(r), green(g), blue(b) {}

Color Color::operator+(const Color& other) const {
    return Color(red + other.red, green + other.green, blue + other.blue);
}

Color Color::operator-(const Color& other) const {
    return Color(red - other.red, green - other.green, blue - other.blue);
}

Color Color::operator*(const Color& other) const {
    return Color(red * other.red, green * other.green, blue * other.blue);
}

Color Color::operator*(float scalar) const {
    return Color(red * scalar, green * scalar, blue * scalar);
}

void Color::PrintColor() {
    std::cout << red << " " << green << " " << blue << std::endl;
}

Canvas::Canvas(int w, int h) : width(w), height(h) {
    pixels.resize(height, std::vector<Color>(width, Color(0, 0, 0))); // Initialize pixels
}

void Canvas::Write_Pixel(int x, int y, Color c) {
    if (x >= 0 && x < width && y >= 0 && y < height)
        pixels[y][x] = c;
    else
        std::cout << "Out of bounds" << std::endl;
}

Color Canvas::Pixel_At(int x, int y) {
    if (x >= 0 && x < width && y >= 0 && y < height)
        return pixels[y][x];
    else {
        std::cout << "Out of bounds" << std::endl;
        return Color(0.0f, 0.0f, 0.0f);
    }
}

void Canvas::SetBackground(Color c) {
    for (int y = 0; y < height; y++)
        for (int x = 0; x < width; x++)
            pixels[y][x] = c;
}

PPM::PPM(Canvas c, std::string filename) : width(c.width), height(c.height) {
    Canvas_to_PPM(c, filename);
}

void PPM::Canvas_to_PPM(Canvas c, const std::string filename) {
    std::ofstream file(filename);

    if (!file) {
        std::cout << "Error in file creation" << std::endl;
        return;
    }

    file << "P3\n";
    file << width << " " << height << "\n";
    file << maxColorValue << "\n";

    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            Color col = c.Pixel_At(x, y);
            col = Format_RGB(col);
            file << col.red << " " << col.green << " " << col.blue << " ";
        }
        file << "\n";
    }

    file.close();
}

float Clamp_0_to_1(float& val) { // Pass by value
    return std::max(0.0f, std::min(val, 1.0f));
}

Color Format_RGB(Color c) {
    Color& new_col = c;

    new_col.red = Clamp_0_to_1(new_col.red);
    new_col.green = Clamp_0_to_1(new_col.green);
    new_col.blue = Clamp_0_to_1(new_col.blue);

    new_col.red = static_cast<int>(new_col.red * 255.0f + 0.5f);
    new_col.green = static_cast<int>(new_col.green * 255.0f + 0.5f);
    new_col.blue = static_cast<int>(new_col.blue * 255.0f + 0.5f);

    return c; // Return the modified color
}


Color MultiplyColor(Color a, Color b) {
    return Color(a.red * b.red, a.green * b.green, a.blue * b.blue);
}

Color MultiplyColorFactor(Color a, float factor) {
    return Color(a.red * factor, a.green * factor, a.blue * factor);
}

bool ColorEquality(Color a, Color b) {
    if (a.red == b.red)
        if (a.blue == b.blue)
            if (a.green == b.green)
                return true;
    return false;
}