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

#include <math.h>

#include "include/Shader.h"
#include "include/Camera.h"
#include "include/Model.h"

#include "include/FileSystem.h"

#include <iostream>

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void mouse_button_callback(GLFWwindow* window, int button, int action, int mods);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void key_callback(GLFWwindow* window, int key, int r, int x,int o);
void processInput(GLFWwindow *window);


//Key
bool Keys[1024];

// settings
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

// camera
Camera camera(glm::vec3(0.0f, 0.0f, 0.0f));
//If turn look any direction, will still always move forward normally
//float StableLook[]={};
bool StableMove = true;
//My Movement Parameters
float MyX = SCR_WIDTH / 2.0f;
float MyY = SCR_HEIGHT / 2.0f;
float MyLookX = 0.0f;
float MyLookY = 0.0f;
float MyZ = 0.0f;

float DetectionDistance = 1.0f;

//Stores info about each evidence
class Evidence{
public:
	float* Vertices;
	bool Found = false;
	const char* Name;
	Model** M;
	Evidence(const char* Name,Model** Model,float* EvidenceVertices);
	~Evidence();
	void NoMore();
};

Evidence::Evidence(const char*N,Model** Model,float*V){
this->Name=N;
this->Vertices=V;
this->M=Model;
}

void Evidence::NoMore(){
*(this->M)=NULL;
}

Evidence::~Evidence(){

//Goodbye Evidence!!
printf("\n GoodBye Evidence:%s @ (%f,%f,%f)",this->Name,Vertices[0],Vertices[1],Vertices[2]);

//*(this->M) = NULL;

}

//Stores info about each room

class Room{
public:
	//Room dimensions
	float * Vertices;

	//ALL evidence positions
	//float * EvidenceVertices;
	std::vector<Evidence> Evidences;

	//Determines if the space in room is occupied or not
	bool Occupied = false;

	//Create room Obj
	Room(float* Vertices,std::vector<Evidence> Evidences);
		
	//Destructor
	~Room();


};

Room::Room(float*Vertices,std::vector<Evidence>EvidenceVertices){
this->Vertices=Vertices;
this->Evidences=EvidenceVertices;
}

Room::~Room(){

printf("\n NO ROOM");

}

std::vector<Room> Rooms;

//Store Vertices for each room to then check if within a room
//For rendering efficiency and bounds checking
//Any point must be >= all lower plane values and <= all upper plane values
//Store in order of Lower plane (4 vertices = 12 floats)
//Then Upper plane (remaining 12 float values)
//In format x1,y1,z1,x2,y2,z2,....,z24
//std::vector<float> BedRoomVertices;
bool InBedRoom=true;
float BedRoomVertices[]={
//BOTTOM FACE
6.69,-6.31,-0.1,
6.69,6.25,-0.1,
-6.69,-6.31,-0.1,
-6.69,6.25,-0.1,
//END BOTTOM FACE
//TOP FACE
-6.63,-6.31,7.22,
6.69,-6.31,7.22,
6.69,6.25,7.22,
-6.63,6.25,7.22
//END TOP FACE
};
float BedRoomEvidenceVertices[]={
//1

//2

//3

//4


};
bool InLivingRoom=false;
std::vector<float> LivingRoomVertices;
bool InBathRoom=false;
std::vector<float> BathRoomVertices;

//All Rooms Stored
std::vector<float*> AllRooms;

bool firstMouse = true;

// timing
float deltaTime = 0.0f;
float DeltaMultiplier = 10.0f;

float lastFrame = 0.0f;


//My Position will use camera's position
bool WithinBounds(glm::vec3 Position){
printf("\n Camera Position:%f,%f,%f",camera.Position[0],camera.Position[1],camera.Position[2]);

//Check if either in bed room, living room, or bath room
//Check for all rooms
//IF in NEITHER ROOMS, return false!
//Otherwise Return True
/*if(MyPosition.size()!=3){
printf("\n Invalid Bound Vector");
return false;
}
*/

//float * Position = MyPosition.data();

bool Within = true;

for(int i=0;i<Rooms.size();i+=1){
printf("\nRoom:%d",i);
for(int j=0; j<=23;j+=3){

printf("\nData:%f,%f,%f",Rooms[i].Vertices[j],Rooms[i].Vertices[j+1],Rooms[i].Vertices[j+2]);

float* Data = Rooms[i].Vertices;

//Checking if either above a plane or below a plane, because ether you are there, or you arent!
if(

((
//ggg
Position[0]>=Data[j] && Position[1]>=Data[j+1] && Position[2]>=Data[j+2] 
||
//llg
Position[0]<=Data[j] && Position[1]<=Data[j+1] && Position[2]>=Data[j+2]
||
//glg
Position[0]>=Data[j] && Position[1]<=Data[j+1] && Position[2]>=Data[j+2]
||
//lgg
Position[0]<=Data[j] && Position[1]>=Data[j+1] && Position[2]>=Data[j+2]
)
&&j<=11)

||

((
//gll
Position[0]>=Data[j] && Position[1]<=Data[j+1] && Position[2]<=Data[j+2]
||
//
//lgl
Position[0]<=Data[j] && Position[1]>=Data[j+1] && Position[2]<=Data[j+2]
||
//ggl
Position[0]>=Data[j] && Position[1]>=Data[j+1] && Position[2]<=Data[j+2]
||
//
//lll
Position[0]<=Data[j] && Position[1]<=Data[j+1] && Position[2]<=Data[j+2]
)
&&j>=12
)

)
{


}
else{
Within=false;
break;
}

}

if(Within){

printf("\n WITHIN!");

Rooms[i].Occupied=true;

for(int k=0; k<=Rooms.size()-1;k+=1){
	if(k!=i){
		Rooms[k].Occupied=false;
	}
}

return true;

}

}

return false;

}

//std::vector<Room> Rooms

//Lookup evidence from the corresponding Room
//If within distance margin of evidence, will then remove that evidence from existence
//You win if ALL evidence is found
Evidence * NearEvidence(){

//Iterate all Rooms UNTIL reach room that is occupied
//Iterate all Roo Evidence Positions (at a centerpoint)
//Compare distance of camera to distance of this object
//If distance <=D, then remove evidence, found it!
for(int i=0; i<Rooms.size();i+=1){
	if(Rooms[i].Occupied){
		for(int j=0; j<Rooms[i].Evidences.size();j+=1){
			Evidence E = Rooms[i].Evidences[j];
			printf("\n Evidence %s Position:%f,%f,%f,",E.Name,E.Vertices[0],E.Vertices[1],E.Vertices[2]);
			printf("\nCam Position:%f,%f,%f",camera.Position[0],camera.Position[1],camera.Position[2]);
			float Distance = sqrt( 
			pow(camera.Position[0] - Rooms[i].Evidences[j].Vertices[0],2) + 
			pow(camera.Position[1] - Rooms[i].Evidences[j].Vertices[1],2) +
			pow(camera.Position[2] - Rooms[i].Evidences[j].Vertices[2],2)
			); 
			printf("Effective Distance:%f",Distance);
			if(Distance<=DetectionDistance)
				return &(Rooms[i].Evidences[j]);
		}
		printf("\n No Evidence Found (WITHIN A DISTANCE)!");
		return NULL;
	}
}

printf("\n No rooms occupied?!?");

return NULL;;
}

int main()
{
        
    //AllRooms.push_back(BedRoomVertices);

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
    glfwSetMouseButtonCallback(window, mouse_button_callback);

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

    // load/store models
    // -----------

    //BedRoom Evidence
    std::vector<Evidence> BRE; 
    
    
//    printf("\n Camera Position:%f,%f,%f",camera.Position[0],camera.Position[1],camera.Position[2]);

    //printf("\n WITHIN BOUNDS:%d",WithinBounds(camera.Position));

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

    Model * RightShoe = new Model(FileSystem::getPath("../BedRoom/RightShoe.obj"),"../BedRoom");
    
    //REMEMBER THAT THE THE Y BECOMES Z AND IT IS NEGATIVED!!!!!!
    //So x1,y1,z1 -> x1,z1,-y1
    //Create accurate vertices of all evidence positions here
    float ShoePosition[]={
	-0.072f,
	//3.3826f,
	0.039f/*-2.5f*/,
	-3.3826f,
	//0.039f
    };
    BRE.push_back(Evidence("RightShoe",&RightShoe,ShoePosition)); 

    Model Step(FileSystem::getPath("../BedRoom/Step.obj"),"../BedRoom");
	 
    Model TvGun(FileSystem::getPath("../BedRoom/TvGun.obj"),"../BedRoom");
    
    //Model Fan(FileSystem::getPath("../BedRoom/fan.obj"),"../BedRoom");
    //Model Phone2(FileSystem::getPath("../BedRoom/Phone.obj"),"../BedRoom");

    Rooms.push_back(Room(BedRoomVertices,BRE));

    //*/

    //Model Room
 
    // draw in wireframe
    //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    
    // render loop
    
    // -----------
    
      printf("\n WITHIN BOUNDS:%d",WithinBounds(camera.Position));


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
        //model = glm::translate(model, glm::vec3(0.0f, 0.0f, -0.466f)); // translate it down so it's at the center of the scene
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
	
	if(RightShoe!=NULL)
		(*RightShoe).Draw(ourShader);

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

   //printf("\nCam Position:%f,%f,%f",camera.Position[0],camera.Position[1],camera.Position[2]);

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

void mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
{
    if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS)
    {    
	printf("\n CLICKED");	

	Evidence* Evidence = (NearEvidence());

		if(Evidence!=NULL){

			//DESTROY EVIDENCE!!!
			(*Evidence).NoMore();
			(*Evidence).~Evidence();

		}
    }
}
