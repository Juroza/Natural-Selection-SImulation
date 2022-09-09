//
//  main.cpp
//  Natural Selection Simulation
//
//  Created by Jamaine Scarlett on 19/07/2021.
//


#include <iostream>
#include <sqlite3.h>
#include "engine/Window.h"
#include "engine/Shader.h"
#include "engine/mesh.h"
#include "engine/Object.h"
#include "engine/Renderer.h"
#include "engine/Camera.h"
#include "engine/LightObject.h"
#include <cstdlib>
#include <ctime>
#include <array>
#include <string>
#include "engine/Window.h"
#include "engine/Shader.h"
#include "engine/mesh.h"
#include "engine/Object.h"
#include "engine/Renderer.h"
#include "engine/Camera.h"
#include "engine/LightObject.h"
#include "Organisms.h"
#include <stdio.h>
#include <math.h>
#include <algorithm>
#include <fstream>
#include<ctype.h>
#include <tuple>
#include <iterator>
Window window(1000,600,"Simulation");
void viewSimulationResults();
sqlite3* DB;
char* messaggeError;
int sqlexit = 0;
int sqlINT;
char sqlString;
vector<string> listofSQLString;
const std::string genes[4]={"size","speed","maxVisionDistance","foodPriority"};
int main();

float makePositive(float num){
    if(num<0){
        num*=-1;
    }
    return num;
}

string toUpper(string input){
    transform(input.begin(), input.end(), input.begin(), ::toupper);
    return input;
}

bool calculateFoodPriority(int num,Prey* prey){
    if(num<=prey->genes["foodPriority"]){
        return true;
    }
    return false;
}

std::vector<Predator*> predatorPreyInteraction(std::vector<Predator*> predatorPopulation,std::vector<Prey*> preyPopulation,Renderer* renderer,Shader* lightCubeShader,Mesh* predatorMesh,bool canReproduce){
    bool eatAnyway=false;
    if(preyPopulation.size()<=predatorPopulation.size()){
        eatAnyway=true;
    }
    for(int i=0;i<predatorPopulation.size();i++){
      //
        predatorPopulation[i]->health-=0.085f;
        if(predatorPopulation[i]->health<=0){
            //Kills predator
            if(predatorPopulation[i]->targetFood!=nullptr){
                predatorPopulation[i]->targetFood->isTargeted=false;
                for(int i=0;i<predatorPopulation[i]->targetFood->threat.size();i++){
                    if(predatorPopulation[i]->targetFood->threat[i]==predatorPopulation[i]){
                        cout<<"REMOVED------------------------";
                        predatorPopulation[i]->targetFood->threat[i]=NULL;
                        break;
                    }
                }
            }
           
            renderer->removeObjectToRender(predatorPopulation[i]->body);
            delete predatorPopulation[i];
            predatorPopulation.erase(predatorPopulation.begin()+i);
            if(predatorPopulation.size()==0){
                break;
            }
            continue;
        }
        if(predatorPopulation[i]->mitosis){
            //Creates new predator with the same characterstics as the parent
            Predator* offspring=new Predator(predatorMesh,lightCubeShader);
            renderer->addObjectToRender(offspring->body);
            offspring->body->IsaffectedByLight(true);
            offspring->young=true;
            offspring->body->changePosition(glm::vec3(predatorPopulation[i]->body->getPosition().x,1,predatorPopulation[i]->body->getPosition().z));
            offspring->changeCharacteristics(predatorPopulation[i]->genes["size"], predatorPopulation[i]->genes["speed"], predatorPopulation[i]->genes["maxVisionDistance"]);
            predatorPopulation.push_back(offspring);
            predatorPopulation[i]->mitosis=false;
        
        }
        if(predatorPopulation[i]->health>=50&& canReproduce&&!(predatorPopulation[i]->mitosis)&&!(predatorPopulation[i]->young)){
            predatorPopulation[i]->mitosis=true;
            
        }
        if(canReproduce){
            predatorPopulation[i]->young=false;
        }
        if(preyPopulation.size()==0){
            continue;
        }
        if(!(predatorPopulation[i]->foundFood)){
            int searchRadius=1;
            while(!(predatorPopulation[i]->foundFood) && !(predatorPopulation[i]->givenUp)){
                //Sets up initial search radius
                int minX=predatorPopulation[i]->body->getPosition().x-searchRadius;
                int minZ=predatorPopulation[i]->body->getPosition().z-searchRadius;
                int maxX=predatorPopulation[i]->body->getPosition().x+searchRadius;
                int maxZ=predatorPopulation[i]->body->getPosition().z+searchRadius;
                for(int j=0;j<preyPopulation.size();j++){
                    //If there are any prey within the search radius that are not already targeted, that prey is targeted
                    /*If there is an extremely low prey population the "eatAnyway" condition will cause the predators
                     to swarm around the prey
                    */
                    if((minX<=preyPopulation[j]->body->getPosition().x && maxX>=preyPopulation[j]->body->getPosition().x)
                       &&
                       (minZ<=preyPopulation[j]->body->getPosition().z && maxZ>=preyPopulation[j]->body->getPosition().z)
                       &&
                       (!(preyPopulation[j]->isTargeted) || eatAnyway)
                       ){
                        predatorPopulation[i]->targetFood=preyPopulation[j];
                        preyPopulation[j]->threat.push_back(predatorPopulation[i]);
                        preyPopulation[j]->isTargeted=true;
                        preyPopulation[j]->body->changeColour(glm::vec3(1,0,0), 1);
                        predatorPopulation[i]->foundFood=true;
                        break;
                        
                    }
                       
                }
                searchRadius+=1;
                if(searchRadius>=predatorPopulation[i]->genes["maxVisionDistance"]){
                    //Ensures that the predator will not infinitely try to find prey
                    predatorPopulation[i]->givenUp=true;
                }
            }
        }else if((!(predatorPopulation[i]->givenUp))){
                int speed=predatorPopulation[i]->genes["speed"];
                float curx=predatorPopulation[i]->body->getPosition().x,curz=predatorPopulation[i]->body->getPosition().z;
                bool minorx = false,majorx = false,minorz = false,majorz = false,hitx=false,hitz=false;
                float diffmx=predatorPopulation[i]->body->getPosition().x-(predatorPopulation[i]->targetFood->body->getPosition().x)-0.5f;
                diffmx=makePositive(diffmx);
                float diffex=predatorPopulation[i]->body->getPosition().x-predatorPopulation[i]->targetFood->body->getPosition().x+0.5f;
                diffex=makePositive(diffex);
                float diffmz=predatorPopulation[i]->body->getPosition().z-predatorPopulation[i]->targetFood->body->getPosition().z-0.5f;
                diffmz=makePositive(diffmz);
                float diffez=predatorPopulation[i]->body->getPosition().z-predatorPopulation[i]->targetFood->body->getPosition().z+0.5f;
                diffez=makePositive(diffez);
                if(diffmx<0.6f or diffex<0.6f){
                    hitx=true;
                }
                if(diffmz<0.6f or diffez<0.6f){
                    hitz=true;
                }
                if(!(hitx&&hitz)){
                    if(diffmx<diffex){
                        minorx=true;
                    }else{
                        majorx=true;
                    }
                    if(diffmz<diffez){
                        minorz=true;
                    }else{
                        majorz=true;
                    }
                    if(!(predatorPopulation[i]->targetFood->body->getPosition().x-0.3f<=predatorPopulation[i]->body->getPosition().x-0.5f&&minorx)){
                       // std::cout<<"minorx";
                        predatorPopulation[i]->body->changePosition(glm::vec3(curx+0.05*speed,1,curz));
                    }
                    if(!(predatorPopulation[i]->targetFood->body->getPosition().x+0.3f>=predatorPopulation[i]->body->getPosition().x+0.5f&&majorx)){
                       // std::cout<<"majorx";
                        predatorPopulation[i]->body->changePosition(glm::vec3(curx-0.05*speed,1,curz));
                    }
                    curx=predatorPopulation[i]->body->getPosition().x;
                    if(!(predatorPopulation[i]->targetFood->body->getPosition().z-0.3f<=predatorPopulation[i]->body->getPosition().z-0.5f&&minorz)){
                        //std::cout<<"minorz";
                        predatorPopulation[i]->body->changePosition(glm::vec3(curx,1,curz+0.05f*speed));
                    }
                    if(!(predatorPopulation[i]->targetFood->body->getPosition().z+0.3f>=predatorPopulation[i]->body->getPosition().z+0.5f&&majorz)){
                        //std::cout<<"majorz";
                        predatorPopulation[i]->body->changePosition(glm::vec3(curx,1,curz-0.05*speed));
                    }
                }else{
                        predatorPopulation[i]->targetFood->takeDamage();
                        if(predatorPopulation[i]->targetFood->health<=0){
                            predatorPopulation[i]->targetFood->isAlive=false;
                            predatorPopulation[i]->foundFood=false;
                            predatorPopulation[i]->eat();
                        }
                    }
            
        }else{
            predatorPopulation[i]->foundFood=false;
            predatorPopulation[i]->targetFood=NULL;
            
        }
        
    }
    return predatorPopulation;
}
void preyRun(Prey* prey,int areaofLand){
    float curX=prey->body->getPosition().x,curZ=prey->body->getPosition().z,curY=prey->body->getPosition().y;
    float tempX=0,tempZ=0;
    bool corneredX=false,corneredZ=false;
    for(int i =0;i<prey->threat.size();i++){
        //As the prey can have multiple predators targetting it, it must take into account all of them to escape
        if(prey->threat[i]==NULL){
            break;
        }
        if(prey->threat[i]->body==NULL){
            break;
        }
        cout<<prey;
        cout<<prey->threat[i];
        if(prey->threat[i]->body->getPosition().x>=prey->body->getPosition().x){
            tempX-=1;
        }else{
            tempX+=1;
        }
        if(prey->threat[i]->body->getPosition().z>=prey->body->getPosition().z){
            tempZ-=1;
        }else{
            tempZ+=1;
        }
    }
    if(tempX>=0){
        prey->body->changePosition(glm::vec3(curX+0.05f*prey->genes["speed"],curY,curZ));
        if(prey->body->getPosition().x>=sqrt((float)areaofLand)){
            prey->body->changePosition(glm::vec3(curX-0.05f*prey->genes["speed"],curY,curZ));
            corneredX=true;
        }
    }else{
        prey->body->changePosition(glm::vec3(curX-0.05f*prey->genes["speed"],curY,curZ));
        if(prey->body->getPosition().x<=-sqrt((float)areaofLand)){
            prey->body->changePosition(glm::vec3(curX+0.05f*prey->genes["speed"],curY,curZ));
            corneredX=true;
        }
    }
    if(tempZ>=0){
        prey->body->changePosition(glm::vec3(curX,curY,curZ+0.05f*prey->genes["speed"]));
        
        if(prey->body->getPosition().z>=sqrt((float)areaofLand)){
            prey->body->changePosition(glm::vec3(curX,curY,curZ-0.05f*prey->genes["speed"]));
            corneredZ=true;
        }
    }else{
        prey->body->changePosition(glm::vec3(curX,curY,curZ-0.05f*prey->genes["speed"]));
        if(prey->body->getPosition().z<=-sqrt((float)areaofLand)){
            prey->body->changePosition(glm::vec3(curX,curY,curZ+0.05f*prey->genes["speed"]));
            corneredZ=true;
        }
    }
    if(!(corneredX&&corneredZ)){
        //The prey will run for as long as it can until it is cornered on both axis
        if(corneredX){
         //   std::cout<<"CORNERED X";
            float diffmx=makePositive(prey->body->getPosition().z-(0-sqrt((float)areaofLand)));
            float diffex=makePositive(prey->body->getPosition().z-(0+sqrt((float)areaofLand)));
            if(diffmx>diffex){
                prey->body->changePosition(glm::vec3(curX,curY,curZ+0.05f*prey->genes["speed"]));
            }else{
                prey->body->changePosition(glm::vec3(curX,curY,curZ-0.05f*prey->genes["speed"]));
            }
        }else if(corneredZ){
          //  std::cout<<"CORNERED Z";
            float diffmx=makePositive(prey->body->getPosition().x-(0-sqrt((float)areaofLand)));
            float diffex=makePositive(prey->body->getPosition().x-(0+sqrt((float)areaofLand)));
            if(diffmx>diffex){
                prey->body->changePosition(glm::vec3(curX+0.05f*prey->genes["speed"],curY,curZ));
            }else{
                prey->body->changePosition(glm::vec3(curX-0.05f*prey->genes["speed"],curY,curZ));
            }
        }
    }
}

std::vector<Food*> preyFoodInteraction(Prey* prey,std::vector<Food*> allFood,int areaOfLand){
    int randNum=rand() % 10 + 1;
    bool go=false;
    if(prey->isTargeted){
        go=calculateFoodPriority(randNum, prey);
    }
    if(!prey->isTargeted){
        go=true;
    }
    if(go==false){
        preyRun(prey,areaOfLand);
        return allFood;
    }
    if(!(prey->foundFood)&&go&&(!prey->reproduceMode)){
        int searchRadius=1;
        while(!(prey->foundFood) && !(prey->givenUp)){
            int minX=prey->body->getPosition().x-searchRadius;
            int minZ=prey->body->getPosition().z-searchRadius;
            int maxX=prey->body->getPosition().x+searchRadius;
            int maxZ=prey->body->getPosition().z+searchRadius;
            for(int j=0;j<allFood.size();j++){
                if((minX<=allFood[j]->body->getPosition().x && maxX>=allFood[j]->body->getPosition().x)
                   &&
                   (minZ<=allFood[j]->body->getPosition().z && maxZ>=allFood[j]->body->getPosition().z)
                   &&
                   (!(allFood[j]->isTargeted))
                   ){
                    prey->targetFood=allFood[j];
                    allFood[j]->isTargeted=true;
                    prey->foundFood=true;
                    break;
                }
            }
            searchRadius+=1;
            if(searchRadius>=prey->genes["maxVisionDistance"]){
                prey->givenUp=true;
            }
        }
    }else if(!(prey->givenUp)&&go&&(!prey->reproduceMode)){
        int speed=prey->genes["speed"];
        float curx=prey->body->getPosition().x,curz=prey->body->getPosition().z;
        bool minorx = false,majorx = false,minorz = false,majorz = false,hitx=false,hitz=false;
        float diffmx=prey->body->getPosition().x-prey->targetFood->body->getPosition().x-0.3f;
        diffmx=makePositive(diffmx);
        float diffex=prey->body->getPosition().x-prey->targetFood->body->getPosition().x+0.3f;
        diffex=makePositive(diffex);
        float diffmz=prey->body->getPosition().z-prey->targetFood->body->getPosition().z-0.3f;
        diffmz=makePositive(diffmz);
        float diffez=prey->body->getPosition().z-prey->targetFood->body->getPosition().z+0.3f;
        diffez=makePositive(diffez);
        if(diffmx<0.3f or diffex<0.3f){
            hitx=true;
        }
        if(diffmz<0.3f or diffez<0.3f){
            hitz=true;
        }
        if(!(hitx&&hitz)){
            if(diffmx<diffex){
                minorx=true;
            }else{
                majorx=true;
            }
            if(diffmz<diffez){
                minorz=true;
            }else{
                majorz=true;
            }
            if(!(prey->targetFood->body->getPosition().x-0.3f<=prey->body->getPosition().x-0.5f&&minorx)){
               // std::cout<<"minorx";
                prey->body->changePosition(glm::vec3(curx+0.05*speed,1,curz));
            }
            if(!(prey->targetFood->body->getPosition().x+0.3f>=prey->body->getPosition().x+0.5f&&majorx)){
               // std::cout<<"majorx";
                prey->body->changePosition(glm::vec3(curx-0.05*speed,1,curz));
            }
            curx=prey->body->getPosition().x;
            if(!(prey->targetFood->body->getPosition().z-0.3f<=prey->body->getPosition().z-0.5f&&minorz)){
                //std::cout<<"minorz";
                prey->body->changePosition(glm::vec3(curx,1,curz+0.05f*speed));
            }
            if(!(prey->targetFood->body->getPosition().z+0.3f>=prey->body->getPosition().z+0.5f&&majorz)){
                //std::cout<<"majorz";
                prey->body->changePosition(glm::vec3(curx,1,curz-0.05*speed));
            }
        }else{
            prey->targetFood->eaten=true;
            prey->eat();
            prey->foundFood=false;
        }
        
        
    }
    return allFood;
    
}
std::vector<Food*> removeEatenFood(std::vector<Food*> allFood,Renderer* renderer){
    for(int i=0;i<allFood.size();i++){
        if(allFood[i]->eaten){
            //std::cout<<"EATING\n";
            renderer->removeObjectToRender(allFood[i]->body);
            delete allFood[i];
            allFood.erase(allFood.begin()+i);
        }
    }
    return allFood;
}
std::tuple<std::vector<Prey*>,std::vector<Predator*>> removeEatenPrey(std::vector<Prey*> allPrey,Prey* Prey,Renderer* renderer,std::vector<Predator*> predatorPopulation,int count){
    if(Prey->health<=0){
        for(int j=0;j<predatorPopulation.size();j++){
            if(predatorPopulation[j]->targetFood==Prey){
                predatorPopulation[j]->foundFood=false;
            }
        }
        for(int j=0;j<allPrey.size();j++){
            if(Prey==allPrey[j]->partner){
                allPrey[j]->reproduceMode=false;
                allPrey[j]->body->changeColour(glm::vec3(1,1,1), 1);
            }
        }
        renderer->removeObjectToRender(Prey->body);
        delete Prey;
        allPrey.erase(allPrey.begin()+count);
    }
    return std::make_tuple(allPrey,predatorPopulation) ;
}
std::vector<Food*> addFood(std::vector<Food*> allFood,Renderer* renderer,int num,Mesh* simple, Shader* lightSourceShader,int areaOfLand){
    for(int i=0;i<num;i++){
        Food* crate=new Food(simple,lightSourceShader);
        renderer->addObjectToRender(crate->body);
        crate->body->changeColour(glm::vec3(1,1,0.1f), 1);
        crate->body->scaleModel(glm::vec3(0.3,0.3,0.3));
        float x=(float(rand())/float((RAND_MAX)) * (sqrt(areaOfLand)+sqrt(areaOfLand))-sqrt(areaOfLand));
        float z= (float(rand())/float((RAND_MAX)) * (sqrt(areaOfLand)+sqrt(areaOfLand))-sqrt(areaOfLand));
        crate->body->changePosition(glm::vec3(x,1,z));
        allFood.push_back(crate);
        
    }
    return allFood;
}
void preyTryAgain(Prey* prey){
    if(prey->givenUp){
        prey->givenUp=false;
    }
}
void predatorTryAgain(std::vector<Predator*> predatorPopulation){
    for(int i=0;i<predatorPopulation.size();i++){
        if(predatorPopulation[i]->givenUp){
            predatorPopulation[i]->givenUp=false;
        }
    }
}
bool mutationEvent(){
    // returns true or false, 50% chance then prey will mutate
    int num=(rand() % 9);
    if(num<5){
        return true;
    }
    else{
        return false;
    }
}
std::vector<Prey*> preyReproduce(Prey* prey,std::vector<Prey*> preyPopulation,Renderer* renderer,Mesh* preyMesh,Shader* lightCubeShader){
    if(prey->reproduceMode&&!(prey->isTargeted)){
    std::cout<<"LOOKING FOR MATE";
        int speed=prey->genes["speed"];
        float curx=prey->body->getPosition().x,curz=prey->body->getPosition().z;
        bool minorx = false,majorx = false,minorz = false,majorz = false,hitx=false,hitz=false;
        float diffmx=prey->body->getPosition().x-prey->partner->body->getPosition().x-0.5f;
        diffmx=makePositive(diffmx);
        float diffex=prey->body->getPosition().x-prey->partner->body->getPosition().x+0.5f;
        diffex=makePositive(diffex);
        float diffmz=prey->body->getPosition().z-prey->partner->body->getPosition().z-0.5f;
        diffmz=makePositive(diffmz);
        float diffez=prey->body->getPosition().z-prey->partner->body->getPosition().z+0.5f;
        diffez=makePositive(diffez);
        if(diffmx<0.5f or diffex<0.5f){
            hitx=true;
        }
        if(diffmz<0.5f or diffez<0.5f){
            hitz=true;
        }
        if(!(hitx&&hitz)){
            if(diffmx<diffex){
                minorx=true;
            }else{
                majorx=true;
            }
            if(diffmz<diffez){
                minorz=true;
            }else{
                majorz=true;
            }
            if(!(prey->partner->body->getPosition().x-0.5f<=prey->body->getPosition().x-0.5f&&minorx)){
               // std::cout<<"minorx";
                prey->body->changePosition(glm::vec3(curx+0.05*speed,1,curz));
            }
            if(!(prey->partner->body->getPosition().x+0.5f>=prey->body->getPosition().x+0.5f&&majorx)){
               // std::cout<<"majorx";
                prey->body->changePosition(glm::vec3(curx-0.05*speed,1,curz));
            }
            curx=prey->body->getPosition().x;
            if(!(prey->partner->body->getPosition().z-0.5f<=prey->body->getPosition().z-0.5f&&minorz)){
                //std::cout<<"minorz";
                prey->body->changePosition(glm::vec3(curx,1,curz+0.05f*speed));
            }
            if(!(prey->partner->body->getPosition().z+0.5f>=prey->body->getPosition().z+0.5f&&majorz)){
                //std::cout<<"majorz";
                prey->body->changePosition(glm::vec3(curx,1,curz-0.05*speed));
            }
        }else{
            std::cout<<"FOUND MATE, DONE";
            prey->body->changeColour(glm::vec3(1,1,1), 1);
            prey->partner->body->changeColour(glm::vec3(1,1,1), 1);
            prey->reproduceMode=false;
            prey->partner->reproduceMode=false;
            Prey* offspring=new Prey(preyMesh,lightCubeShader);
            renderer->addObjectToRender(offspring->body);
            offspring->body->IsaffectedByLight(true);
            offspring->body->changePosition(glm::vec3(prey->body->getPosition().x,1,prey->body->getPosition().z));
            bool n=mutationEvent();
            std::cout<<n;
            int num;
            if(n){
                //MUTATE
                num=(rand() % 3);
                offspring->genes["size"]=(rand() % prey->genes["size"]+3)+(prey->genes["size"]-1);
            }
            n=mutationEvent();
            if(n){
                num=(rand() % 3);
                offspring->genes["maxVisionDistance"]=(rand() % prey->genes["maxVisionDistance"]+3)+(prey->genes["maxVisionDistance"]-1);
            }
            n=mutationEvent();
            if(n){
                num=(rand() % 3);
                offspring->genes["foodPriority"]=(rand() % prey->genes["foodPriority"]+3)+(prey->genes["foodPriority"]-1);
            }
            n=mutationEvent();
            if(n){
                num=(rand() % 3);
                offspring->genes["speed"]=(rand() % prey->genes["speed"]+3)+(prey->genes["speed"]-1);
            }
            offspring->mutate();
            preyPopulation.push_back(offspring);
        }
    }else if(prey->reproduceSignal&&!(prey->isTargeted)){
        int searchRadius=1;
        while(prey->partner!=NULL&&!(prey->givenUp)){
            int minX=prey->body->getPosition().x-searchRadius;
            int minZ=prey->body->getPosition().z-searchRadius;
            int maxX=prey->body->getPosition().x+searchRadius;
            int maxZ=prey->body->getPosition().z+searchRadius;
            for(int j=0;j<preyPopulation.size();j++){
                if((minX<=preyPopulation[j]->body->getPosition().x && maxX>=preyPopulation[j]->body->getPosition().x)
                &&
                (minZ<=preyPopulation[j]->body->getPosition().z && maxZ>=preyPopulation[j]->body->getPosition().z)
                &&
                 (preyPopulation[j]->partner!=NULL))
                       {
                           prey->partner=preyPopulation[j];
                           prey->body->changeColour(glm::vec3(0,0,1), 1);
                           preyPopulation[j]->partner=prey;
                           preyPopulation[j]->body->changeColour(glm::vec3(0,0,1), 1);
                           prey->reproduceMode=true;
                           preyPopulation[j]->reproduceMode=true;
                           prey->reproduceSignal=false;
                           preyPopulation[j]->reproduceSignal=false;
                        break;
                    }
                }
                searchRadius+=1;
                if(searchRadius>=prey->genes["maxVisionDistance"]){
                    prey->givenUp=true;
                    prey->reproduceMode=false;
                    prey->reproduceSignal=false;
                }
            }
    }
    return preyPopulation;
}
int getIndex(vector<Prey*> v, Prey* K)
{
    auto it = find(v.begin(), v.end(), K);
 
    // If element was found
    if (it != v.end())
    {
     
        // calculating the index
        // of K
        int index = it - v.begin();
        return index;
    }
    else {
        // If the element is not
        // present in the vector
        return -1;
    }
}


std::tuple<std::vector<Prey*>,std::vector<Food*>,std::vector<Predator*>,vector<Prey>> preyBehaviour(std::vector<Prey*> preyPopulation,std::vector<Predator*>predatorPopulation,Renderer* renderer,Mesh* preyMesh,Shader* lightCubeShader,std::vector<Food*> allFood,int areaOfLand,bool save){
    vector<Prey> tempPreyStore;
    bool food = false,retry=false;
    if(allFood.size()!=0){
        food=true;
    };
    if((int)window.getTime()%3==0){
        //Prey can give up and not look for food if it does find any. The boolean value retry is neccessary to ensure that the prey starts moving again when food has regenerated
        retry=true;
    }
    srand((unsigned int)time(NULL));
    for(int i=0;i<preyPopulation.size();i++){
        //Removing eaten prey must be done first to ensure that
        std::tie(preyPopulation,predatorPopulation)=removeEatenPrey(preyPopulation,preyPopulation[i], renderer,predatorPopulation,i);
        if(preyPopulation.size()==0){
            return std::make_tuple(preyPopulation,allFood,predatorPopulation,tempPreyStore);
        }
        if(getIndex(preyPopulation, preyPopulation[i])>preyPopulation.size()){
            //This ensures that the prey currently being iterated through exists, avoiding an error when trying to operate on a non existent prey
            continue;
        }
        if(retry){
            preyTryAgain(preyPopulation[i]);
        }
        if(save){
            // Stores a copy of the prey into the sample if it is time to sample
            tempPreyStore.push_back(*preyPopulation[i]);
        }
        if(food){
            allFood=removeEatenFood(allFood, renderer);
            allFood=preyFoodInteraction(preyPopulation[i], allFood,areaOfLand);
        }
        preyPopulation[i]->health-=0.05f;
        preyPopulation=preyReproduce(preyPopulation[i],preyPopulation,renderer,preyMesh,lightCubeShader);
        //Ensures prey can die of hunger
    }
    return std::make_tuple(preyPopulation,allFood,predatorPopulation,tempPreyStore);
};
void validateSQLCommand(char* messageError){
    if (sqlexit != SQLITE_OK) {
        std::cerr << "Error!" << std::endl;
        sqlite3_free(messageError);
    }
}
static int getSQLIntValue(void* data, int argc, char** argv, char** azColName)
{
    sqlINT=*argv[0]-'0';
    return 0;

}
static int getSQLStringValue(void* data, int argc, char** argv, char** azColName)
{
    for (int i = 0; i < argc; i++) {
        char value[50];
        strcpy(value, argv[i]);
        listofSQLString.push_back(value);
    }
    
    return 0;

}
static int getSQLSingleStringValue(void* data, int argc, char** argv, char** azColName)
{
    sqlString=*argv[0];
    return 0;

}

void storePreySamplesIntoDatabase(vector<tuple<vector<Prey>,float>> samplePreyData,string simulationName){
    string sampleScheme="CREATE TABLE IF NOT EXISTS "+simulationName+"("
    "SampleID INT NOT NULL, "
    "PreySize FLOAT NOT NULL, "
    "PreySpeed FLOAT NOT NULL, "
    "PreyMaxVisionDistance INT NOT NULL, "
    "PreyFoodPriority INT NOT NULL, "
    "PreyHealth INT NOT NULL); ";
    sqlexit = sqlite3_exec(DB, sampleScheme.c_str(), NULL, 0, &messaggeError);
    validateSQLCommand(messaggeError);
    string insertPrey;
    for(int i=0;i<samplePreyData.size();i++){
        for(int j=0;j<get<0>(samplePreyData[i]).size();j++){
            Prey prey=get<0>(samplePreyData[i])[j];
            insertPrey="INSERT INTO "+simulationName+" VALUES("+to_string(i)+","+to_string(prey.genes["size"])+","+to_string(prey.genes["speed"])+","+to_string(prey.genes["maxVisionDistance"])+","+to_string(prey.genes["foodPriority"])+","+to_string(prey.health)+");";
            
            sqlexit = sqlite3_exec(DB, insertPrey.c_str(), NULL, 0, &messaggeError);
            
        }
        
    }
}
void storePreySamplesIntoTextFile(vector<tuple<vector<Prey>,float>> samplePreyData,string simulationName){
    ofstream simulationFile;
    simulationFile.open(simulationName+".txt");
    for(int i=0;i<samplePreyData.size();i++){
        for(int j=0;j<get<0>(samplePreyData[i]).size();j++){
            Prey prey=get<0>(samplePreyData[i])[j];
            simulationFile <<to_string(i)+";";
            simulationFile <<to_string(prey.genes["size"])+";";
            simulationFile <<to_string(prey.genes["speed"])+";";
            simulationFile <<to_string(prey.genes["maxVisionDistance"])+";";
            simulationFile <<to_string(prey.genes["foodPriority"])+";";
            simulationFile <<to_string(prey.health)+"\n";
            
        }
        
    }
    simulationFile.close();
}
int getDataMenuChoice(){
    int num = 0;
    bool valid=false;
    cout<<"\n"<<"---------DATA MENU---------"<<"\n"<<"(1)-View Simulation Results\n"<<"(2)-Delete Simulation\n"<<"(3)-Return\n";
    while(!valid){
        cin>>num;
        if(1<=num && num<=3){
            valid=true;
        }else{
            cout<<"Please Enter Valid Choice 1-2-3";
        }
    }
    return num;
}
void deleteSimulationData(){
    string simulationName,selectStatement,deleteStatement;
    int selectedSim;
    selectStatement="SELECT SimName FROM SimulationTable;";
    sqlite3_exec(DB, selectStatement.c_str(), getSQLStringValue,0, NULL);
    cout<<"-----Saved Simulations-----\n";
    for(int i=0;i<listofSQLString.size();i++){
        cout<<i<<"--"<<listofSQLString[i]<<"\n";
    }
    cout<<"Enter Number of Simulation you want to delete(or -1 to go back)";
    cin>>selectedSim;
    if(selectedSim==-1){
        listofSQLString.clear();
        main();
    }
    deleteStatement="DELETE FROM SimulationTable WHERE SimName='"+listofSQLString[selectedSim]+"'";
    sqlite3_exec(DB, deleteStatement.c_str(), getSQLStringValue,0, NULL);
    if(remove((listofSQLString[selectedSim]+".txt").c_str())){
        cout<<"\nError Deleting simulation File\n";
    }else{
        cout<<"\nSimulation File was successfully deleted\n";
    }
    listofSQLString.clear();
    main();
}
void processPoplulationOverTime(vector<sampledPrey> input){
    vector<int> population;
    int total=0,x=0;
    for(int i=0;i<input.size()-1;i++){
        if(input[i].sampleSet==input[i+1].sampleSet){
            total=total+2;
        }else{
            total+=1;
            population.push_back(total);
            cout<<x<<" months:"<<total<<"\n";
            x+=3;
            total=0;
        }
    }
    listofSQLString.clear();
}

const char* calculateHighestValue(std::unordered_map<const char*, int> input){
    const char* highestValuekey = nullptr;
    int highestValue=0;
    unordered_map<const char*, int>::iterator it;
    for (it = input.begin(); it != input.end(); it++)
    {
        if(it->second>highestValue){
            highestValue=it->second;
            highestValuekey=it->first;
        }
        
    }
    return highestValuekey;
    
    
}
void calculateMostCommonAlleles(vector<sampledPrey> input){
    std::unordered_map<const char*, int> maxVisionDistance;
    std::unordered_map<const char*, int> speed;
    std::unordered_map<const char*, int> foodPriority;
    std::unordered_map<const char*, int> size;
    for(int i=0;i<input.size();i++){
        maxVisionDistance[(to_string(input[i].genes["maxVisionDistance"])).c_str()]+=1;
        speed[(to_string(input[i].genes["speed"])).c_str()]+=1;
        foodPriority[(to_string(input[i].genes["foodPriority"])).c_str()]+=1;
        size[(to_string(input[i].genes["size"])).c_str()]+=1;
    }
    cout<<"\nMost common Vision distance: "<<calculateHighestValue(maxVisionDistance);
    cout<<"\nMost common Speed: "<<calculateHighestValue(speed);
    cout<<"\nMost common size: "<<calculateHighestValue(size);
    cout<<"\nMost common foodPriority: "<<calculateHighestValue(foodPriority)<<"\n";
}
void calculateMostSuccessfullPrey(vector<sampledPrey> input){
    sampledPrey apexPrey;
    apexPrey.health=0;
    for (int i =0; i<input.size(); i++) {
        if((input[i].health-(input[i].genes["size"])*50)>(apexPrey.health-(apexPrey.genes["size"]*50))){
            apexPrey.genes["maxVisionDistance"]=input[i].genes["maxVisionDistance"];
            apexPrey.genes["speed"]=input[i].genes["speed"];
            apexPrey.genes["foodPriority"]=input[i].genes["foodPriority"];
            apexPrey.genes["size"]=input[i].genes["size"];
            apexPrey.health=input[i].health;
        }
    }
    cout<<"\nMost successful Prey's Genes:";
    cout<<"\nMax Vision Distance: "<<apexPrey.genes["maxVisionDistance"];
    cout<<"\nSpeed: "<<apexPrey.genes["speed"];
    cout<<"\nSize: "<<apexPrey.genes["size"];
    cout<<"\nFood Priority: "<<apexPrey.genes["foodPriority"]<<"\n";
}
void processPoplulationOverTime(vector<vector<sampledPrey>> input){
    int x=0;
    for(int i=0;i<input.size();i++){
        cout<<"\n-----------------------------------------------";
        cout<<"\nMonth: "<<x<<"\nPopulation: "<<input[i].size()<<"\n";
        if(input[i].size()==0){
            continue;
        }
        calculateMostCommonAlleles(input[i]);
        calculateMostSuccessfullPrey(input[i]);
        x+=3;
        
    }
    listofSQLString.clear();
}

vector<vector<sampledPrey>> seperateSampleSets(vector<sampledPrey> input){
    vector<vector<sampledPrey>> setOfSamples;
    int currentSet=0;
    vector<sampledPrey> oneSampleGen;
    for(int i=0;i<input.size();i++){
        if(currentSet==input[i].sampleSet){
            oneSampleGen.push_back(input[i]);
        }else{
            currentSet=input[i].sampleSet;
            setOfSamples.push_back(oneSampleGen);
            oneSampleGen.clear();
        }
    }
    return setOfSamples;
}

void viewSimulationResults(){
    int selectedSim;
    string simulationName,selectStatement;
    selectStatement="SELECT SimName FROM SimulationTable;";
    sqlite3_exec(DB, selectStatement.c_str(), getSQLStringValue,0, NULL);
    cout<<"-----Saved Simulations-----\n";
    for(int i=0;i<listofSQLString.size();i++){
        cout<<i<<"--"<<listofSQLString[i]<<"\n";
    }
    cout<<"Enter Desired Simulation Number(or -1 to go back): ";
    cin>>selectedSim;
    if(selectedSim==-1){
        main();
    }
    string line;
    vector<sampledPrey> v;
    ifstream myfile (listofSQLString[selectedSim]+".txt");
    sampledPrey sample;
      if (myfile.is_open())
      {
        while ( getline (myfile,line) )
        {
         // cout << line << '\n';
             
                stringstream ss(line);
                while (ss.good()) {
                    string substr;
                    getline(ss, substr, ';');
                    sample.sampleSet=stoi(substr);
                 
                    getline(ss, substr, ';');
                    sample.genes["size"]=stoi(substr);
                  
                    getline(ss, substr, ';');
                    sample.genes["speed"]=stof(substr);
             
                    getline(ss, substr, ';');
                    sample.genes["maxVisionDistance"]=stoi(substr);
                    getline(ss, substr, ';');
                    sample.genes["foodPriority"]=stoi(substr);
                  
                    getline(ss, substr, ';');
                    sample.health=stof(substr);

                }
            v.push_back(sample);
            
            }
        }
    //for (size_t i = 0; i < v.size(); i++){
    
   //     myfile.close();
    //}
    vector<vector<sampledPrey>> setOfSamples;
    setOfSamples=seperateSampleSets(v);
    processPoplulationOverTime(setOfSamples);
    listofSQLString.clear();
    
    main();
}

int main(){
    listofSQLString.clear();
    //--------------------------SQL STUFF-----------------------------//
    string userScheme="CREATE TABLE IF NOT EXISTS UserTable("
    "UserID integer PRIMARY KEY   AUTOINCREMENT, "
    "UserName   TEXT    NOT NULL);";
    string simulationScheme="CREATE TABLE IF NOT EXISTS SimulationTable("
    "SimID integer PRIMARY KEY   AUTOINCREMENT, "
    "SimName   TEXT    NOT NULL,"
    "UserID integer NOT NULL,"
    "FOREIGN KEY (UserID) REFERENCES UserTable(UserID));";
    
   sqlexit = sqlite3_open("NSS.db", &DB);
    sqlexit = sqlite3_exec(DB, userScheme.c_str(), NULL, 0, &messaggeError);
    validateSQLCommand(messaggeError);
    sqlexit = sqlite3_exec(DB, simulationScheme.c_str(), NULL, 0, &messaggeError);
    validateSQLCommand(messaggeError);
    
    //--------------------------SQL STUFF-----------------------------//
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    std::vector<float> vertices = {
        -0.5f, -0.5f, -0.5f,  0.0f, 0.0f,
         0.5f, -0.5f, -0.5f,  1.0f, 0.0f,
         0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
         0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
        -0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, 0.0f,

        -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
         0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
         0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
         0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
        -0.5f,  0.5f,  0.5f,  0.0f, 1.0f,
        -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,

        -0.5f,  0.5f,  0.5f,  0.0f, 0.0f,
        -0.5f,  0.5f, -0.5f,  1.0f, 0.0f,
        -0.5f, -0.5f, -0.5f,  1.0f, 1.0f,
        -0.5f, -0.5f, -0.5f,  1.0f, 1.0f,
        -0.5f, -0.5f,  0.5f,  0.0f, 1.0f,
        -0.5f,  0.5f,  0.5f,  0.0f, 0.0f,

         0.5f,  0.5f,  0.5f,  0.0f, 0.0f,
         0.5f,  0.5f, -0.5f,  1.0f, 0.0f,
         0.5f, -0.5f, -0.5f,  1.0f, 1.0f,
         0.5f, -0.5f, -0.5f,  1.0f, 1.0f,
         0.5f, -0.5f,  0.5f,  0.0f, 1.0f,
         0.5f,  0.5f,  0.5f,  0.0f, 0.0f,

        -0.5f, -0.5f, -0.5f,  0.0f, 0.0f,
         0.5f, -0.5f, -0.5f,  1.0f, 0.0f,
         0.5f, -0.5f,  0.5f,  1.0f, 1.0f,
         0.5f, -0.5f,  0.5f,  1.0f, 1.0f,
        -0.5f, -0.5f,  0.5f,  0.0f, 1.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, 0.0f,

        -0.5f,  0.5f, -0.5f,  0.0f, 0.0f,
         0.5f,  0.5f, -0.5f,  1.0f, 0.0f,
         0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
         0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
        -0.5f,  0.5f,  0.5f,  0.0f, 1.0f,
        -0.5f,  0.5f, -0.5f,  0.0f, 0.0f
    };
    // verticesEX is exclusively used for the light source as it is the only object in the scene that does not require a texture.
    std::vector<float> verticesEX = {
        -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
         0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
         0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
         0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
        -0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
        -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,

        -0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,
         0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,
        -0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,
        -0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,

        -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,
        -0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
        -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
        -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
        -0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f,
        -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,

         0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,
         0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
         0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
         0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
         0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f,
         0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,

        -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,
         0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,
         0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
         0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
        -0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,

        -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,
         0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
        -0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
        -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f
    };
    std::vector<float> groundVert ={
             0.5f,  0.5f, 0.0f,1,1,  // top right
             0.5f, -0.5f, 0.0f,1,0,  // bottom right
            -0.5f, -0.5f, 0.0f,0,0,  // bottom left
            -0.5f,  0.5f, 0.0f,1,0   // top left
    };
    std::vector<float>groundIndices = {
        0, 1, 3,   // first triangle
        1, 2, 3    // second triangle
    };
    
    
    /*VerticesTEX is used to create the cube shape, with any texture.
     This will be passed into an instance of the "mesh" class, where it will be
     prepared to be used by the GPU.
*/
    std::vector<float> verticesTEX = {
        // positions          // normals           // texture coords
        -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f, 0.0f,
         0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f, 0.0f,
         0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f, 1.0f,
         0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f, 1.0f,
        -0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f, 1.0f,
        -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f, 0.0f,

        -0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   0.0f, 0.0f,
         0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   1.0f, 0.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   1.0f, 1.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   1.0f, 1.0f,
        -0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   0.0f, 1.0f,
        -0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   0.0f, 0.0f,

        -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  0.0f, 0.0f,
        -0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  1.0f, 0.0f,
        -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  1.0f, 1.0f,
        -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  1.0f, 1.0f,
        -0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
        -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  0.0f, 0.0f,

         0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  0.0f, 0.0f,
         0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f,
         0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  1.0f, 1.0f,
         0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  1.0f, 1.0f,
         0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
         0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  0.0f, 0.0f,

        -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f, 0.0f,
         0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  1.0f, 0.0f,
         0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f, 1.0f,
         0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f, 1.0f,
        -0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  0.0f, 1.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f, 0.0f,

        -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f, 0.0f,
         0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  1.0f, 0.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f, 1.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f, 1.0f,
        -0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  0.0f, 1.0f,
        -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f, 0.0f
    };
    std::vector<int> indices =
    {  // Note that we start from 0!
        0
    };
//SET UP GRAPHICS OF THE CUBES
    //To save memory space these mesh objects are created only once, able to be used by multiple objects with similar visuals.
    Mesh groundMesh(verticesTEX,indices,8,0,3,"res/images/grass.jpg");
    Mesh predatorMesh(verticesTEX,indices,8,0,3,"res/images/predatorTexture.png");
    Mesh preyMesh(verticesTEX,indices,8,0,3,"res/images/preyTexture.png");
    Mesh simple(verticesEX,indices,6,0,0,"");
    
    Shader shader("res/shaders/VertexShader2.glsl","res/shaders/mainFragmentShader.glsl");
    Shader lightCubeShader("res/shaders/TexturedLightingVertexShader.glsl","res/shaders/TexturedPointLightFragmentShader.glsl");
    Shader lightSourceShader("res/shaders/lightSourceVertexShader.glsl","res/shaders/lightSourceFragmentShader.glsl");
    
    LightObject lightSourceCube(&simple,&lightSourceShader,"LightSource");
    Camera camera(45.0f,( float )window.getScreenWidth() / ( float )window.getScreenHeight(),0.1f, 100.0f,glm::vec3(0.0f,0.0f,3.0f));
    //the memory address of the camera must be accessible by the renderer as the user must be able to look around with the camera.
    Renderer renderer(&camera);


    int n;
    Object ground(&groundMesh,&lightCubeShader,"CubeTEX2");
    renderer.addLightObjectToRender(&lightSourceCube);
    
    //Sets up the position and characteristics of the light
    
    lightSourceCube.changePosition(glm::vec3( 0, 30, 0));
    lightSourceCube.setLinear(0.09f);
    lightSourceCube.setQuadratic(0.032f);

    lightSourceCube.changeColour(glm::vec3(1.0f,1.0f,1.0f),1.0f);

    lightSourceCube.setLightEmitColor(glm::vec3(1,1,1));
    lightSourceCube.setLightIntensity(10);
    camera.setInnerCutOff(90.3f);
    camera.setOuterCutOff(180.5f);

    float x = 1,y = 1,z = 1;
    string splashINFO=
    "\n|---------------------------------------|\n"
    "|      Natural Selection Simulation     |\n"
    "|  This is a tool to demonstrate Natural|\n"
    "|  Selection.                           |\n"
    "|                                       |\n"
    "| You have the option to make a username|\n"
    "| If you choose this option you will be |\n"
    "| able to save your simulation resutls, |\n"
    "| and anylase it                        |\n"
    "|                                       |\n"
    "| Within the simulation you can move    |\n"
    "| the camera with 'W,A,S,D' and the     |\n"
    "| mouse.                                |\n"
    "| Terminate the Simulation by pressing  |\n"
    "| 'T'                                   |\n"
    "|---------------------------------------|\n";
    cout<<splashINFO;
    window.setBackgroundColour(0.0f, 0.05f, 0.1f);
    //samplePreyData stores samples of the prey population and will be used to save this data when the simulation ends.
    vector<tuple<vector<Prey>,float>> samplePreyData;
    /* The prey, predator and food objects are stored as pointers to ensure that changes to their states are recogonised throughout the program
     */
    vector<Prey*> preyPopulation;
    vector<Predator*> predatorPopulation;
    vector<Food*> allFood;
    bool userDefined=false, valid=false;
    string userName,simulationName;
    string response;
    string data("CALLBACK FUNCTION");
    cout<<"Start a new Simulation (S) or Interact with Data(D)(Enter S or D or E to exit):";
    while(!(valid)){
        try {
            cin>>response;
            if(response.length()==1&&(toupper(response[0])=='S'|toupper(response[0])=='E'|toupper(response[0])=='D')){
                valid=true;
                response=toupper(response[0]);
                break;
            }else{
                cout<<"Enter S or D";
            }
        } catch (exception) {
            cout<<"Enter S or D";
        }
        
    }
    valid=false;
    bool saveMode;
    if(response=="S"){
        cout<<"Run under a UserName?(Y or N)";
        while(!(valid)){
            try {
                cin>>response;
                if(response.length()==1&&(toupper(response[0])=='Y'|toupper(response[0])=='N')){
                    valid=true;
                    response=toupper(response[0]);
                    break;
                }else{
                    cout<<"Enter Y or N";
                }
            } catch (exception) {
                cout<<"Enter Y or N";
            }
            
        }
        valid=false;
        if(response=="Y"){
            cout<<"Enter UserName(Cannot contain Spaces): ";
            cin>>userName;
            string query="SELECT COUNT(1) "
            "FROM UserTable "
            "WHERE UserName = '"+userName+"';";
            sqlexit =sqlite3_exec(DB, query.c_str(), getSQLIntValue,0, NULL);
            
            validateSQLCommand(messaggeError);
            cout<<sqlINT;
            if(sqlINT){
                
            }else{
                string newUser="INSERT INTO UserTable VALUES(NULL,'"+userName+"');";
                sqlexit =sqlite3_exec(DB, newUser.c_str(), NULL, 0, &messaggeError);
                validateSQLCommand(messaggeError);
            }
            string query1="SELECT UserID FROM UserTable "
            "WHERE UserName ='"+userName+"';";
            sqlexit =sqlite3_exec(DB, query1.c_str(), getSQLSingleStringValue,0, &messaggeError);
            validateSQLCommand(messaggeError);
            cout<<"Enter Simulation Name (Cannot contain Spaces): ";
            cin>>simulationName;
            cout<<sqlString;
            string newSim="INSERT INTO SimulationTable VALUES(NULL,'"+simulationName+"',"+sqlString+");";
            sqlexit =sqlite3_exec(DB, newSim.c_str(), NULL, 0, &messaggeError);
  
            window.changeWindowTitle(simulationName.c_str());
            saveMode=true;
        }else{
            saveMode=false;
        }
        
        
        int initialPreyPopulation=0,initialPredatorPopulation=0;
        double areaOfLand;
        cout<<"Recommendation: Let the number of prey be 10 times the number of predators\n";
        while(!valid){
            cout<<"Enter Initial Prey Population: ";
            cin>>initialPreyPopulation;
            if(initialPreyPopulation!=0){
                valid=true;
            }
        }
        cout<<"\n"<<"Enter Initial Predator Population:";
        cin>>initialPredatorPopulation;
        float size,speed;
        int visdis;
        cout<<"-----------PREDATOR DESIGN-----------\n";
        cout<<"Hint: Entering '1' is the normal value. Enter a number equal to or more than 1\n";
        cout<<"Predator Size:";
        cin>>size;
        cout<<"\nPredator Speed:";
           cin>>speed;
           cout<<"\nPredator Vision Distance:";
           cin>>visdis;
        cout<<"Recommended: Make Land Area more than or equal to 100";
           cout<<"\n"<<"Enter Land Area:";
           cin>>areaOfLand;
           srand((unsigned int)time(NULL));
        //The following three for loops are used to create new organisms and place them randomly around the area of land
        for(int i=0;i<initialPreyPopulation;i++){
            //instantiates a new Prey
            Prey* crate=new Prey(&preyMesh,&lightCubeShader);
            //Lets renderer class know it must render it
            renderer.addObjectToRender(crate->body);
            crate->body->IsaffectedByLight(true);
            //Set its spawn position
            float x=(float(rand())/float((RAND_MAX)) * (sqrt(areaOfLand)+sqrt(areaOfLand))-sqrt(areaOfLand));
            float z= (float(rand())/float((RAND_MAX)) * (sqrt(areaOfLand)+sqrt(areaOfLand))-sqrt(areaOfLand));
            crate->body->changePosition(glm::vec3(x,1,z));
            //Add it the prey population
            preyPopulation.push_back(crate);
                             
        }
        for(int i=0;i<initialPredatorPopulation;i++){
            //instantiate new predator
            Predator* crate=new Predator(&predatorMesh,&lightCubeShader);
            //Lets renderer class know it must render it
            renderer.addObjectToRender(crate->body);
            crate->body->IsaffectedByLight(true);
            //Set its spawn position
            float x=(float(rand())/float((RAND_MAX)) * (sqrt(areaOfLand)+sqrt(areaOfLand))-sqrt(areaOfLand));
            float z= (float(rand())/float((RAND_MAX)) * (sqrt(areaOfLand)+sqrt(areaOfLand))-sqrt(areaOfLand));
            crate->body->changePosition(glm::vec3(x,1,z));
            crate->body->changeRotation(glm::vec3(0,0,1), -45, 1);
            //Sets predator characteristics based on user input
            crate->changeCharacteristics(size, speed, visdis);
            //Add to predator population
            predatorPopulation.push_back(crate);
                             
        }
        for(int i=0;i<areaOfLand;i++){
            Food* crate=new Food(&simple,&lightSourceShader);
            //Lets renderer class know it must render it
            renderer.addObjectToRender(crate->body);
            crate->body->changeColour(glm::vec3(1,1,0.1f), 1);
            crate->body->scaleModel(glm::vec3(0.3,0.3,0.3));
            //Set its spawn position
            float x=(float(rand())/float((RAND_MAX)) * (sqrt(areaOfLand)+sqrt(areaOfLand))-sqrt(areaOfLand));
            float z= (float(rand())/float((RAND_MAX)) * (sqrt(areaOfLand)+sqrt(areaOfLand))-sqrt(areaOfLand));
            crate->body->changePosition(glm::vec3(x,1,z));
            //add to food population
            allFood.push_back(crate);
                             
        }
        //ground.scaleModel(glm::vec3(sqrt(areaOfLand),1,sqrt(areaOfLand)));
        renderer.addObjectToRender(&ground);
        ground.changePosition(glm::vec3(0,0,0));
        ground.scaleModel(glm::vec3(sqrt(areaOfLand)*2+0.5f,0.5,sqrt(areaOfLand)*2)+0.5f);
        
        //If the land is too big the light may not reach the entire area, so 50 units is as high the light source will go.
        if(areaOfLand>=50){
            camera.changeView(glm::vec3(0.0f,20,0.0f));
        }else{
            camera.changeView(glm::vec3(0.0f,areaOfLand,0.0f));
        }
        camera.cameraFront=glm::vec3(0,-1,0);
        
        //predCanRepro-indicates when the predators can multiply
        //Save- indicates when a the preyPopulation should be sampled
        bool predCanRepro=false,save=false;
        float predRepro=glfwGetTime()+5,saveRate=glfwGetTime()+3;
        window.open();
        while(!window.shouldClose()){
            if(glfwGetTime()>=saveRate&&saveMode){
                save=true;
            }

            vector<Prey> tempPreyStore;
            if(glfwGetTime()>=predRepro){
                predCanRepro=true;
                predRepro=glfwGetTime()+5;
            }
            //Creates day and night Cycle effect
            window.setBackgroundColour((sin(0.5*(glfwGetTime()*0.4f))-0.5f)+1, (sin(0.5*(glfwGetTime()*0.4f))-0.2f)+1, (sin(0.5f*(glfwGetTime()*0.4f)-0.1))+1);
            lightSourceCube.changeTimeOfDay((float)(1+sin(0.5*(glfwGetTime()*0.4f))));
            lightSourceCube.setLightIntensity((float)(4+cos((glfwGetTime()*0.4f)+3)));
            predatorPopulation=predatorPreyInteraction(predatorPopulation, preyPopulation,&renderer,&lightCubeShader,&predatorMesh,predCanRepro);
            std::tie(preyPopulation,allFood,predatorPopulation,tempPreyStore)=preyBehaviour(preyPopulation,predatorPopulation, &renderer, &preyMesh, &lightCubeShader, allFood, areaOfLand,save);

            predCanRepro=false;
                        //Regenerate Food
            if((int)window.getTime()%2==0 && allFood.size()<areaOfLand){
                allFood=addFood(allFood, &renderer,1,&simple,&lightSourceShader,areaOfLand);
            }
            allFood=addFood(allFood, &renderer,(preyPopulation.size()-allFood.size()),&simple,&lightSourceShader,areaOfLand);
            window.update();
            camera.processInput(window.getWindow(),window.getDeltaTime());
            if((glfwGetKey(window.getWindow(), GLFW_KEY_T))){
                window.close();
                window.turnCursorOn();
                std::cout<<"\nRENDER OBJECTS: "<<renderer.objects.size();
            }
           // renderer.checkFood();
            if (glfwGetKey(window.getWindow(), GLFW_KEY_P) != GLFW_PRESS){
                renderer.renderScene();
            }

            if(glfwGetKey(window.getWindow(), GLFW_KEY_Q)){
                window.turnCursorOff();
            }
            if(glfwGetKey(window.getWindow(), GLFW_KEY_E)){
                window.turnCursorOn();
            }
            window.input();
            window.getFPS();
            if(save&&(tempPreyStore.size()!=0)){
                //Stores the sample of the prey population in the "samplePreyData" vector
                samplePreyData.push_back(make_tuple(tempPreyStore,(float)glfwGetTime()));
                saveRate=glfwGetTime()+3;
            }
            save=false;

            camera.UpdateMousePos(window.getMousePosX(),window.getMousePosY());
            window.swapBuffers();
            
        }
        if(samplePreyData.size()!=0){
            storePreySamplesIntoTextFile(samplePreyData,simulationName);
        }
        
        std::cout<<"Prey:"<<preyPopulation.size()<<"\n";
        int count=0;
        //Frees memory
        for(int i=0;i<renderer.objects.size();i++){
            try{
                if(renderer.objects[i]->name=="Predator" || renderer.objects[i]->name=="Prey" || renderer.objects[i]->name=="Food"){
                    delete renderer.objects[i];
                }

            }catch(error_code){
                
            }
        }
        std::cout<<samplePreyData.size();
        main();
        
    }else if(response=="D"){
        int choice=getDataMenuChoice();
        if(choice==1){
            viewSimulationResults();
        }
        
        if(choice==2){
            deleteSimulationData();
        }
    }
    
    window.terminate();
    
    return 0;

};

