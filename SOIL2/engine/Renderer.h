//
//  Renderer.h
//  Natural Selection Simulation
//
//  Created by Jamaine Scarlett on 04/08/2021.
//

#ifndef Renderer_h
#define Renderer_h
#include "Object.h"
#include "Camera.h"
#include "LightObject.h"
#include <vector>

class Renderer{
    
    std::vector<LightObject*> lightObjects;
    Camera* camera;
    
public:
    std::vector<Object*> objects;
    Renderer(Camera* cam){
        camera=cam;
    }
    void addObjectToRender(Object* inputObject){
        objects.push_back(inputObject);
    }
    void addLightObjectToRender(LightObject* inputLightObject){
        lightObjects.push_back(inputLightObject);
    }
    void removeObjectToRender(Object* target){
        for(int i=0;i<objects.size();i++){
            if(objects[i]==target){
               // objects[i]=nullptr;
                objects.erase(objects.begin()+i);
                break;
            }
        }
        
    }
    void checkFood(){
        int max=(int)objects.size();
        for(int i=0;i<max;i++){
            if(objects[i]->name=="Food"){
                std::cout<<"STILL FOOD";
                objects.erase(objects.begin()+i);
            }
        }
    }
    void renderScene(){
        for(int i=0;i<objects.size();i++){
            objects[i]->shader->use();
            if(objects[i]->getAffectedByLightState()){
                objects[i]->shader->passVec3(lightObjects[0]->getPosition(), "light.position");
                objects[i]->shader->passVec3(glm::vec3(0,-1,0), "light.direction");
                objects[i]->shader->passFloat(glm::cos(glm::radians(camera->innerCutOffRad)), "light.innerCutOff");
                objects[i]->shader->passFloat(glm::cos(glm::radians(camera->outerCutOffRad)), "light.outerCutOff");
                objects[i]->shader->passFloat((8), "light.intensityVal");
                objects[i]->shader->passFloat(lightObjects[0]->getConstant(), "light.constant");
                objects[i]->shader->passFloat(lightObjects[0]->getLinear(), "light.linear");
                objects[i]->shader->passFloat(lightObjects[0]->getQuadratic(), "light.quadratic");
                objects[i]->shader->passVec3(lightObjects[0]->getAmbient(), "light.ambient");
                 objects[i]->shader->passVec3(lightObjects[0]->getDiffuse(), "light.diffuse");
                 objects[i]->shader->passVec3(lightObjects[0]->getSpecular(), "light.specular");
                objects[i]->shader->passFloat(lightObjects[0]->timeOfDay, "ambientLight");
            }
            camera->Update(objects[i]->shader);
            objects[i]->render();
        };
       for(int i=0;i<lightObjects.size();i++){
            lightObjects[i]->shader->use();
          camera->Update(lightObjects[i]->shader);
            lightObjects[i]->render();
        
    };
    }
};

#endif /* Renderer_h */
