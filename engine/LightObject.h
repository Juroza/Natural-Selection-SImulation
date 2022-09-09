//
//  LightObject.h
//  Natural Selection Simulation
//
//  Created by Jamaine Scarlett on 07/08/2021.
//

#ifndef LightObject_h
#define LightObject_h
#include "Object.h"
class LightObject: public Object{
    glm::vec3 ambient=glm::vec3(0.9f,0.9f,0.9f),specular=glm::vec3(1.0f,1.0f,1.0f),diffuse=glm::vec3(0.5f,0.5f,0.5f),direction;
    float constant=1.0f,linear, quadratic,intensity;
public:
    using Object::Object;
    float timeOfDay;
    void changeAmbient(glm::vec3 amb){
        ambient=amb;
    }
    void changeDiffuse(glm::vec3 diff){
        diffuse=diff;
    }
    void changeSpecular(glm::vec3 spec){
        specular=spec;
    }
    void changeDirection(glm::vec3 dir){
        direction=dir;
    }
    void setLinear(float x){
        linear=x;
    }
    float getLightIntensity(){
        return intensity;
    }
    void setQuadratic(float x){
        quadratic=x;
    }
    void setLightIntensity(float x){
        intensity=x;
    }
    void setLightEmitColor(glm::vec3 colour){
        specular=colour;
        diffuse=colour;
        
    }
    void changeTimeOfDay(float input){
        timeOfDay=input;
    }
    float getConstant(){
        return constant;
    }
    float getLinear(){
        return linear;
    }
    float getQuadratic(){
        return quadratic;
    }
    glm::vec3 getDirection(){
        return direction;
    }

    glm::vec3 getAmbient(){
        return ambient;
    }
    glm::vec3 getDiffuse(){
        return diffuse;
    }
    glm::vec3 getSpecular(){
        return specular;
    }
};

#endif /* LightObject_h */
