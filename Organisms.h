//
//  Organisms.h
//  Natural Selection Simulation
//
//  Created by Jamaine Scarlett on 02/10/2021.
//

#ifndef Organisms_h
#define Organisms_h
#include "engine/Shader.h"
#include "engine/mesh.h"
#include "engine/Object.h"
#include "Organisms.h"
#include <map>
class Predator;
class Food{
public:
    Object* body;
    Mesh* mesh;
    Shader* shader;
    bool eaten=false,isTargeted=false;
    Food(Mesh* m,Shader* shad){
        body=new Object(m,shad,"Food");
        shader=shad;
        mesh=m;
     //   m->addTexture("res/images/predatorTexture.png", 6);
    }
    
};
class Prey{
public:
    Object* body;
    Mesh* mesh;
    Shader* shader;
    std::map<std::string,int> genes{{"size",1},{"speed",1.5},{"maxVisionDistance",90},{"foodPriority",4}};
    int energy=0;
    double baseHealth=genes["size"]*50;
    double healthChange=genes["size"]*50;
    double health=genes["size"]*50;
    bool isAlive=true,foundFood=false,givenUp=false,isTargeted=false,reproduceSignal=false,reproduceMode=false,hasPartner=false;
    Food* targetFood;
    Prey* partner;
    std::vector<Predator*> threat;
    
    Prey(Mesh* m,Shader* shad){
        body=new Object(m,shad,"Prey");
        shader=shad;
        mesh=m;
    }
    void mutate(){
        body->scaleModel(glm::vec3(genes["size"]));
        baseHealth=genes["size"]*50;
        health=genes["size"]*50;
        healthChange=genes["size"]*50;
    }
    void eat(){
        energy+=1;
        health+=7;
        if(!(reproduceMode)&&energy>=5){
            energy-=10;
            std::cout<<"ABLE TO REPRODUCE";
            
            reproduceSignal=true;
        }
    }
    void takeDamage(){
        health-=5;
        healthChange-=5;
        std::cout<<"\n currenthealth:"<<health<<"\nbase\n"<<baseHealth;
        body->changeColour(glm::vec3(1,(float)(health/baseHealth),(float)(health/baseHealth)), 1);
        body->scaleModel(glm::vec3(1,(float)(health/50),(float)(health/50)));
    }
    void take(){
        body->changeColour(glm::vec3(1,(float)(health/baseHealth),(float)(health/baseHealth)), 1);
        body->scaleModel(glm::vec3(1,(float)(health/baseHealth),(float)(health/baseHealth)));
    }
    int getHealth(){
        return health;
    }
    
    
    
};
class sampledPrey{
public:
    std::map<std::string,int> genes{{"size",1},{"speed",1.5},{"maxVisionDistance",90},{"foodPriority",4}};
    int sampleSet;
    float health;
};
class Predator{
public:
    std::map<std::string,int> genes{{"size",1},{"speed",1.5},{"maxVisionDistance",90}};
    Object* body;
    Mesh* mesh;
    Shader* shader;
    Prey* targetFood;
    bool isAlive=true,foundFood=false,givenUp=false;
    double health=50;
    bool mitosis,young=false;
    
    Predator(Mesh* m,Shader* shad){
        body=new Object(m,shad,"Predator");
        shader=shad;
        mesh=m;
    }
    void eat(){
        health+=7;
    }
    void changeCharacteristics(float size,float speed,int maxVisionDistance){
        body->scaleModel(glm::vec3(size));
        genes["size"]=size;
        genes["speed"]=speed;
        genes["maxVisionDistance"]=maxVisionDistance;
        
    }
};


#endif /* Organisms_h */
