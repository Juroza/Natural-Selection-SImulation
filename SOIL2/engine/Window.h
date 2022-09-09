//
//  Window.h
//  Natural Selection Simulation
//
//  Created by Jamaine Scarlett on 23/07/2021.
//

#ifndef Window_h
#define Window_h
#include <iostream>
#define GLEW_STATIC
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include<string>
using namespace std;
double mousePX,mousePY;
void mouseCallback(GLFWwindow*window,double xpos,double ypos){
    mousePX=xpos;
    mousePY=ypos;
    
};

class Window{
    bool firstMouse;
    double mousePosX,mousePosY,lastXPos,lastYPos;
    int width,height,frame=0;
    // BACKGROUND COLOUR
    float backgroundr,backgroundg,backgroundb,currentFrame,lastFrame=0.0f,deltaTime=0.0f;
    int a;
    double time,checkTime;
    int screenWidth, screenHeight;
    GLFWwindow* window;
    const char*title;
public:
    Window(int x,int y,const char* t){
        width=x;
        height=y;
        title=t;
        window=init();
    };
    GLFWwindow* init(){
        glfwInit();
        //Setup OpenGl Conditions, OpenGl functions like a state machine so state must be set
        //OpenGL version
        glfwWindowHint( GLFW_CONTEXT_VERSION_MAJOR, 3 );
        glfwWindowHint( GLFW_CONTEXT_VERSION_MINOR, 3 );
        glfwWindowHint( GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE );
        glfwWindowHint( GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE );
        //Allows window to be freely resized
        glfwWindowHint( GLFW_RESIZABLE, GL_TRUE );
        GLFWwindow* window=glfwCreateWindow(width,height,title,nullptr, nullptr);
        //prevents screen tearing, tells GPU how often to update data
        glfwSwapInterval(1);
        std::cout<<"HERE!!";

        
        if ( nullptr == window )
        {
            std::cout << "Failed to create GLFW window" << std::endl;
            glfwTerminate( );
            
            //Stops if there was an issue
        }
        //Sets the size of the window
        glfwGetFramebufferSize( window, &screenWidth, &screenHeight );
        //Disables the curosor
        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
        //Sets the function that will be used to process the mouse input
        glfwSetCursorPosCallback(window,mouseCallback);
        
        
        glfwMakeContextCurrent( window );

        glewExperimental=GL_TRUE;
        if ( GLEW_OK != glewInit( ) )
        {
            std::cout << "Failed to initialize GLEW" << std::endl;
            //return EXIT_FAILURE;
        }
        
        // Define the viewport dimensions
        glViewport( 0, 0, screenWidth, screenHeight );
        glEnable(GL_BLEND);
        glEnable(GL_DEPTH_TEST);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        time=glfwGetTime();
        return window;
        
    }
    void setBackgroundColour(float r,float g,float b){
        backgroundr=r;
        backgroundg=g;
        backgroundb=b;
    }
    float getMousePosX(){
        float offset=mousePosX-lastXPos;
        lastXPos=mousePosX;
        return offset;
    }
    float getMousePosY(){
        float offset=lastYPos-mousePosY;
        lastYPos=mousePosY;
        return offset;
    }
    int getScreenWidth(){
        return screenWidth;
    }
    float getDeltaTime(){
        return deltaTime;
    }
    int getScreenHeight(){
        return screenHeight;
    }
    int getFPS(){
        frame=frame+1;
        if(glfwGetTime()>=time+1){
            cout<<"\n"<<frame;
            string a =" FPS:"+to_string(frame);
            string c= title+a;
            const char *C = c.c_str();
            glfwSetWindowTitle(window,C);
            frame=0;
            time=glfwGetTime();
        }
        return frame;
    }
    void changeWindowTitle(const char* enter){
        title=enter;
    }
    void turnCursorOn(){
        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
    }
    void turnCursorOff(){
        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    }
    void input(){
        if(glfwGetKey(window, GLFW_KEY_ESCAPE)==GLFW_PRESS){
            turnCursorOn();
            glfwSetWindowShouldClose(window, true);
            
        }
    }
    void close(){
        glfwSetWindowShouldClose(window, true);
        turnCursorOn();
    }
    void open(){
        glfwSetWindowShouldClose(window, false);
        turnCursorOn();
    }
    GLFWwindow* getWindow(){
        return window;
    }
    double getTime(){
        return glfwGetTime();
    }
    bool shouldClose(){
        return glfwWindowShouldClose(window);
    }
    void update(){
        glfwPollEvents();
        glClearColor(backgroundr, backgroundg, backgroundb, 0.0f);
        glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
        if(firstMouse){
            lastXPos=mousePosX;
            lastYPos=mousePosY;
            firstMouse=false;
        }
        mousePosX=mousePX;
        mousePosY=mousePY;
        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;
    }
    
    void swapBuffers(){
        glfwSwapBuffers(window);
    }
    
    void terminate(){
        glfwTerminate();
    }
    void printToTerminal(string x){
        std::cout<<x;
    }
};

#endif /* Window_h */
