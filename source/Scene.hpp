#pragma once

#include "glm/glm.hpp"
#include "glm/ext/matrix_transform.hpp"
#include "glm/gtx/rotate_vector.hpp"

#include "auxiliary.hpp"
#include "Camera.hpp"
#include "Interpolator.hpp"
#include "LightSource.hpp"
#include "Object3D.hpp"
#include "Texture.hpp"
#include "TexturedObject3D.hpp"

#include <vector>
#include <cmath>
#include <stdexcept>

class Scene
{
private:
    std::vector<TexturedObject3D> objects;
    std::vector<glm::dvec3> rotations, scales, translations;
    Camera cam;
    glm::dvec3 camPos, camDir, camUp;
    std::vector<LightSource*> lights;
    std::vector<glm::dvec3> lights_pos;
public:
    uint32_t n_objects = 0;

    Scene(
        std::vector<TexturedObject3D>& obj, std::vector<glm::dvec3> rot, std::vector<glm::dvec3> sc, std::vector<glm::dvec3> trans,
        glm::dvec3 pos, glm::dvec3 dir, glm::dvec3 up, double theta, double near, double far, double ar,
        std::vector<LightSource*> l, std::vector<glm::dvec3> l_pos
    ):
    Scene(obj, rot, sc, trans, pos, dir, up, Camera(theta, near, far, ar), l, l_pos)
    {}

    Scene(
        std::vector<TexturedObject3D> obj, std::vector<glm::dvec3> rot, std::vector<glm::dvec3> sc, std::vector<glm::dvec3> trans,
        glm::dvec3 pos, glm::dvec3 dir, glm::dvec3 up, Camera camera, std::vector<LightSource*> l, std::vector<glm::dvec3> l_pos
    ):
    objects(obj), n_objects(obj.size()), rotations(rot), scales(sc), translations(trans), camPos(pos), camDir(dir), camUp(up), cam(camera), lights(l), lights_pos(l_pos)
    {
        if (n_objects != rot.size() || n_objects != sc.size() || n_objects != trans.size()) {
            throw std::invalid_argument("Number of transforms is not equal to the number of objects");
        }
    }

    Scene(glm::dvec3 pos, glm::dvec3 dir, glm::dvec3 up, Camera camera, std::vector<LightSource*> l, std::vector<glm::dvec3> l_pos):
    camPos(pos), camDir(dir), camUp(glm::normalize(up)), cam(camera), lights(l), lights_pos(l_pos)
    {}

    void addObject(TexturedObject3D object, glm::dvec3 rot = glm::dvec3(0.), glm::dvec3 sc = glm::dvec3(1.), glm::dvec3 trans = glm::dvec3(0.)) {
        objects.push_back(object);
        rotations.push_back(rot);
        scales.push_back(sc);
        translations.push_back(trans);
        n_objects++;
    }

    void addObjects(std::vector<TexturedObject3D> objs, std::vector<glm::dvec3> rots, std::vector<glm::dvec3> scs, std::vector<glm::dvec3> transs) {
        objects.insert(objects.end(), objs.begin(), objs.end());
        rotations.insert(rotations.end(), rots.begin(), rots.end());
        scales.insert(scales.end(), scs.begin(), scs.end());
        translations.insert(translations.end(), transs.begin(), transs.end());
        n_objects += objs.size();
    }

    void addObjects(std::vector<TexturedObject3D> objs) {
        objects.insert(objects.end(), objs.begin(), objs.end());
        rotations.insert(rotations.end(), objs.size(), {0., 0., 0.});
        scales.insert(scales.end(), objs.size(), {1., 1., 1.});
        translations.insert(translations.end(), objs.size(), {0., 0., 0.});
        n_objects += objs.size();
    }

    glm::dmat4 cameraMatrix() const {
        return glm::lookAtLH(camPos, camPos + camDir, camUp);
    }

    glm::dmat3 cameraMatrixNormal() const {
        return glm::dmat3(glm::lookAtLH(camPos, camPos + camDir, camUp));
    }

    glm::dmat4 modelMatrix(uint32_t i) const {
        if (i >= n_objects) {
            throw std::out_of_range("Object index out of range");
        }
        return glm::scale(
            glm::rotate(
                glm::rotate(
                    glm::rotate(
                        glm::translate(glm::dmat4(1.), translations[i]),
                        rotations[i].x,
                        glm::dvec3(1., 0., 0.)
                    ),
                    rotations[i].y,
                    glm::dvec3(0., 1., 0.)
                ),
                rotations[i].z,
                glm::dvec3(0., 0., 1.)
            ),
            scales[i]
        );
    }

    glm::dmat3 modelMatrixNormals(uint32_t i) const {
        if (i >= n_objects) {
            throw std::out_of_range("Object index out of range");
        }
        return 
        glm::dmat3(
            glm::rotate(
                glm::rotate(
                    glm::rotate(
                        glm::dmat4(1.),
                        rotations[i].x,
                        glm::dvec3(1., 0., 0.)
                    ),
                    rotations[i].y,
                    glm::dvec3(0., 1., 0.)
                ),
                rotations[i].z,
                glm::dvec3(0., 0., 1.))
        );
    }

    glm::dmat4 projectionMatrix() const {
        return cam.projectionMatrix();
    }

    const TexturedObject3D& getObject(uint32_t i) const {
        if (i >= n_objects) {
            throw std::out_of_range("Object index out of range");
        }
        return objects[i];
    }

    void moveCamera(double up, double right, double forward) {
        camPos += camUp * up + camDir * forward + glm::cross(camDir, camUp) * right;
    }

    void rotateCamera(double up, double right) {
        auto camRight = glm::cross(camDir, camUp);
        camDir = glm::rotate(camDir, up, camRight);
        camUp = glm::rotate(camUp, up, camRight);
        camDir = glm::rotate(camDir, -right, camUp);
        camUp = glm::rotate(camUp, -right, camUp);
    }

    void setCameraAspectRatio(double w, double h) {
        cam.setAspectRatio(w / h);
    }

    std::vector<glm::dvec3> getFrustrumNormals() const {
        return cam.getFrustrumNormals();
    }

    std::vector<double> getFrustrumOffsets() const {
        return cam.getFrustrumOffsets();
    }

    Interpolator getInterpolator() const {
        return Interpolator(cam.A(), cam.B());
    }

    const std::vector<glm::dvec3>& getLightsPosition() const {
        return lights_pos;
    }

    const std::vector<LightSource*>& getLights() const {
        return lights;
    }
};
