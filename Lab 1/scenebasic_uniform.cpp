#include "scenebasic_uniform.h"

#include <cstdio>
#include <cstdlib>
#include "helper/texture.h"
#include <string>
using std::string;

#include <sstream>

#include <iostream>
using std::cerr;
using std::endl;

#include "helper/glutils.h"
using glm::vec3;
using glm::vec4;
using glm::mat3;
using glm::mat4;


SceneBasic_Uniform::SceneBasic_Uniform() :
    sky(100.0f),
    plane(100.0f, 100.0f, 1, 1), 
    tPrev(0){
    mesh = ObjMesh::load("media/car.obj", true);
}

void SceneBasic_Uniform::initScene()
{
    compile();
    skyboxProg.use();
    GLuint cubeTex = Texture::loadHdrCubeMap("media/texture/cube/place/place");
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_CUBE_MAP, cubeTex);

    glEnable(GL_DEPTH_TEST);
    model = mat4(1.0f);
    view = glm::lookAt(vec3(0.0f, 4.0f, 10.0f), vec3(0.0f, 0.2f, 0.0f), vec3(0.0f, 1.0f, 0.0f));
    projection = mat4(1.0f);
    angle = 0.0f;
    spin = true;
    camDistance = 10.0f;

    planeProg.use();
    GLuint tiles = Texture::loadTexture("media/texture/tiles_d.png");
    GLuint rust = Texture::loadTexture("media/texture/rust.png");
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, tiles);
    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D, rust);

    planeProg.setUniform("Spot.L", vec3(0.5f));
    planeProg.setUniform("Spot.La", vec3(0.5f));
    planeProg.setUniform("Spot.Exponent", 0.5f);
    planeProg.setUniform("Spot.Cutoff", glm::radians(30.0f));
    
    carProg.use();

    model = mat4(1.0f);
    view = glm::lookAt(vec3(0.0f, 4.0f, 10.0f), vec3(0.0f, 0.2f, 0.0f), vec3(0.0f, 1.0f, 0.0f));
    projection = mat4(1.0f);

    carProg.setUniform("Spot.L", vec3(0.5f));
    carProg.setUniform("Spot.La", vec3(0.5f));
    carProg.setUniform("Spot.Exponent", 0.5f);
    carProg.setUniform("Spot.Cutoff", glm::radians(30.0f));

}

void SceneBasic_Uniform::compile()
{
    try {
        planeProg.compileShader("shader/multiTextureSpotToon.vert");
        planeProg.compileShader("shader/multiTextureSpotToon.frag");
        planeProg.link();
        planeProg.use();

        carProg.compileShader("shader/toonSpotSingleColor.vert");
        carProg.compileShader("shader/toonSpotSingleColor.frag");
        carProg.link();
        carProg.use();

        skyboxProg.compileShader("shader/skyBox.vert");
        skyboxProg.compileShader("shader/skyBox.frag");
        skyboxProg.link();
        skyboxProg.use();
    }
    catch (GLSLProgramException& e) {
        std::cerr << "Shader compilation failed: " << e.what() << std::endl;
    }
}

void SceneBasic_Uniform::update(float t)
{
    if (spin) {
        float deltaT = t - tPrev;
        if (tPrev == 0.0f) deltaT = 0.0f;
        tPrev = t;
        angle += 0.2f * deltaT;
        if (angle > glm::two_pi<float>()) angle -= glm::two_pi<float>();
    }
}

void SceneBasic_Uniform::render()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    skyboxProg.use();
    model = mat4(1.0f);

    skyboxProg.use();
    model = glm::translate(model, vec3(0.0f, 10.0f, 0.0f));
    setMatricesSkybox();
    skyboxProg.use();
    sky.render();

    planeProg.use();
    view = glm::lookAt(vec3(0.0f, 4.0f, camDistance), vec3(0.0f, 0.2f, 0.0f), vec3(0.0f, 1.0f, 0.0f));

    vec4 lightPos = vec4(0.0f, 20.0f, 5.0f, 1.0f);
    planeProg.setUniform("Spot.Position", vec3(view * lightPos));
    mat3 normalMatrix = mat3(vec3(view[0]), vec3(view[1]), vec3(view[2]));
    planeProg.setUniform("Spot.Direction", normalMatrix * vec3(-lightPos));

    carProg.use();
    view = glm::lookAt(vec3(0.0f, 4.0f, camDistance), vec3(0.0f, 0.2f, 0.0f), vec3(0.0f, 1.0f, 0.0f));

    lightPos = vec4(0.0f, 20.0f, 5.0f, 1.0f);
    carProg.setUniform("Spot.Position", vec3(view * lightPos));
    normalMatrix = mat3(vec3(view[0]), vec3(view[1]), vec3(view[2]));
    carProg.setUniform("Spot.Direction", normalMatrix * vec3(-lightPos));
    
    planeProg.use();
    planeProg.setUniform("Material.Kd", vec3(0.0f, 0.0f, 0.0f));
    planeProg.setUniform("Material.Ks", vec3(0.1f, 0.1f, 0.1f));
    planeProg.setUniform("Material.Ka", vec3(0.2f, 0.2f, 0.2f));
    planeProg.setUniform("Material.Shininess", 100.0f);
    model = mat4(1.0f);
    planeProg.use();
    setMatricesPlane();
    plane.render();

    carProg.use();
    carProg.setUniform("Material.Kd", vec3(0.0f, 0.0f, 1.0f));
    carProg.setUniform("Material.Ks", vec3(0.95f, 0.95f, 0.95f));
    carProg.setUniform("Material.Ka", vec3(0.2f, 0.2f, 0.2f));
    carProg.setUniform("Material.Shininess", 100.0f);
    model = mat4(1.0f);

    carProg.use();
    model = glm::rotate(model, glm::radians(angle * 100.0f), vec3(0.0f, 1.0f, 0.0f));
    model = glm::translate(model, vec3(0.0f, 2.0f, 0.0f));
    setMatricesCar();
    mesh->render();

    
}

void SceneBasic_Uniform::resize(int w, int h)
{
    glViewport(0, 0, w, h);
    width = w;
    height = h;
    projection = glm::perspective(glm::radians(70.0f), (float)w / h, 0.3f, 100.0f);
}

void SceneBasic_Uniform::setMatricesPlane() {
    planeProg.use();
    mat4 mv = view * model;
    planeProg.setUniform("ModelViewMatrix", mv);
    planeProg.setUniform("NormalMatrix", glm::mat3(vec3(mv[0]), vec3(mv[1]), vec3(mv[2])));
    planeProg.setUniform("MVP", projection * mv);
}

void SceneBasic_Uniform::setMatricesCar() {
    carProg.use();
    mat4 mv = view * model;
    carProg.setUniform("ModelViewMatrix", mv);
    carProg.setUniform("NormalMatrix", glm::mat3(vec3(mv[0]), vec3(mv[1]), vec3(mv[2])));
    carProg.setUniform("MVP", projection * mv);
}

void SceneBasic_Uniform::setMatricesSkybox() {
    skyboxProg.use();
    mat4 mv = view * model;
    skyboxProg.setUniform("ModelViewMatrix", mv);
    skyboxProg.setUniform("NormalMatrix", glm::mat3(vec3(mv[0]), vec3(mv[1]), vec3(mv[2])));
    skyboxProg.setUniform("MVP", projection * mv);
}

void SceneBasic_Uniform::upPressed() {
    camDistance -= 0.1f;
}

void SceneBasic_Uniform::spinToggle() {
    if (spin) {
        spin = false;
    }
    else {
        spin = true;
    }
}

void SceneBasic_Uniform::downPressed() {
    camDistance += 0.1f;
}