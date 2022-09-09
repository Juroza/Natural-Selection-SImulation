//
//  Object.h
//  Natural Selection Simulation
//
//  Created by Jamaine Scarlett on 04/08/2021.
//

#ifndef Object_h
#define Object_h
#include "mesh.h"
#include "Shader.h"
#define GLM_ENABLE_EXPERIMENTAL
#include "glm/ext.hpp"

class Object{
    glm::mat4 model=glm::mat4(1.0f);
    glm::vec3 objColor,position=glm::vec3(1.0f,1.0f,1.0f),specularColor=glm::vec3(0.5f,0.5f,0.5f),rotationAxis=glm::vec3(0,1,0),scaler=glm::vec3(1);
    glm::vec4 color=glm::vec4(1,1,1,1);
    bool affectedByLight=false;
    float r=0.0f,g=0.0f,b=0.0f,opacity=1.0f,shininess=32.0f,rotationAngle=0,rotVariance=1;
    void alterModel(){
        glm::mat4 tempModel=glm::mat4(1.0f);
        tempModel=glm::translate(tempModel,position);
        tempModel=glm::rotate(tempModel,rotVariance*glm::radians(rotationAngle), rotationAxis);
        tempModel=glm::scale(tempModel,scaler);
        model=tempModel;
        
    }
public:
    Mesh* mesh;
    int ID;
    Shader* shader;
    std::string name;
    Object(Mesh* inputMesh,Shader* shad,std::string nam){
        name=nam;
        mesh= inputMesh;
        shader = shad;
       // mesh->readyForUse();
    }
    Mesh* getMesh(){
        return mesh;
    }
    void render(){
        shader->passMat4(model, "model");
        shader->passVec4(color, "ourColour");
        if(affectedByLight){
            shader->passVec3(objColor,"objectColor");
            shader->passVec3(objColor, "material.ambient");
            shader->passVec3(objColor, "material.diffuse");
            shader->passVec3(specularColor,"material.specular");
            shader->passFloat(shininess,"material.shininess");
        }
        glBindVertexArray(mesh->VAO);
        if(mesh->textureStrider!=0){
            mesh->activateTexture(shader->Program);
        }
            
        mesh->render();
        mesh->finishRender();
    }
    void setDiffuse(int num){
        shader->use();
        shader->passInt(num, "material.diffuse");
    }
    glm::vec3 getScale(){
        return scaler;
    }
    void changeColour(glm::vec3 inputcolor,float inputOpacity){
        color= glm::vec4(inputcolor,inputOpacity);
        objColor=inputcolor;
    }
    void changePosition(glm::vec3 pos){
        position=pos;
        alterModel();
    }
    void scaleModel(glm::vec3 scale){
        scaler=scale;
        alterModel();
    }
    void changeSpecularColour(glm::vec3 color){
        specularColor=color;
    }
    void setSpecularMap(int num){
        shader->use();
        shader->passInt(num, "material.specular");
    }
    void changeShininess(float shine){
        shininess=shine;
        
    }
    void IsaffectedByLight(bool x){
        affectedByLight=x;
    }
    bool getAffectedByLightState(){
        return affectedByLight;
    }
    void changeRotation(glm::vec3 inrotationAxis,float inrotationAngle,float variance){
        rotationAxis=inrotationAxis;
        rotationAngle=inrotationAngle;
        rotVariance=variance;
        alterModel();
    }
    glm::vec3 getPosition(){
        return position;
    }
    glm::vec3 getColour(){
        return color;
    }
    void stop(){
        shader->terminate();
    }
};


#endif /* Object_h */
