//
//  main.cpp
//  OpenCVTest
//
//  Created by 김경률 on 2020/09/17.
//  Copyright © 2020 Ryul. All rights reserved.
//
#define GLEW_STATIC
#define STB_IMAGE_IMPLEMENTATION
#include <OpenGL/gl3.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>

#include <vector>
#include <math.h>
#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "stb_image.h"
#include "toys.h"
#include "j3a.hpp"

using namespace glm;
using namespace std;

void render(GLFWwindow* window);
void init();
void mouseButtonCallback(GLFWwindow*, int, int, int);
void cursorMotionCallback(GLFWwindow*, double, double);

GLuint triangleVBO = 0;
GLuint normalVBO = 0;
GLuint texCoordVBO = 0;
GLuint vertexArrayID = 0;
GLuint indexVBOID = 0;

GLuint diffTexID = 0;


Program program;

float cameraDistance = 4;
float cameraYaw = 0.f;
float cameraPitch = 0.f;
float cameraFov = 60.f;

vec3 sceneCenter = vec3(0, 0, 0);
vec3 lightPos = vec3(3, 3, 3);
vec3 lightColor = vec3(1);
vec3 ambientLight = vec3(0.1);

vec4 diffuseMaterial = vec4(1, 0.4, 0, 1);
vec4 specularMaterial = vec4(1);

GLint lightEffect = 10;
int lastX = 0, lastY = 0;

int main(int argc, const char * argv[]) {
    // insert code here...
    glfwInit();
    
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, true);
    glfwWindowHint(GLFW_SAMPLES, 8);

    GLFWwindow* window = glfwCreateWindow(800, 600, "Test", 0, 0);
    glfwMakeContextCurrent(window);
    glfwSetMouseButtonCallback(window, mouseButtonCallback);
    glfwSetCursorPosCallback(window, cursorMotionCallback);

    init();
    while(!glfwWindowShouldClose(window)){
        render(window);
        glfwPollEvents();
    }
    glfwTerminate();
    return 0;
}

#define PATH "/Users/supro/Desktop/Archive/"

void init(){
    loadJ3A(PATH"apple.j3a");
    int texWidth, texHeight, texChannels;
    program.loadShaders("shader.vert", "shader.frag");
    
    void* buffer = stbi_load("appleD.jpg", &texWidth, &texHeight, &texChannels, 4);

    glGenTextures(1, &diffTexID);
    glBindTexture(GL_TEXTURE_2D, diffTexID);
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexImage2D( GL_TEXTURE_2D, 0, GL_SRGB8_ALPHA8, texWidth, texHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, buffer);
    //GL_SRGB8_ALPHA8: 이미지에 들어가 있는 값이 SRGB값이라고 표시.

    stbi_image_free( buffer );
    
    glGenVertexArrays(1, &vertexArrayID);
    glBindVertexArray(vertexArrayID);
    
    glGenBuffers(1, &triangleVBO);
    glBindBuffer(GL_ARRAY_BUFFER, triangleVBO);
    glBufferData(GL_ARRAY_BUFFER, nVertices[0]*sizeof(vec3), vertices[0], GL_STATIC_DRAW);
    
    glBindBuffer(GL_ARRAY_BUFFER, triangleVBO);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
    
    glGenBuffers(1, &normalVBO);
    glBindBuffer(GL_ARRAY_BUFFER, normalVBO);
    glBufferData(GL_ARRAY_BUFFER, nVertices[0]*sizeof(vec3), normals[0], GL_STATIC_DRAW);
    
    glBindBuffer(GL_ARRAY_BUFFER, normalVBO);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);
    
    glGenBuffers(1, &texCoordVBO);
    glBindBuffer(GL_ARRAY_BUFFER, texCoordVBO);
    glBufferData(GL_ARRAY_BUFFER, nVertices[0]*sizeof(vec2), texCoords[0], GL_STATIC_DRAW); //2차원이므로 vec2를 사용
    
    glBindBuffer(GL_ARRAY_BUFFER, texCoordVBO);
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 0, 0);
    
    
    glGenBuffers(1, &indexVBOID);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexVBOID);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, nTriangles[0]*sizeof(vec3), triangles[0], GL_STATIC_DRAW);
    
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_FRAMEBUFFER_SRGB);
}

float rotAngle = 0;

void render(GLFWwindow* window){
    int w, h;
    glfwGetFramebufferSize(window, &w, &h);
    glViewport(0, 0, w, h);
    glClearColor(0, 0, .5, 1);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    glUseProgram(program.programID);
    
    glBindVertexArray(vertexArrayID);
    
    
    rotAngle += 0.3/180.f*3.141592;
    GLuint loc;
    
    loc = glGetUniformLocation(program.programID, "modelMat");
    glUniformMatrix4fv(loc, 1, 0, value_ptr(rotate(90/180.f * 3.141592f, vec3(1, 0, 0))));
    
    loc = glGetUniformLocation( program.programID, "viewMat");
    vec3 cameraPosition = vec3(0, 0, cameraDistance);
    cameraPosition = vec3(rotate(cameraPitch, vec3(-1, 0, 0)) * vec4(cameraPosition, 1));
    cameraPosition = vec3(rotate(cameraYaw, vec3(0, 1, 0)) * vec4(cameraPosition, 1));
    
//    vec3 cameraPosition = vec3(rotate(cameraYaw, vec3(0, 1, 0)) * rotate(cameraPitch, vec3(1,0,0)) * vec4(0,0,cameraDistance,0));
    mat4 viewMat = lookAt(cameraPosition, vec3(0, 0, 0), vec3(0, 1, 0));
    glUniformMatrix4fv(loc, 1, 0, value_ptr(viewMat));
    
    loc = glGetUniformLocation( program.programID, "projMat");
    mat4 projMat = perspective(cameraFov*3.141592f/180, w/(float)h, 0.01f, 100.f);
    glUniformMatrix4fv(loc, 1, 0, value_ptr(projMat));
    
    loc = glGetUniformLocation( program.programID, "cameraPos");
    glUniform3fv( loc, 1, value_ptr(cameraPosition));
    
    loc = glGetUniformLocation( program.programID, "lightPos");
    glUniform3fv( loc, 1, value_ptr(lightPos));
    
    loc = glGetUniformLocation( program.programID, "lightColor");
    glUniform3fv( loc, 1, value_ptr(lightColor));
    
    loc = glGetUniformLocation( program.programID, "ambientLight");
    glUniform3fv( loc, 1, value_ptr(ambientLight));
    
    loc = glGetUniformLocation( program.programID, "diffuseMaterial");
    glUniform4fv( loc, 1, value_ptr(diffuseMaterial));
    
    loc = glGetUniformLocation( program.programID, "specularMaterial");
    glUniform4fv( loc, 1, value_ptr(specularMaterial));
    
    loc = glGetUniformLocation( program.programID, "lightEffect");
    glUniform1i(loc, lightEffect);
    
    glActiveTexture(GL_TEXTURE0 + 2);
    glBindTexture(GL_TEXTURE_2D, diffTexID);
    loc = glGetUniformLocation( program.programID, "diffTex");
    glUniform1i(loc, 2);

    glBindVertexArray(vertexArrayID);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexVBOID);
    glDrawElements(GL_TRIANGLES, nTriangles[0]*3, GL_UNSIGNED_INT, 0);
    
    glfwSwapBuffers(window);
}

void mouseButtonCallback( GLFWwindow* window, int button, int action, int mods){
    if(button == GLFW_MOUSE_BUTTON_1 && action == GLFW_PRESS){
        double xpos, ypos;
        glfwGetCursorPos(window, &xpos, &ypos);
        lastX = int(xpos);
        lastY = int(ypos);
    }
}
void cursorMotionCallback(GLFWwindow* window, double xpos, double ypos){
    if(glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_1) == GLFW_PRESS){
        if(glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS){
            cameraFov *= (pow(1.001, ypos - lastY));
            cameraFov *= (pow(1.001, xpos - lastX));
        }
        else{
            cameraPitch += (ypos - lastY)/300;
            cameraYaw -= (xpos - lastX)/300;
            lastX = int(xpos);
            lastY = int(ypos);
        }
    }
}

