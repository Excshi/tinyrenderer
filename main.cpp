#include <cmath>
#include "tgaimage.h"
#include <iostream>
#include <random>
#include <fstream>
#include <sstream>

constexpr TGAColor white   = {255, 255, 255, 255}; // attention, BGRA order
constexpr TGAColor green   = {  0, 255,   0, 255};
constexpr TGAColor red     = {  0,   0, 255, 255};
constexpr TGAColor blue    = {255, 128,  64, 255};
constexpr TGAColor yellow  = {  0, 200, 255, 255};

void draw_line(int ax, int ay, int bx, int by, TGAColor color, TGAImage& framebuffer) {
    // float version is faster
    bool steep = std::abs(ay-by) > std::abs(ax-bx);
    if (steep) {
        std::swap(ax, ay);
        std::swap(bx, by);
    }

    if (ax > bx) {
        std::swap(ax, bx);
        std::swap(ay, by);
    }

    float yt = ay;
    float inc = (by - ay) / static_cast<float>(bx - ax);
    for (int x = ax; x <= bx; x++) {
        if (steep) framebuffer.set(yt + 0.5f, x, color);
        else framebuffer.set(x, yt + 0.5f, color);

        yt += inc;
    }
    
    // Bresenham
    // bool steep = std::abs(ay-by) > std::abs(ax-bx);
    // if (steep) {
    //     std::swap(ax, ay);
    //     std::swap(bx, by);
    // }

    // if (ax > bx) {
    //     std::swap(ax, bx);
    //     std::swap(ay, by);
    // }

    // int yt = ay;
    // int error = 0;
    // for (int x = ax; x <= bx; x++) {
    //     if (steep) framebuffer.set(yt, x, color);
    //     else framebuffer.set(x, yt, color);

    //     error += 2 * std::abs(by - ay);
    //     // slight branchless optimization
    //     yt += (by > ay ? 1 : -1) * (error > (bx - ax));
    //     error -= (2 * (bx - ax)) * (error > (bx - ax));
    // }
}

void read_obj(std::string filename) {
    std::ifstream obj_file(filename);

    if (!obj_file.is_open()) {
        std::cerr << "Error: Cannot open file" << std::endl;
        return;
    }

    std::string line;
    while (std::getline(obj_file, line)) {
        std::istringstream iss(line);
        std::string token;

        while (iss >> token) {
            std::cout << token << std::endl;
        }
    }

    return;
}

int main(int argc, char** argv) {
    constexpr int width  = 64;
    constexpr int height = 64;
    TGAImage framebuffer(width, height, TGAImage::RGB);

    // stress test
    // std::random_device rd; // seed
    // std::mt19937 gen(rd()); // generator
    // std::uniform_int_distribution<> width_dist(0, width-1); // dist
    // std::uniform_int_distribution<> height_dist(0, height-1);
    // std::uniform_int_distribution<> color_dist(0, 255);

    // for (int i=0; i<(1<<24); i++) {
    //     int ax = width_dist(gen), ay = height_dist(gen);
    //     int bx = width_dist(gen), by = height_dist(gen);
    //     draw_line(ax, ay, bx, by, { static_cast<u_int8_t>(color_dist(gen)), static_cast<u_int8_t>(color_dist(gen)), static_cast<u_int8_t>(color_dist(gen)), static_cast<u_int8_t>(color_dist(gen)) }, framebuffer);
    // }

    // draw triangle
    // int ax =  7, ay =  3;
    // int bx = 12, by = 37;
    // int cx = 62, cy = 53;

    // // a to b
    // draw_line(ax, ay, bx, by, blue, framebuffer);
    // // c to b
    // draw_line(cx, cy, bx, by, green, framebuffer);
    // // c to a
    // draw_line(cx, cy, ax, ay, yellow, framebuffer);
    // // a to c
    // draw_line(ax, ay, cx, cy, red, framebuffer);

    // framebuffer.set(ax, ay, white);
    // framebuffer.set(bx, by, white);
    // framebuffer.set(cx, cy, white);

    // wireframe rendering
    read_obj("../obj/diablo3_pose/diablo3_pose.obj");

    framebuffer.write_tga_file("framebuffer.tga");
    return 0;
}

