#include <cstdio>
#include <cstdlib>
#include <cmath>
#include <vector>
#include <memory>
#include "light.hpp"
using std::vector;
using std::unique_ptr;
using namespace MLight;
color* image;
int W = 1000;
int H = 1000;
constexpr int max_sample_num = 64;
constexpr float PI = 3.1415926535f;
constexpr int max_iterations = 64;
constexpr float eps = 1e-6f;
float get_rand(float min = 0., float max = 1.) {
    return min + (rand() / RAND_MAX) * (max - min);
}

vector<unique_ptr<Light>> lights;

// color trace(float x, float y) {

// }

struct result { color emissive; float sd; };

result unionOp(result a, result b) {
    return a.sd < b.sd ? a : b;
}

result intersectOp(result a, result b) {
    result r = a.sd > b.sd ? b : a;
    r.sd = a.sd > b.sd ? a.sd : b.sd;
    return r;
}

result subtractOp(result a, result b) {
    result r = a.sd > b.sd ? b : a;
    r.sd = (a.sd > -b.sd) ? a.sd : -b.sd;
    return r;
}

result scene(pointf p) {
    if(lights.size() == 1) return {lights[0]->getColor(), lights[0]->getSDF(p)};
    result a = {lights[0]->getColor(), lights[0]->getSDF(p)};
    result b = {lights[1]->getColor(), lights[1]->getSDF(p)};
    // return unionOp(a, b);
    // return intersectOp(a, b);
    return subtractOp(a, b);
    // return subtractOp(b, a);
}

color marching(float x, float y, float dx, float dy){
    float t = 0.0f;
    for (int i = 0; i < max_iterations && t < 2.f; i++) {
        result r = scene(pointf(x + dx * t, y + dy * t));
        if (r.sd < eps) return r.emissive;
        t += r.sd;
    }
    return color(0, 0, 0);
}

color sample(float x, float y) {
    int sr = 0, sg = 0, sb = 0;
    for(int i = 0; i < max_sample_num; i++) {
        // float alpha = 2 * PI * rand() / RAND_MAX;
        // float alpha = 2 * PI * i / max_sample_num;
        float alpha = 2 * PI * (i + (float)rand() / RAND_MAX) / max_sample_num;
        float dx = cos(alpha);
        float dy = sin(alpha);
        auto [r, g, b] = marching(x, y, dx, dy);
        sr += r, sg += g, sb += b;
    }
    return color((double)sr/max_sample_num, (double)sg/max_sample_num, (double)sb/max_sample_num);
}

int main(int argc, char* argv[]) {
    if(argc >= 3) H = atoi(argv[1]), W = atoi(argv[2]);
    image = new color[H*W];

    using std::make_unique;
    lights.push_back(make_unique<CircleLight>(pointf(0.4f, 0.5f), 0.20f, color(248,218,106)));
    lights.push_back(make_unique<CircleLight>(pointf(0.55f, 0.5f), 0.20f, color(127,127,127)));

#pragma omp parallel for
    for(int x = 0; x < W; x++) {
        for(int y = 0; y < H; y++) {
            image[x + y*W] = sample((float)(x)/W, (float)(y)/H);
        }
    }

    freopen("out.ppm", "w", stdout);
    printf("P3\n%d %d\n255\n", W, H);
    for(int y = H-1; y >= 0; y--) {
        for(int x = 0; x < W; x++) {
            printf("%d %d %d\n", (int)image[x + y*W].r, (int)image[x + y*W].g, (int)image[x + y*W].b);
        }
    }
    delete[] image;
    return 0;
}