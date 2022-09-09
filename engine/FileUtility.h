//
//  FileUtility.h
//  Natural Selection Simulation
//
//  Created by Jamaine Scarlett on 23/07/2021.
//

#ifndef FileUtility_h
#define FileUtility_h
#include <string>
#include <fstream>
#include <sstream>
#include <GL/glew.h>
#include <iostream>
std::string readFile(const char* path){
    std::ifstream file;
    std::string readData;
    file.exceptions(std::ifstream::badbit);
    try {
        file.open(path);
        std::stringstream fileStream;
        fileStream<<file.rdbuf();
        file.close();
        readData=fileStream.str();
    } catch (std::ifstream::failure e ) {
        std::cout << "ERROR::File Could Not Be Read!" << std::endl;
    }
    return readData;
    
    
}
const GLchar* readShaderFile(const GLchar *path){
    std::ifstream file;
    std::string readData;
    std::cout<<readData;
    file.exceptions(std::ifstream::badbit);
    try {
        file.open(path);
        std::stringstream fileStream;
        fileStream<<file.rdbuf();
        file.close();
        readData=fileStream.str();
        //std::cout<<readData;
    } catch (std::ifstream::failure e ) {
        std::cout << "ERROR::File Could Not Be Read!" << std::endl;
    }
    const GLchar *kaishi= readData.c_str();
   // std::cout<<kaishi;
    return kaishi;
    
}



#endif /* FileUtility_h */
