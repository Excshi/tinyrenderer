#include <cmath>
#include "tgaimage.h"
#include <iostream>
#include <random>
#include <fstream>
#include <sstream>
#include <vector>
#include <tuple>

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

void draw_obj(std::string filename, int w, int h, TGAImage& framebuffer) {
    std::ifstream obj_file(filename);

    if (!obj_file.is_open()) {
        std::cerr << "Error: Cannot open file" << std::endl;
        return;
    }

    std::vector<std::vector<double>> vertices;
    std::vector<std::vector<double>> textures;
    std::vector<std::vector<double>> normals;
    std::vector<std::tuple<int, int, int>> faces;
    std::vector<std::tuple<int, int, int>> texture_faces;
    std::vector<std::tuple<int, int, int>> normal_faces;

    std::string line;
    while (std::getline(obj_file, line)) {
        std::istringstream iss(line);
        std::string type;

        iss >> type;

        if (type == "v") {
            double x, y, z;
            iss >> x >> y >> z;
            vertices.push_back({(x+1)/2 * w + 0.5, (y+1)/2 * h + 0.5, z});
        } else if (type == "vt") {
            double x, y, z;
            iss >> x >> y >> z;
            textures.push_back({(x+1)/2 * w + 0.5, (y+1)/2 * h + 0.5, z});
        } else if (type == "vn") {
            double x, y, z;
            iss >> x >> y >> z;
            normals.push_back({(x+1)/2 * w + 0.5, (y+1)/2 * h + 0.5, z});
        } else if (type == "f") {
            std::tuple<int, int, int> face;
            std::tuple<int, int, int> texture_face;
            std::tuple<int, int, int> normal_face;
            std::string buff;
            iss.ignore(1);
            
            std::getline(iss, buff, '/');
            std::get<0>(face) = std::stoi(buff);
            std::getline(iss, buff, '/');
            std::get<0>(texture_face) = std::stoi(buff);
            std::getline(iss, buff, ' ');
            std::get<0>(normal_face) = std::stoi(buff);

            std::getline(iss, buff, '/');
            std::get<1>(face) = std::stoi(buff);
            std::getline(iss, buff, '/');
            std::get<1>(texture_face) = std::stoi(buff);
            std::getline(iss, buff, ' ');
            std::get<1>(normal_face) = std::stoi(buff);

            std::getline(iss, buff, '/');
            std::get<2>(face) = std::stoi(buff);
            std::getline(iss, buff, '/');
            std::get<2>(texture_face) = std::stoi(buff);
            std::getline(iss, buff, ' ');
            std::get<2>(normal_face) = std::stoi(buff);
            
            faces.push_back(face);
            texture_faces.push_back(texture_face);
            normal_faces.push_back(normal_face);

        } else if (type == "#") {
            std::cout << line << std::endl;
        }
        
    }

    std::cout << "Vertices vector size: " << vertices.size() << std::endl;

    for (int i = 0; i < 10; i++) {
        std::cout << "Vertice " << i+1 << ": " << vertices[i][0] << " " << vertices[i][1] << " " << vertices[i][2] << std::endl;
    }

    for (int i = 0; i < 10; i++) {
        std::cout << "Face " << i+1 << ": " << std::get<0>(faces[i]) << " " << std::get<1>(faces[i]) << " " << std::get<2>(faces[i]) << std::endl;
    }

    // draw faces
    for (const auto& face : faces) {
        std::vector<double> v1 = vertices[std::get<0>(face)-1];
        std::vector<double> v2 = vertices[std::get<1>(face)-1];
        std::vector<double> v3 = vertices[std::get<2>(face)-1];

        // v1 to v2
        draw_line(v1[0], v1[1], v2[0], v2[1], red, framebuffer);
        // v3 to v2
        draw_line(v3[0], v3[1], v2[0], v2[1], red, framebuffer);
        // v3 to v1
        draw_line(v3[0], v3[1], v1[0], v1[1], red, framebuffer);
    }

    // draw vertices
    for (const auto& vertice : vertices) {
        framebuffer.set(vertice[0], vertice[1], white);
    }

    return;
}

int main(int argc, char** argv) {
    constexpr int scale = 10;
    constexpr int width  = 64 * scale;
    constexpr int height = 64 * scale;
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
    draw_obj("../obj/diablo3_pose/diablo3_pose.obj", width, height, framebuffer);

    framebuffer.write_tga_file("framebuffer.tga");
    return 0;
}

