//
//  Camera.h
//  Natural Selection Simulation
//
//  Created by Jamaine Scarlett on 04/08/2021.
//

#ifndef Camera_h
#define Camera_h
#define GLM_ENABLE_EXPERIMENTAL
#include "glm/ext.hpp"
#include "Window.h"
class Camera{
    glm::mat4 projection=glm::mat4(1.0f);
    glm::mat4 view=glm::mat4(1.0f);
    glm::vec3 cameraUp    = glm::vec3(0.0f, 1.0f,  0.0f);
    bool firstMouse=true;
    double lastXPos=500,lastYPos=300;
    Shader* shader;
    float FOV,aspectRatio,nearPlane, farPlane,yaw=-90.0f,pitch=0.0f,sensitivity=0.1f;
public:
    float xpos, ypos,innerCutOffRad,outerCutOffRad;
    glm::vec3 cameraPos;
    glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
    Camera(float inputFOV,float inputaspectRatio,float inputnearPlane,float inputfarPlane,glm::vec3 inputcameraPos){
        projection = glm::perspective( inputFOV,inputaspectRatio, inputnearPlane, inputfarPlane);
        view=glm::translate(view,inputcameraPos);
        cameraPos=inputcameraPos;
        
    }
    void Update(Shader* shad){
        shader=&*shad;
        shader->passMat4(projection, "projection");
        shader->passMat4(view, "view");
        shader->passVec3(cameraPos, "viewPos");
        
    }
    void setInnerCutOff(float rad){
        innerCutOffRad=rad;
        
    }
    void setOuterCutOff(float rad){
        outerCutOffRad=rad;
        
    }
    void UpdateMousePos(float x, float y){
        xpos=x;
        ypos=y;
    }
    void changeView(glm::vec3 inputCameraPos){
        cameraPos=inputCameraPos;
        view = glm::lookAt(inputCameraPos, inputCameraPos + cameraFront, cameraUp);
    }
    
    void processInput(GLFWwindow * window,float deltatTime)
    {
        float xoffset=xpos;
        float yoffset=ypos;
        xoffset*=sensitivity;
        yoffset*=sensitivity;
        yaw+= xoffset;
        pitch+=yoffset;
        if (pitch > 89.0f)
            pitch = 89.0f;
        if (pitch < -89.0f)
            pitch = -89.0f;
        glm::vec3 front;
        front.x=cos(glm::radians(yaw))*cos(glm::radians(pitch));
        front.y=sin(glm::radians(pitch));
        front.z=sin(glm::radians(yaw))*cos(glm::radians(pitch));
        cameraFront=glm::normalize(front);
        const float cameraSpeed = 2.5f*deltatTime;
        if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
            cameraPos += cameraSpeed * cameraFront;
        if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
            cameraPos -= cameraSpeed * cameraFront;
        if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
            cameraPos -= glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
        if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
            cameraPos += glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
        view = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);
    }
};

#endif /* Camera_h */
