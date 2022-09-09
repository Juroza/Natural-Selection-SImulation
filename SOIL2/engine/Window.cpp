//
//  Window.cpp
//  Natural Selection Simulation
//
//  Created by Jamaine Scarlett on 19/07/2021.
//

#include <iostream>
#define GLEW_STATIC
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include<string>
using namespace std;
class Window{
    int width,height,frame=0;
    // BACKGROUND COLOUR
    float backgroundr,backgroundg,backgroundb;
    double time;
    
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
        glfwWindowHint( GLFW_CONTEXT_VERSION_MAJOR, 3 );
        glfwWindowHint( GLFW_CONTEXT_VERSION_MINOR, 3 );
        glfwWindowHint( GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE );
        glfwWindowHint( GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE );
        glfwWindowHint( GLFW_RESIZABLE, GL_FALSE );
        GLFWwindow* window=glfwCreateWindow(width,height,title,nullptr, nullptr);
        glfwSwapInterval(1);
       // GLFWwindow* window=glfwCreateWindow(width,height,"testing",nullptr,nullptr);
        if ( nullptr == window )
        {
            std::cout << "Failed to create GLFW window" << std::endl;
            glfwTerminate( );
            
            //return EXIT_FAILURE;
        }
        int screenWidth, screenHeight;
        glfwGetFramebufferSize( window, &screenWidth, &screenHeight );
        
        
        glfwMakeContextCurrent( window );
        glewExperimental=GL_TRUE;
        if ( GLEW_OK != glewInit( ) )
        {
            std::cout << "Failed to initialize GLEW" << std::endl;
            //return EXIT_FAILURE;
        }
        
        // Define the viewport dimensions
        glViewport( 0, 0, screenWidth, screenHeight );
        time=glfwGetTime();
        return window;
        
    }
    void setBackgroundColour(float r,float g,float b){
        backgroundr=r;
        backgroundg=g;
        backgroundb=b;
    }
    void getFPS(){
        frame=frame+1;
        if(glfwGetTime()>=time+1){
          //  cout<<"\n"<<frame;
            string a =" FPS:"+to_string(frame);
            string c= title+a;
            const char *C = c.c_str();
            glfwSetWindowTitle(window,C);
            frame=0;
            time=glfwGetTime();
        }
    }
    
    void input(){
        if(glfwGetKey(window, GLFW_KEY_ESCAPE)==GLFW_PRESS){
            glfwSetWindowShouldClose(window, true);
        }
    }
    bool shouldClose(){
        return glfwWindowShouldClose(window);
    }
    void update(){
        glfwPollEvents();
        glClearColor(backgroundr, backgroundg, backgroundb, 0.0f);
        glClear(GL_COLOR_BUFFER_BIT);
    }
    void swapBuffers(){
        glfwSwapBuffers(window);
    }
    void terminate(){
        glfwTerminate();
    }
};
