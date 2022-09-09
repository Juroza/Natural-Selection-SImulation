//
//  mesh.h
//  Natural Selection Simulation
//
//  Created by Jamaine Scarlett on 23/07/2021.
//

#ifndef mesh_h
#define mesh_h
#include <iostream>
#define GLEW_STATIC
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <vector>
#include <array>
#include "../SOIL2/SOIL2.h"

class Mesh{
    int vertexSize,colourStride,normalStride,width,height,numOfVertices;
    const char* texturePath;
    std::vector<GLuint> textureList;
    GLuint singleTexture;
public:
    GLuint VAO,VBO,EBO;
    long a;
    float* vertices;
    int* indices;
    int size=0,indicesSize=0,textureStrider;
    int indicesLength;
    Mesh(std::vector<float>v,std::vector<int>ind, int sizeOfVertex,int stride1temp,int normalStridetemp,const char*path){
        colourStride=stride1temp;
        normalStride=normalStridetemp;
        vertexSize=sizeOfVertex;
        indicesLength=(int)ind.size();
        a=v.size();
        vertices=new float[a];
        indices= new int[ind.size()];
        numOfVertices=(int)v.size()/sizeOfVertex;
        //COPY ARRAY CONTENTS INTO MESH
        for(int i=0;i<v.size();i++){
            vertices[i]=v[i];
        }
        for(int i=0;i<ind.size();i++){
            indices[i]=ind[i];
        }
        //GET SIZES OF ARRAYS
        //Used to tell GPU how many bytes in represents the colour, texture coordinate etc
        int c=0;
        for(std::vector<float>::iterator i=v.begin();i!=v.end();++i){
            size= size+4;
            c++;
        
        };
        for(std::vector<int>::iterator i=ind.begin();i!=ind.end();++i){
            indicesSize=indicesSize+4;
            
        }
        
        readyForUse();
        if((strcmp(path, "")!=0)){
            addTexture(path, 6);
        }
    }
    void readyForUse(){
        glGenVertexArrays(1,&VAO);
        glGenBuffers(1,&VBO);
        glGenBuffers(1,&EBO);
        
        glBindVertexArray(VAO);
        glBindBuffer(GL_ARRAY_BUFFER,VBO);
        glBufferData(GL_ARRAY_BUFFER,size,vertices,GL_STATIC_DRAW);
        if(indicesLength>1){
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,EBO);
            glBufferData(GL_ELEMENT_ARRAY_BUFFER,indicesSize,indices,GL_STATIC_DRAW);
        }
        //Passes the vertices to the GPU
        glVertexAttribPointer(0,3,GL_FLOAT,GL_FALSE,vertexSize*sizeof(GL_FLOAT),(void*)0);
        glEnableVertexAttribArray(0);
        //Passes the colour of the cube to the GPU
        if(colourStride!=0){
            glVertexAttribPointer( 1, 3, GL_FLOAT, GL_FALSE,vertexSize * sizeof( float ), ( GLvoid * )( colourStride * sizeof( GL_FLOAT ) ) );
            glEnableVertexAttribArray( 1 );
            
        }
        //Tells the GPU what direcction the light bounces of at
        if(normalStride!=0){
            glVertexAttribPointer( 3, 3, GL_FLOAT, GL_FALSE, vertexSize * sizeof( float ), ( GLvoid * )( normalStride * sizeof( float ) ) );
            glEnableVertexAttribArray( 3 );
            
        }

    }
    void addTexture(const char* path,int textureStride){
        //Loads txture from file path
        GLuint texture;

        if(textureStride!=0){
            textureStrider=textureStride;
            glBindVertexArray(VAO);
            glVertexAttribPointer(2,2,GL_FLOAT,GL_FALSE,vertexSize*sizeof(float),(GLvoid*)(textureStride* sizeof(float)));
            glEnableVertexAttribArray(2);
            glBindVertexArray( 0 );
        }
        glGenTextures(1, &texture);
        glBindTexture(GL_TEXTURE_2D, texture);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
        glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
        unsigned char *image = SOIL_load_image( path, &width, &height, 0, SOIL_LOAD_RGBA );
        glTexImage2D( GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image );
        glGenerateMipmap( GL_TEXTURE_2D );
        SOIL_free_image_data( image );
        glBindTexture( GL_TEXTURE_2D, 0 );
        singleTexture=texture;
    }
    void activateTexture(GLuint program){
        if(textureStrider!=0){
            glActiveTexture(GL_TEXTURE0+0);
            glBindTexture(GL_TEXTURE_2D, singleTexture);
            glUniform1i(glGetUniformLocation(program,"Texture"),0);
        }
    }
    void render(){
        if(indicesLength>1){
            
        glDrawElements(GL_TRIANGLES, indicesLength, GL_UNSIGNED_INT, 0);
        }else{
            glDrawArrays(GL_TRIANGLES, 0, numOfVertices);
        }
    }
    void finishRender(){
        glBindVertexArray(0);
    }

    void deleteInfo(){
        glDeleteVertexArrays(1,&VAO);
        glDeleteBuffers(1,&VBO);
        glDeleteBuffers( 1, &EBO );
    }
};


#endif /* mesh_h */
