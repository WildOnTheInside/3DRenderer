#pragma once

#include "Renderer.hpp"
#include "Scene.hpp"

class Application
{
private:
    Renderer renderer;
    Scene scene;
public:

    Application(uint32_t w, uint32_t h, const Scene& s): renderer(h, w), scene(s)
    {}

    void run(double m_i = 0.1, double r_i = 0.05) {
        double camMovInc = m_i, camRotInc = r_i;
        auto [ww, wh] = renderer.getSize();
        sf::RenderWindow window(sf::VideoMode(ww, wh), "3DRenderer");
        bool focus = true;
        while (window.isOpen()) {
            sf::Event event;
            while (window.pollEvent(event)) {
                switch (event.type) {
                    case sf::Event::Closed:
                        window.close();
                        break;
                    case sf::Event::Resized:
                        ww = event.size.width;
                        wh = event.size.height;
                        renderer.setViewportSize(ww, wh);
                        scene.setCameraAspectRatio(ww, wh);
                        window.setView(sf::View(sf::FloatRect(0, 0, ww, wh)));
                        break;
                    case sf::Event::GainedFocus:
                        focus = true;
                        break;
                    case sf::Event::LostFocus:
                        focus = false;
                        break;
                    default:
                        break;
                }
            }
            if (!focus) {
                continue;
            }
            int r = 0, u = 0, f = 0;
            int rotU = 0, rotR = 0;
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left)) {
                r -= 1;
            }
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right)) {
                r += 1;
            }
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up)) {
                f += 1;
            }
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down)) {
                f -= 1;
            }
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Space)) {
                u += 1;
            }
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::LAlt)) {
                u -= 1;
            }
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::W)) {
                rotU += 1;
            }
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::S)) {
                rotU -= 1;
            }
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::A)) {
                rotR -= 1;
            }
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::D)) {
                rotR += 1;
            }

            scene.moveCamera(camMovInc * u, camMovInc * r, camMovInc * f);
            scene.rotateCamera(camRotInc * rotU, camRotInc * rotR);

            window.clear(sf::Color::White);
            sf::Texture t;
            t.create(ww, wh);
            t.update(renderer.render(scene));
            sf::Sprite s(t);
            window.draw(s);
            window.display();
        }
    }
};
