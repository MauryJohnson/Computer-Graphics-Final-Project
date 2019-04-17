//#define GLFW_INCLUDE_NONE

//#include "glad.c"

//#include "Shader.h"

#include "include/glad/glad.h"
//#include <GLFW/glfw3.h>
//#include <glad/glad.h>
#include "include/glad.c"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "include/Shader.h"
#include "include/Camera.h"
#include "include/Model.h"

#include "include/FileSystem.h"

#include <iostream>

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void key_callback(GLFWwindow* window, int key, int r, int x,int o);
void processInput(GLFWwindow *window);


//Key
bool Keys[1024];

// settings
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

// camera
Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));
//If turn look any direction, will still always move forward normally
//float StableLook[]={};
bool StableMove = true;
//My Movement Parameters
float MyX = SCR_WIDTH / 2.0f;
float MyY = SCR_HEIGHT / 2.0f;
float MyLookX = 0.0f;
float MyLookY = 0.0f;
float MyZ = 0.0f;



//Store Vertices for each room
std::vector<float> BedRoomVertices;
std::vector<float> LivingRoomVertices;
std::vector<float> BathRoomVertices;

bool firstMouse = true;

// timing
float deltaTime = 0.0f;
float DeltaMultiplier = 10.0f;

float lastFrame = 0.0f;

int main()
{
    // glfw: initialize and configure
    // ------------------------------
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // uncomment this statement to fix compilation on OS X
#endif

    // glfw window creation
    // --------------------
    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "LearnOpenGL", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    //glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);
    glfwSetKeyCallback(window,key_callback);


    // tell GLFW to capture our mouse
    //glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    // glad: load all OpenGL function pointers
    // ---------------------------------------

    //glfwMakeContextCurrent(window);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }
    
    // configure global opengl state
    // -----------------------------
    glEnable(GL_DEPTH_TEST);

    // build and compile shaders
    // -------------------------
    Shader ourShader("shader.vs", "shader.frag");

    // load models
    // -----------
    
    //Model ourModel(FileSystem::getPath("./NanoSuit/nanosuit.obj"));
    
    Model ourModel(FileSystem::getPath("../BedRoom/EmptyRoom.obj"),"../BedRoom");
    Model Bed(FileSystem::getPath("../BedRoom/Bed.obj"),"../BedRoom");
     
    //Model LBD(FileSystem::getPath("../BedRoom/RightBedDresser.obj"),"../BedRoom");

    Model Furniture(FileSystem::getPath("../BedRoom/Furniture.obj"),"../BedRoom");

    Model MoreItems(FileSystem::getPath("../BedRoom/MoreItems.obj"),"../BedRoom");
    
    Model BloodyAxe(FileSystem::getPath("../BedRoom/BloodyAxe.obj"),"../BedRoom");

    Model DoorToBathRoom(FileSystem::getPath("../BedRoom/DoorToBathRoom.obj"),"../BedRoom");

    Model DoorToLivingRoom(FileSystem::getPath("../BedRoom/DoorToLivingRoom.obj"),"../BedRoom");

    Model Phone(FileSystem::getPath("../BedRoom/Phone.obj"),"../BedRoom");

    Model RightShoe(FileSystem::getPath("../BedRoom/RightShoe.obj"),"../BedRoom");
    Model Step(FileSystem::getPath("../BedRoom/Step.obj"),"../BedRoom");
	 
    Model TvGun(FileSystem::getPath("../BedRoom/TvGun.obj"),"../BedRoom");
    
    //Model Fan(FileSystem::getPath("../BedRoom/fan.obj"),"../BedRoom");
    //Model Phone2(FileSystem::getPath("../BedRoom/Phone.obj"),"../BedRoom");

    //*/

    //Model Room
 
    // draw in wireframe
    //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    
    // render loop
    
    // -----------
    
    while (!glfwWindowShouldClose(window))
    {
        // per-frame time logic
        // --------------------
        float currentFrame = glfwGetTime();
        
        deltaTime = (currentFrame - lastFrame) * DeltaMultiplier;
        
        lastFrame = currentFrame;

        // input
        // -----
        //processInput(window);

        // render
        // ------
        glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // don't forget to enable shader before setting uniforms
        ourShader.use();

        // view/projection transformations
        glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
        glm::mat4 view = camera.GetViewMatrix();
        ourShader.setMat4("projection", projection);
        ourShader.setMat4("view", view);

        // render the loaded model
        glm::mat4 model = glm::mat4(1.0f);
        //model = glm::translate(model, glm::vec3(0.0f, 0.0f, 1.0f)); // translate it down so it's at the center of the scene
        //model = glm::scale(model, glm::vec3(0.2f, 0.2f, 0.2f));	// it's a bit too big for our scene, so scale it down
        ourShader.setMat4("model", model);

        // STATIC ITEMS
        ourModel.Draw(ourShader);

  	Bed.Draw(ourShader);

        Furniture.Draw(ourShader);
        //LBD.Draw(ourShader);

  	//RBD.Draw(ourShader);

        MoreItems.Draw(ourShader);
 	//END STATIC ITEMS

 	//EVIDENCE
	
	//BEDROOM EVIDENCE
	DoorToBathRoom.Draw(ourShader);
 	DoorToLivingRoom.Draw(ourShader);
	
	Phone.Draw(ourShader);
	
	RightShoe.Draw(ourShader);

	Step.Draw(ourShader);

        BloodyAxe.Draw(ourShader);

	TvGun.Draw(ourShader);

	//END BEDROOM EVIDENCE
	
	//END ALL EVIDENCE

        //ourModel.Draw(ourShader);

        // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
        // -------------------------------------------------------------------------------

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // glfw: terminate, clearing all previously allocated GLFW resources.
    // ------------------------------------------------------------------
    glfwTerminate();
    return 0;
}

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
/*
void processInput(GLFWwindow *window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        camera.ProcessKeyboard(FORWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        camera.ProcessKeyboard(BACKWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        camera.ProcessKeyboard(LEFT, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        camera.ProcessKeyboard(RIGHT, deltaTime);
}
*/

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    // make sure the viewport matches the new window dimensions; note that width and 
    // height will be significantly larger than specified on retina displays.
    glViewport(0, 0, width, height);
}

void key_callback(GLFWwindow* window,int key,int scancode,int action,int mode)
{
    if(key==GLFW_KEY_ESCAPE && action==GLFW_PRESS)
        glfwSetWindowShouldClose(window,GL_TRUE);

    //camera.ProcessKeyboard(FORWARD, deltaTime);
    if(key==GLFW_KEY_Q &&(action==GLFW_PRESS || Keys[key]))
    {	
	Keys[key]=true;
    	//printf("\n STOP MOVING!!");
	return;
    }
    else if(key==GLFW_KEY_Q)
	Keys[key]=false;

    //Check if you set parameter for special movement
    //This will always reset view look to 0 by backtracking
    //And then will set view look back to orig again!
    if(StableMove){
	camera.ProcessMouseMovement(0.0f,-MyLookX);
    }

    if(key==GLFW_KEY_UP && (action==GLFW_PRESS || Keys[key]))
    {
      Keys[key]=true;
      //printf("Pressed Up\n"); 
      camera.ProcessKeyboard(FORWARD,deltaTime);
      MyZ-=1;
    }
    else if(key==GLFW_KEY_UP){
      Keys[key]=false;
      //printf("End Press Up\n");
    }

    if(key==GLFW_KEY_DOWN && (action==GLFW_PRESS||Keys[key]))
    {
      Keys[key]=true;     
      //printf("Pressed Down\n");
      camera.ProcessKeyboard(BACKWARD,deltaTime);
      MyZ+=1;
    }
    else if(key==GLFW_KEY_DOWN){
      Keys[key]=false;
      //printf("End Press Down\n");
    }

    if(key==GLFW_KEY_LEFT && (action==GLFW_PRESS || Keys[key]))
    {
      Keys[key]=true;
      //printf("Pressed Left\n");
      camera.ProcessKeyboard(LEFT,deltaTime);
      MyX-=1;
    }
    else if(key==GLFW_KEY_LEFT){
      Keys[key]=false;
      //printf("End Press Left\n");
    }

    if(key==GLFW_KEY_RIGHT && (action==GLFW_PRESS||Keys[key]))
    {
      Keys[key]=true;
      //printf("Pressed Right\n");
      camera.ProcessKeyboard(RIGHT,deltaTime);
      MyX+=1;
    }
    else if(key==GLFW_KEY_RIGHT){
      Keys[key]=false;
      //printf("End Press Right\n");
    }

    if(key==GLFW_KEY_W && (action==GLFW_PRESS||Keys[key]))
    {
      Keys[key]=true;
      //printf("Pressed LOOK UP\n");
      //camera.ProcessKeyboard(RIGHT,deltaTime);
      
      camera.ProcessMouseMovement(0.0f,1.0f); 

      MyLookX+=1;
    }
    else if(key==GLFW_KEY_W){
      Keys[key]=false;
      //printf("End Press LOOK UP\n");
    }

    if(key==GLFW_KEY_S && (action==GLFW_PRESS||Keys[key]))
    {
      Keys[key]=true;
      //printf("Pressed LOOK DOWN\n");
      //camera.ProcessKeyboard(RIGHT,deltaTime);

      camera.ProcessMouseMovement(0.0f,-1.0f);

      MyLookX-=1;
    }
    else if(key==GLFW_KEY_S){
      Keys[key]=false;
      //printf("End Press LOOK DOWN\n");
    }

    if(key==GLFW_KEY_D && (action==GLFW_PRESS||Keys[key]))
    {
      Keys[key]=true;
      //printf("Pressed LOOK RIGHT\n");
      //camera.ProcessKeyboard(RIGHT,deltaTime);

      camera.ProcessMouseMovement(1.0f,0.0f);

      MyLookY+=1;
    }
    else if(key==GLFW_KEY_D){
      Keys[key]=false;
      //printf("End Press LOOK RIGHT\n");
    }

    if(key==GLFW_KEY_A && (action==GLFW_PRESS||Keys[key]))
    {
      Keys[key]=true;
      //printf("Pressed LOOK LEFT\n");
      //camera.ProcessKeyboard(RIGHT,deltaTime);

      camera.ProcessMouseMovement(-1.0f,0.0f);

      MyLookY-=1;
    }
    else if(key==GLFW_KEY_A){
      Keys[key]=false;
      //printf("End Press LOOK LEFT\n");
    }


    if(StableMove){
        camera.ProcessMouseMovement(0.0f,MyLookX);
    }

}


// glfw: whenever the mouse moves, this callback is called
// -------------------------------------------------------
/*void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
    if (firstMouse)
    {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos; // reversed since y-coordinates go from bottom to top

    lastX = xpos;
    lastY = ypos;

    camera.ProcessMouseMovement(xoffset, yoffset);
}
*/

// glfw: whenever the mouse scroll wheel scrolls, this callback is called
// ----------------------------------------------------------------------
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    //camera.ProcessMouseScroll(yoffset);
    if(yoffset>0)
    camera.ProcessKeyboard(UP,deltaTime);
    else
    camera.ProcessKeyboard(DOWN,deltaTime);
}

