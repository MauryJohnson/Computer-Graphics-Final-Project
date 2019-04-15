#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include "SOIL.h"
#include <vector>

#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/ext.hpp>

#include "Shader.h"

GLuint VAO[15],VBO[15];

bool keys[1024];
GLfloat delta_time=0.0f;                // time between current frame and last frame
GLfloat last_frame=0.0f;                // time of last frame
GLfloat last_X=0.0f,last_Y=0.0f;
GLfloat fov=45.0f;
GLfloat yaw=-90.0f;                     // yaw is initialized to -90.0 degrees since a yaw of 0.0 results in a direction vector pointing
                                        // to the right (due to how Euler angles work) so we initially rotate a bit to the left.
GLfloat pitch=0.0f;
bool first_mouse=true;

glm::vec3 camera_position=glm::vec3(0.0f,0.0f,7.0f);
glm::vec3 camera_front=glm::vec3(0.0f,0.0f,-1.0f);
glm::vec3 camera_up=glm::vec3(0.0f,1.0f,0.0f);

GLfloat ROT = 0;

#define PI 3.14159

/**
Create cube with center pointe at X,Y,Z coordinate
*/
std::vector<GLfloat> CreateCube(GLfloat X, GLfloat Y, GLfloat Z, GLfloat L){
std::vector<GLfloat> Cube;

Cube.reserve(48);

//L/2, L/2, sqrt(L/2^2 + L/2^2)
//Create 8 points

GLfloat Plane1 = Y+L;


GLfloat P1[] = {
X-L/2,Y+L/2,Z-L/2,
X+L/2,Y+L/2,Z-L/2,
X+L/2,Y+L/2,Z+L/2,
X-L/2,Y+L/2,Z+L/2
};

GLfloat P2[] = {
X-L/2,Y-L/1.25,Z-L/2,
X+L/2,Y-L/1.25,Z-L/2,
X+L/2,Y-L/1.25,Z+L/2,
X-L/2,Y-L/1.25,Z+L/2
};

//for(int i=0; i<4; i+=1){
//Cube.push_back(P1[i]);
//Cube.push_back(P2[i]);
//}

//for(int i=0; i<4;i+=1)
//Cube.push_back(P2[i]);

//Front Face
Cube.push_back(P1[0]);
Cube.push_back(P1[1]);
Cube.push_back(P1[2]);

Cube.push_back(P1[3]);
Cube.push_back(P1[4]);
Cube.push_back(P1[5]);

Cube.push_back(P2[0]);
Cube.push_back(P2[1]);
Cube.push_back(P2[2]);

Cube.push_back(P2[3]);
Cube.push_back(P2[4]);
Cube.push_back(P2[5]);


//Right Face
Cube.push_back(P1[3]);
Cube.push_back(P1[4]);
Cube.push_back(P1[5]);

Cube.push_back(P1[6]);
Cube.push_back(P1[7]);
Cube.push_back(P1[8]);

Cube.push_back(P2[3]);
Cube.push_back(P2[4]);
Cube.push_back(P2[5]);

Cube.push_back(P2[6]);
Cube.push_back(P2[7]);
Cube.push_back(P2[8]);

//Back Face
Cube.push_back(P1[6]);
Cube.push_back(P1[7]);
Cube.push_back(P1[8]);

Cube.push_back(P1[9]);
Cube.push_back(P1[10]);
Cube.push_back(P1[11]);

Cube.push_back(P2[6]);
Cube.push_back(P2[7]);
Cube.push_back(P2[8]);

Cube.push_back(P2[9]);
Cube.push_back(P2[10]);
Cube.push_back(P2[11]);

//Left Face
Cube.push_back(P1[9]);
Cube.push_back(P1[10]);
Cube.push_back(P1[11]);

Cube.push_back(P1[0]);
Cube.push_back(P1[1]);
Cube.push_back(P1[2]);

Cube.push_back(P2[9]);
Cube.push_back(P2[10]);
Cube.push_back(P2[11]);

Cube.push_back(P2[0]);
Cube.push_back(P2[1]);
Cube.push_back(P2[2]);

//TOP
for(int i=0; i<12;i+=1)
Cube.push_back(P1[i]);

//BOTTOM
for(int i=0; i<12;i+=1)
Cube.push_back(P2[i]);

return Cube;
}

/**
Create circle from bottom up
*/
std::vector<GLfloat> CreateSphere(GLfloat X, GLfloat Y, GLfloat Z, GLfloat Radius,GLfloat StepSize,GLfloat RStepSize){

std::vector<GLfloat> Points;
Points.reserve(1000);

GLfloat y1 = Y;

printf("ITerate from %f to %f",y1,y1+Radius);

//GLfloat R = Radius;
GLfloat R = 0;

//Iterate from y1 to y1+Radius
for(; y1<Y+Radius;y1+=StepSize){

//X and Z PLANE circles
GLfloat x = X;
GLfloat z = Z;
//Iterate from radians 0 to 2pi with its step size..

for(GLfloat r=0; r<2*PI;r+=RStepSize){
	//X
	Points.push_back(x+(GLfloat)R*cos(r));
	//Y
	Points.push_back(y1);
	//Z
	Points.push_back(z+(GLfloat)R*sin(r));
}
//if(Radius-RStepSize>0)
        //Radius+=RStepSize;
if(R+RStepSize<=Radius)
	R+=RStepSize;
//else
//break;
}

printf("ITerate from %f to %f",y1,y1+Radius);

/*
//And then from y1+Radius to y1+2*Radius
for(y1=Y+Radius; y1<Y+2*Radius;y1+=StepSize/2){
//X and Z PLANE CIRCLES
GLfloat x = X;
GLfloat z = Z;
//Iterate from radians 0 to 2pi with its step size..
for(GLfloat r=2*PI; r>0;r-=RStepSize){
	//X
        Points.push_back(x+(GLfloat)R*cos(r));
        //Y
        Points.push_back(y1);
        //Z
        Points.push_back(z+(GLfloat)R*sin(r));
}

if(R-RStepSize>0)
	R-=RStepSize;
//else
//break;

}
*/
printf("\n SPHERE \n");
int i=0;
//for(i=0; i<Points.size();i+=1){

//	printf("%f",Points[i]);
//}

return Points;
}

void key_callback(GLFWwindow* window,int key,int scancode,int action,int mode)
{
    if(action==GLFW_PRESS || action==GLFW_REPEAT) keys[key]=true;
    else if(action==GLFW_RELEASE) keys[key]=false;

    printf("\nKEY PRESSED:%d",key);

    if(key==GLFW_KEY_ESCAPE && action==GLFW_PRESS)
        glfwSetWindowShouldClose(window,GL_TRUE);

}



void do_movement()
{
    // camera controls
    GLfloat camera_speed=5.0f*delta_time;
    if(keys[GLFW_KEY_W]) camera_position+=camera_speed*camera_front;
    if(keys[GLFW_KEY_S]) camera_position-=camera_speed*camera_front;
    if(keys[GLFW_KEY_A]) camera_position-=glm::normalize(glm::cross(camera_front,camera_up))*camera_speed;
    if(keys[GLFW_KEY_D]) camera_position+=glm::normalize(glm::cross(camera_front,camera_up))*camera_speed;
}

void scroll_callback(GLFWwindow *window,double xoffset,double yoffset)
{
    if(fov>=1.0f && fov<=45.0f)  fov-=yoffset;
    if(fov<=1.0f) fov=1.0f;
    if(fov>=45.0f) fov=45.0f;
}

/*
void key_callback(GLFWwindow *window,int key,int scancode,int action,int mode)
{
    if(key==GLFW_KEY_ESCAPE && action==GLFW_PRESS)
        glfwSetWindowShouldClose(window,GL_TRUE);

    if(key==GLFW_KEY_UP && action==GLFW_PRESS)
    {
        mix_value+=0.1f;
        if(mix_value>1.0f) mix_value=1.0f;
    }

    if(key==GLFW_KEY_DOWN && action==GLFW_PRESS)
    {
        mix_value-=0.1f;
        if(mix_value<0.0f) mix_value=0.0f;
    }

    if(action==GLFW_PRESS) keys[key]=true;
    else if(action==GLFW_RELEASE) keys[key]=false;
}
*/
void mouse_callback(GLFWwindow *window,double xpos,double ypos)
{
    if(first_mouse)
    {
        last_X=xpos;
        last_Y=ypos;
        first_mouse=false;
    }

    GLfloat x_offset=xpos-last_X;
    GLfloat y_offset=last_Y-ypos;
    last_X=xpos;last_Y=ypos;

    GLfloat sensitivity=0.05f;
    x_offset*=sensitivity;
    y_offset*=sensitivity;
    yaw+=x_offset;
    pitch+=y_offset;

    if(pitch>89.0f) pitch=89.0f;
    else if(pitch<-89.0f) pitch=-89.0f;

    glm::vec3 front;
    front.x=cos(glm::radians(pitch))*cos(glm::radians(yaw));
    front.y=sin(glm::radians(pitch));
    front.z=cos(glm::radians(pitch))*sin(glm::radians(yaw));
    camera_front=glm::normalize(front);
}

GLuint Validate(Shader VFShader,glm::mat4 view, glm::mat4 projection){

GLuint view_location1=glGetUniformLocation(VFShader.program,"view");
        glUniformMatrix4fv(view_location1,1,GL_FALSE,glm::value_ptr(view));
        GLuint projection_location1=glGetUniformLocation(VFShader.program,"projection");

        glUniformMatrix4fv(projection_location1,1,GL_FALSE,glm::value_ptr(projection));

        GLuint model_location1=glGetUniformLocation(VFShader.program,"model");

return model_location1;

}

void SetUp3Array(GLfloat* P,int IDX,int DIM){


    // bind vertex array object
    glBindVertexArray(/* TO CHANGE  */VAO[IDX]);

    // copy the vertices in a buffer
    glBindBuffer(GL_ARRAY_BUFFER,/* TO CHANGE  */VBO[IDX]);
    glBufferData(GL_ARRAY_BUFFER,/* TO CHANGE  */sizeof(GLfloat),/* TO CHANGE  */P,GL_STATIC_DRAW);

    // set position attribute pointers
    glVertexAttribPointer(0,DIM,GL_FLOAT,GL_FALSE,/* TO CHANGE  */DIM*sizeof(GL_FLOAT),(GLvoid*)0);

    glEnableVertexAttribArray(0);

    // unbind the vertex array object
    glBindVertexArray(0);


}

int main()
{

    GLfloat HX = 0.0f;
    GLfloat HZ = 0.0f;
    //std::vector<GLfloat> Head = CreateSphere(0.0f,0.75f,0.0f,0.5f,0.01f,PI/128);
    std::vector<GLfloat> Head = CreateCube(0.0f,1.0f,-0.125f,0.5f);

    std::vector<GLfloat> Eye1 = CreateCube(-0.1f,1.15f,0.1f,0.1f);

    glm::vec3 RightEye_Position(-0.1f,1.15f,0.0f);

    std::vector<GLfloat> Eye2 = CreateCube(0.1f,1.15f,0.1f,0.1f);

    glm::vec3 LeftEye_Position(0.1f,1.15f,0.0f);

    glm::vec3 Head_Position(0.0f,0.15f,0.0f);


    glfwInit();
#if __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT,GL_TRUE);
#endif
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR,3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR,3);
    glfwWindowHint(GLFW_OPENGL_PROFILE,GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_RESIZABLE,GL_FALSE);

    GLFWwindow *window=glfwCreateWindow(800,600,"Learn OpenGL",nullptr,nullptr);
    if(window==nullptr)
    {
        std::cout<<"Failed to create GLFW window!"<<std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);

    glewExperimental=GL_TRUE;
    if(glewInit()!=GLEW_OK)
    {
        std::cout<<"Failed to initialize GLEW!"<<std::endl;
        return -1;
    }

    int width,height;
    glfwGetFramebufferSize(window,&width,&height);
    glViewport(0,0,width,height);

    // projection matrix
    glm::mat4 projection(1.0f);
    projection=glm::perspective(glm::radians(45.0f),(float)width/height,0.1f,100.0f);

    glfwSetKeyCallback(window,key_callback);
    //glfwSetCursorPosCallback(window,mouse_callback);
    //glfwSetScrollCallback(window,scroll_callback);

    //Shader our_shader("shader.vs","tshader.frag");
    Shader VFShader("shader.vs","shader.frag");
    //Shader VFShader2("shader.vs","shader2.frag");
    //Shader VFShader3("shader.vs","shader3.frag");
    GLuint texture1,texture2;

    // view space transform
    glm::mat4 view(1.0f);
    
    //Model Space
    //glm::mat4 model(1.0f);

    // note that we're translating the scene in the reverse direction of where we want to move
    view=glm::translate(view,glm::vec3(0.0f,0.0f,-100.0f));
    GLfloat R = PI/4;
    ///glm::mat4 model = glm::rotate(glm::mat4(1.0f),R,glm::vec3(0.0,1.0,0.0));	

    glEnable(GL_DEPTH_TEST);

    //Shader VFShader("transform.vs","shader.frag");

    //CreateSphere(0.0f,0.0f,0.0f,2.0f,0.1f,PI/12);

    while(!glfwWindowShouldClose(window))
    {
        glfwPollEvents();
        glClearColor(.2f,.3f,.3f,1.f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	GLfloat current_frame=glfwGetTime();
        delta_time=current_frame-last_frame;
        last_frame=current_frame;

	//Update View
	view=glm::lookAt(camera_position,camera_position+camera_front,camera_up);

	//Update Projection
        projection=glm::perspective(glm::radians(fov),(float)width/height,0.1f,100.0f);

        //glBindVertexArray(0);
	//glBindVertexArrat(VAO[1]);

        glfwSwapBuffers(window);
    }

    // deallocate all resources
    //Head.clear();
    //Eye1.clear();
    //Eye2.clear();
    //LArmElbow.clear();
    //RArmElbow.clear();
    //LLegKnee.clear();
    //RLegKnee.clear();
    glDeleteVertexArrays(15,VAO);
    glDeleteBuffers(15,VBO);
    // terminate GLFW
    glfwTerminate();

    return 0;
}
