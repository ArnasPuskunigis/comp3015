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
    //teapot(14, glm::mat4(1.0f)) {
    //torus(1.75f * 0.75f, 1.75f * 0.75f, 50, 50) {
    mesh = ObjMesh::load("media/car.obj", true);
}

void SceneBasic_Uniform::initScene()
{
    compile();

    prog3.use();
    GLuint cubeTex = Texture::loadHdrCubeMap("media/texture/cube/place/place");
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_CUBE_MAP, cubeTex);

    prog3.use();
    prog3.setUniform("SkyBoxTex", 0);  // Skybox texture is bound to texture unit 0

    glEnable(GL_DEPTH_TEST);
    model = mat4(1.0f);
    view = glm::lookAt(vec3(0.0f, 4.0f, 10.0f), vec3(0.0f, 0.2f, 0.0f), vec3(0.0f, 1.0f, 0.0f));
    //model = glm::rotate(model, glm::radians(-35.0f), vec3(1.0f, 0.0f, 0.0f));
    //model = glm::rotate(model, glm::radians(15.0f), vec3(0.0f, 1.0f, 0.0f));
    projection = mat4(1.0f);
    angle = 0.0f;
    spin = true;
    camDistance = 10.0f;

    prog.use();

    GLuint tiles = Texture::loadTexture("media/texture/tiles_d.png");
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT); // Repeat horizontally
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT); // Repeat vertically
    GLuint moss = Texture::loadTexture("media/texture/rust.png");
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, tiles);
    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D, moss);

    /*float x, z;
    for (int i = 0; i < 3; i++) {
        std::stringstream name;
        name << "lights[" << i << "].Position";
        x = 2.0f * cosf((glm::two_pi <float>() / 3) * i);
        z = 2.0f * sinf((glm::two_pi<float>() / 3) * i);
        prog.setUniform(name.str().c_str(), view * glm::vec4(x, 1.2f, z + 1.0f, 1.0f));
    }*/

    //prog.setUniform("Light.Position", view * glm::vec4(5.0f, 5.0f, 2.0f, 1.0f));
    //prog.setUniform("Light.L", vec3(0.9f));

    prog.setUniform("Spot.L", vec3(0.5f));
    prog.setUniform("Spot.La", vec3(0.5f));
    prog.setUniform("Spot.Exponent", 0.5f);
    prog.setUniform("Spot.Cutoff", glm::radians(30.0f));
    
    prog2.use();

    glEnable(GL_DEPTH_TEST);
    model = mat4(1.0f);
    view = glm::lookAt(vec3(0.0f, 4.0f, 10.0f), vec3(0.0f, 0.2f, 0.0f), vec3(0.0f, 1.0f, 0.0f));
    projection = mat4(1.0f);
    angle = 0.0f;
    spin = true;
    camDistance = 10.0f;

    prog2.setUniform("Spot.L", vec3(0.5f));
    prog2.setUniform("Spot.La", vec3(0.5f));
    prog2.setUniform("Spot.Exponent", 0.5f);
    prog2.setUniform("Spot.Cutoff", glm::radians(30.0f));

    //prog.setUniform("Fog.MaxDist", 20.0f);
    //prog.setUniform("Fog.MinDist", 1.0f);
    //prog.setUniform("Fog.Color", vec3(0.5f, 0.5f, 0.5f));

    /*prog.setUniform("lights[1].L", vec3(0.0f, 0.8f, 0.0f));
    prog.setUniform("lights[2].L", vec3(0.8f, 0.0f, 0.0f));

    prog.setUniform("lights[0].La", vec3(0.0f, 0.0f, 0.2f));
    prog.setUniform("lights[1].La", vec3(0.0f, 0.2f, 0.0f));
    prog.setUniform("lights[2].La", vec3(0.2f, 0.0f, 0.0f));*/


}

void SceneBasic_Uniform::compile()
{
    try {
        prog.compileShader("shader/basic_uniform.vert");
        prog.compileShader("shader/basic_uniform.frag");
        prog.link();
        prog.use();

        prog2.compileShader("shader/basic_uniform2.vert");
        prog2.compileShader("shader/basic_uniform2.frag");
        prog2.link();
        prog2.use();

        prog3.compileShader("shader/basic_uniform3.vert");
        prog3.compileShader("shader/basic_uniform3.frag");
        prog3.link();
        prog3.use();
    }
    catch (GLSLProgramException& e) {
        std::cerr << "Shader compilation failed: " << e.what() << std::endl;
    }
}

void SceneBasic_Uniform::update(float t)
{
    //update your angle here

    if (spin) {
        float deltaT = t - tPrev;
        if (tPrev == 0.0f) deltaT = 0.0f;
        tPrev = t;
        angle += 0.1f * deltaT;
        if (angle > glm::two_pi<float>()) angle -= glm::two_pi<float>();
    }
}

void SceneBasic_Uniform::render()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    prog3.use();
    model = mat4(1.0f);

    prog3.use();
    model = glm::translate(model, vec3(0.0f, 10.0f, 0.0f));
    setMatrices3();
    prog3.use();
    sky.render();


    /*vec4 lightPos = vec4(10.0f * cos(90.0f), 10.0f, 10.0f * sin(90.0f), 1.0f);
    prog.setUniform("Light.Position", vec4(view * lightPos));*/

    prog.use();
    view = glm::lookAt(vec3(0.0f, 4.0f, camDistance), vec3(0.0f, 0.2f, 0.0f), vec3(0.0f, 1.0f, 0.0f));

    vec4 lightPos = vec4(0.0f, 20.0f, 5.0f, 1.0f);
    prog.setUniform("Spot.Position", vec3(view * lightPos));
    mat3 normalMatrix = mat3(vec3(view[0]), vec3(view[1]), vec3(view[2]));
    prog.setUniform("Spot.Direction", normalMatrix * vec3(-lightPos));

    prog2.use();
    view = glm::lookAt(vec3(0.0f, 4.0f, camDistance), vec3(0.0f, 0.2f, 0.0f), vec3(0.0f, 1.0f, 0.0f));

    lightPos = vec4(0.0f, 20.0f, 5.0f, 1.0f);
    prog2.setUniform("Spot.Position", vec3(view * lightPos));
    normalMatrix = mat3(vec3(view[0]), vec3(view[1]), vec3(view[2]));
    prog2.setUniform("Spot.Direction", normalMatrix * vec3(-lightPos));
    //prog.setUniform("Material.Kd", vec3(0.2f, 0.55f, 0.9f));
    //prog.setUniform("Material.Ks", vec3(0.95f, 0.95f, 0.95f));
    //prog.setUniform("Material.Ka", vec3(0.2f * 0.3f, 0.55f * 0.3f, 0.9f * 0.3f));
    //prog.setUniform("Material.Shininess", 100.0f);

    //float dist = 0.0f;

    //model = mat4(1.0f);
    //model = glm::translate(model, vec3(dist * 0.6f - 1.0f, 0.0f, -dist));
    //model = glm::rotate(model, glm::radians(-90.0f), vec3(1.0f, 0.0f, 0.0f));
    //setMatrices();
    //teapot.render();

    prog.use();
    prog.setUniform("Material.Kd", vec3(0.0f, 0.0f, 0.0f));
    prog.setUniform("Material.Ks", vec3(0.1f, 0.1f, 0.1f));
    prog.setUniform("Material.Ka", vec3(0.2f, 0.2f, 0.2f));
    prog.setUniform("Material.Shininess", 100.0f);
    model = mat4(1.0f);
    prog.use();
    setMatrices1();
    plane.render();

    prog2.use();
    prog2.setUniform("Material.Kd", vec3(1.0f, 0.0f, 0.0f));
    prog2.setUniform("Material.Ks", vec3(0.95f, 0.95f, 0.95f));
    prog2.setUniform("Material.Ka", vec3(0.2f, 0.2f, 0.2f));
    prog2.setUniform("Material.Shininess", 100.0f);
    model = mat4(1.0f);

    prog2.use();
    model = glm::rotate(model, glm::radians(angle * 100.0f), vec3(0.0f, 1.0f, 0.0f));
    model = glm::translate(model, vec3(0.0f, 2.0f, 0.0f));
    setMatrices2();
    mesh->render();

    
}

void SceneBasic_Uniform::resize(int w, int h)
{
    glViewport(0, 0, w, h);
    width = w;
    height = h;
    projection = glm::perspective(glm::radians(70.0f), (float)w / h, 0.3f, 100.0f);
}

void SceneBasic_Uniform::setMatrices1() {
    prog.use();
    mat4 mv = view * model;
    prog.setUniform("ModelViewMatrix", mv);
    prog.setUniform("NormalMatrix", glm::mat3(vec3(mv[0]), vec3(mv[1]), vec3(mv[2])));
    prog.setUniform("MVP", projection * mv);
}

void SceneBasic_Uniform::setMatrices2() {
    prog2.use();
    mat4 mv = view * model;
    prog2.setUniform("ModelViewMatrix", mv);
    prog2.setUniform("NormalMatrix", glm::mat3(vec3(mv[0]), vec3(mv[1]), vec3(mv[2])));
    prog2.setUniform("MVP", projection * mv);
}

void SceneBasic_Uniform::setMatrices3() {
    prog3.use();
    mat4 mv = view * model;
    prog3.setUniform("ModelViewMatrix", mv);
    prog3.setUniform("NormalMatrix", glm::mat3(vec3(mv[0]), vec3(mv[1]), vec3(mv[2])));
    prog3.setUniform("MVP", projection * mv);
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