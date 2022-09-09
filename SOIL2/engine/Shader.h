//
//  Shader.h
//  Natural Selection Simulation
//
//  Created by Jamaine Scarlett on 23/07/2021.
//

#ifndef Shader_h
#define Shader_h
#include <fstream>
#include <sstream>
#include <iostream>
#include "FileUtility.h"
#define GLEW_STATIC
#include <GL/glew.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <cstdlib>
#include <unordered_map>
#include <vector>
#include <map>
#include <string>
#include <filesystem>
#include <mach-o/dyld.h>

class Shader{
    std::unordered_map<const char*, GLint> UniformCache;
    GLuint vertex,fragment;
    GLint success;
    GLchar infoLog[512];
    void createVertexShader(const GLchar* vshaderCode){
        vertex=glCreateShader(GL_VERTEX_SHADER);
        glShaderSource(vertex,1,&vshaderCode,NULL);
        glCompileShader(vertex);
        glGetShaderiv(vertex,GL_COMPILE_STATUS,&success);
        if(!success){
            glGetShaderInfoLog(vertex, 512, NULL, infoLog);
            std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED :)\n" << infoLog << std::endl;
        }
        
    };
    void createFragmentShader(const GLchar* fShaderCode){
        fragment=glCreateShader(GL_FRAGMENT_SHADER);
        glShaderSource(fragment,1,&fShaderCode,NULL);
        glCompileShader(fragment);
        glGetShaderiv(fragment,GL_COMPILE_STATUS,&success);
        if(!success){
            glGetShaderInfoLog(fragment, 512, NULL, infoLog);
            std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;
        }
    };
    void createShaderProgram(){
        Program=glCreateProgram();
        glAttachShader(this->Program,vertex);
        glAttachShader(this->Program,fragment);
        glLinkProgram(this->Program);
        glGetProgramiv( this->Program, GL_LINK_STATUS, &success );
        if (!success)
        {
            glGetProgramInfoLog( this->Program, 512, NULL, infoLog );
            std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
        }
    }
    void deleteShaders(){
        glDeleteShader(vertex);
        glDeleteShader(fragment);
    }
    //This function stores the locations of the uniforms in an unordered map, meaning that each time a value such as position
    //changes it does not need to find the location again.
    GLint getUniformLocation(const char* name){
        if(UniformCache.find(name)!=UniformCache.end()){
            return UniformCache[name];
        }
        GLint location= glGetUniformLocation(Program,name);
        UniformCache[name]=location;
        std::cout<<"LOADING\n";
        return location;
    }

public:
 
    int ids=10l;
    GLuint Program;
    const GLchar* vShaderCode;
    Shader( const GLchar *vertexPath, const GLchar *fragmentPath )
    {
        std::string vertexCode;
        std::string fragmentCode;
        std::ifstream vShaderFile;
        std::ifstream fShaderFile;
            // ensure ifstream objects can throw exceptions:
        vShaderFile.exceptions (std::ifstream::failbit | std::ifstream::badbit);
        fShaderFile.exceptions (std::ifstream::failbit | std::ifstream::badbit);
        try{
                // open files
            vShaderFile.open(vertexPath);
            fShaderFile.open(fragmentPath);
            std::stringstream vShaderStream, fShaderStream;
                // read file's buffer contents into streams
            vShaderStream << vShaderFile.rdbuf();
            fShaderStream << fShaderFile.rdbuf();
                // close file handlers
            vShaderFile.close();
            fShaderFile.close();
                // convert stream into string
            vertexCode   = vShaderStream.str();
            fragmentCode = fShaderStream.str();
        }catch(std::ifstream::failure e)
        {
            std::cout << "ERROR::SHADER::FILE_NOT_SUCCESFULLY_READ" << std::endl;
        }
        const char* vShaderCode = vertexCode.c_str();
        const char* fShaderCode = fragmentCode.c_str();
        createVertexShader(vShaderCode);
        createFragmentShader(fShaderCode);
        createShaderProgram();
        deleteShaders();
    };
    void use(){
        glUseProgram(this->Program);
    }
    void terminate(){
        glUseProgram(0);
    }
    //Functions that pass information such as colour, position, size to the GPU
    void passVec3(glm::vec3 vector,const char* name){
        GLint vectorLoc=getUniformLocation(name);
        glUniform3f( vectorLoc,vector.x,vector.y,vector.z);
    }
    void passMat4(glm::mat4 matrix,const char* name){
        GLint matrixLoc = getUniformLocation(name);
        glUniformMatrix4fv(matrixLoc,1,GL_FALSE,glm::value_ptr( matrix ));
    }
    void passVec4(glm::vec4 vector,const char* name){
        GLint vectorLoc = getUniformLocation(name);
        glUniform4f(vectorLoc,vector.x,vector.y,vector.z,vector.w);
    }
    void passFloat(float number,const char*name){
        GLint numLoc= getUniformLocation(name);
        glUniform1f(numLoc,number);
    }
    void passInt(int num,const char* name){
        GLint numLoc= getUniformLocation(name);
        glUniform1i(numLoc,num);
    }

};
#endif /* Shader_h */
