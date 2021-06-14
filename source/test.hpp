#pragma once

#include "Application.hpp"
#include "TexturedObject3D.hpp"
#include "Scene.hpp"

#include <utility>
#include <vector>

double pi = std::acos(-1);

void test_scene_1() {
    auto objs = TexturedObject3D::fromFile("../obj/couch/model.obj");
    AmbientLight al({0.7, 0.7, 0.7});
    DirectionalLight dl(0x606060ff, {0.5, 1., -2.});
    Camera cam(pi / 2.);
    glm::dvec3 zero(0.);
    Scene scene({-3, 0, 0}, {1, 0, 0}, {0, 1, 0}, cam, {&al, &dl}, {zero, zero});
    scene.addObjects(objs);
    uint32_t wh = 900, ww = 1600;
    Renderer renderer(wh, ww);
    Application app(ww, wh, scene);
    app.run();
}

void test_scene_2() {
    auto objs = TexturedObject3D::fromFile("../obj/drawer/model.obj");
    AmbientLight al({0.7, 0.7, 0.7});
    DirectionalLight dl(0x606060ff, {0.5, 1., -2.});
    PointLight pl(0xff1122ff, 0.1, 0.1, 0.1);
    Camera cam(pi / 2.);
    glm::dvec3 zero(0.);
    Scene scene({5, 2, 0}, {-1, -0.5, 0}, {0, 1, 0}, cam, {&al, &dl, &pl}, {zero, zero, {-1, 2, 2}});
    scene.addObjects(objs);
    uint32_t wh = 900, ww = 1600;
    Renderer renderer(wh, ww);
    Application app(ww, wh, scene);
    app.run();
}

void test_scene_3() {
    auto objs = TexturedObject3D::fromFile("../obj/flowers/model.obj");
    AmbientLight al({0.9, 0.9, 0.6});
    DirectionalLight dl(0x606060ff, {0.5, 1., -2.});
    Camera cam(pi / 2.);
    glm::dvec3 zero(0.);
    Scene scene({5, 2, 0}, {-1, -0.5, 0}, {0, 1, 0}, cam, {&al, &dl}, {zero, zero});
    scene.addObjects(objs);
    uint32_t wh = 900, ww = 1600;
    Renderer renderer(wh, ww);
    Application app(ww, wh, scene);
    app.run();
}

void test_scene_4() {
    auto objs = TexturedObject3D::fromFile("../obj/knife/OBJ/Kitchenknife_lowpoly.obj");
    AmbientLight al({0.9, 0.9, 0.9});
    PointLight pl(0x10fea1, 0.05, 0.1, 0.1);
    Camera cam(pi / 2.);
    glm::dvec3 zero(0.);
    Scene scene({0, -5, 0}, {0, 1, 0}, {1, 0, 0}, cam, {&al, &pl}, {zero, {3, 0, 1}});
    scene.addObjects(objs);
    uint32_t wh = 900, ww = 1600;
    Renderer renderer(wh, ww);
    Application app(ww, wh, scene);
    app.run();
}

void test_scene_5() {
    auto objs = TexturedObject3D::fromFile("../obj/moon/Moon_2K.obj");
    AmbientLight al({0.4, 0.4, 0.4});
    PointLight pl(0x606060ff, 0.05, 0.01, 0.01);
    Camera cam(pi / 2.);
    glm::dvec3 zero(0.);
    Scene scene({0, -5, 0}, {0, 1, 0}, {1, 0, 0}, cam, {&al, &pl}, {zero, {5, 0, 1}});
    scene.addObjects(objs);
    uint32_t wh = 900, ww = 1600;
    Renderer renderer(wh, ww);
    Application app(ww, wh, scene);
    app.run();
}

void test_scene_6() {
    auto objs = TexturedObject3D::fromFile("../obj/rocks/RockSet.obj");
    AmbientLight al({0.4, 0.4, 0.4});
    DirectionalLight dl(0xfff340ff, {-1, 5, -1});
    Camera cam(pi / 2.);
    glm::dvec3 zero(0.);
    Scene scene({0, -8, 0}, {0, 1, 0}, {1, 0, 0}, cam, {&al, &dl}, {zero, zero});
    scene.addObjects(objs);
    uint32_t wh = 900, ww = 1600;
    Renderer renderer(wh, ww);
    Application app(ww, wh, scene);
    app.run();
}

void test_scene_7() {
    auto objs = TexturedObject3D::fromFile("../obj/sword/Sting-Sword-lowpoly.obj");
    AmbientLight al({0.4, 0.4, 0.4});
    DirectionalLight dl(0xfff340ff, {-1, 5, -1});
    Camera cam(pi / 2.);
    glm::dvec3 zero(0.);
    Scene scene({0, -70, 0}, {0, 1, 0}, {0, 0, -1}, cam, {&al, &dl}, {zero, zero});
    scene.addObjects(objs);
    uint32_t wh = 900, ww = 1600;
    Renderer renderer(wh, ww);
    Application app(ww, wh, scene);
    app.run(0.5, 0.02);
}