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

#include <ctime>

#include <math.h>

#include "include/Shader.h"
#include "include/Camera.h"
#include "include/Model.h"

#include <cstdlib>

#include "include/FileSystem.h"
#include <random>
#include <chrono>
#include <algorithm>
#include <iostream>

float YouWinTimeStart = 0.0f;

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void mouse_button_callback(GLFWwindow* window, int button, int action, int mods);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void key_callback(GLFWwindow* window, int key, int r, int x,int o);
void processInput(GLFWwindow *window);


//Key
bool Keys[1024];

//For every movement U,D,L,R,UU,DD
//CANNOT go again if already couldn't
//IF try another move, resets all OTHER movements
bool CantGoAgain[] = {
false,false,false,false,
false,false
};

bool isPartOf(const char* w1, const char* w2)
{
    int i=0;
    int j=0;

    for(i;i < strlen(w1); i++)
    {
        if(w1[i] == w2[j])
        {
            j++;
        }
    }

    if(strlen(w2) == j)
        return true;
    else
        return false;
}


void UpdateGoAgain(int Position){

//CHECK if all options are false...
//BAD!


if(!CantGoAgain[Position]){
CantGoAgain[Position] = true;
for(int i=0; i<=5;i+=1)
	if(i!=Position){
		CantGoAgain[Position] = false;
	}
}
else{
int Any = -1;

for(int i=0; i<=5;i+=1)
        if(!CantGoAgain[i]){
                Any=i;
		break;
        }

if(Any==-1){
for(int i=0; i<=5;i+=1)
       	CantGoAgain[i]=false;

}

}

}

// settings
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

// camera
Camera camera(glm::vec3(0.0f, 0.9f, 0.0f));

glm::vec3 ConvertCam(){
return glm::vec3(camera.Position[0],camera.Position[2],-camera.Position[1]);
}

glm::vec3 ConvertP(glm::vec3 P){
return glm::vec3(P[0],P[2],-P[1]);
}

float PrevCameraPosition[]={
0.0f,
0.0f,
0.0f
};

//If turn look any direction, will still always move forward normally
//float StableLook[]={};
bool StableMove = true;
//My Movement Parameters
float MyX = SCR_WIDTH / 2.0f;
float MyY = SCR_HEIGHT / 2.0f;
float MyLookX = 0.0f;
float MyLookY = 0.0f;
float MyZ = 0.0f;

float DetectionDistance = 5.0f;

//DEFINE ALL ANIMATION START AND END POSITIONS

float KitchenWaterStart [] ={
-15.00,
2.24067,
14.259,
};
float KitchenWaterEnd [] = {
-15.00,
1.46828,
14.259,
};

//END DEFINE ALL ANIMATION START AND END POSITIONS

class Animated{
public:
	std::vector<glm::vec3> Positions;
	float TimeStep;
	//Make a function which increments time counter
	//COUNTER will ONLY be incremented if time given to it
	int TimeCounter = 0;
	clock_t begin = clock();
  	//clock_t end = clock();
  	//float elapsed_secs = float(end - begin) / CLOCKS_PER_SEC;
	float TotalTime;
	Animated(float* Start, float* End, int NumSteps, int Type);
	bool Update();
};

Animated::Animated(float* Start, float* End, int NumSteps, int Type){

NumSteps+=1;

if(NumSteps<=0 || Start == NULL || End == NULL){
printf("\nBad animate input");
return;
}

switch(Type){

//Falling Object
case 0:{

this->Positions.reserve(NumSteps);

float Distance = sqrt(
pow(abs(Start[0]-End[0]),2) + 
pow(abs(Start[1]-End[1]),2) +
pow(abs(Start[2]-End[2]),2) 
);

printf("\n Distance: %f",Distance);

this->TotalTime = sqrt(Distance/4.9);
this->TimeStep = TotalTime / NumSteps;

//printf("\n TotalTime Taken for my OBJ to fall:%f",TotalTime);

float XX [] = {
Start[0],
Start[1],
Start[2],
};

//printf("\n First POSE:(%f,%f,%f)",XX[0],XX[1],XX[2]);

//printf("\n END TO:

for(;XX[1]>=End[1];XX[1]-=Distance/NumSteps){
//	printf("\n Next POSE:(%f,%f,%f)",XX[0],XX[1],XX[2]);
	this->Positions.push_back(glm::vec3(XX[0],XX[1],XX[2]));
}

break;
}
//...
default:
break;

}

}

//Will Update the Counter for Position in Vector array
//When get time() - start time() >= TimStep
//Then set star time() to get time()
bool Animated::Update(){

clock_t end = clock();

float elapsed_secs = float(end - begin);

if(elapsed_secs >=this->TimeStep){

//printf("\n this Animated Object has surpassed timestep:%f",this->TimeStep);

this->begin = end;

if(this->TimeCounter==this->Positions.size()-1){
this->TimeCounter = 0;

//Reached END of animated object...

return true;
}

else
this->TimeCounter+=1;

//printf("\n ARray Size:%lu",this->Positions.size());

//printf("\n Now using Position in Array:%d ->(%f,%f,%f)",TimeCounter,this->Positions[TimeCounter][0],this->Positions[TimeCounter][1],this->Positions[TimeCounter][2]);


}

return false;
}

class End{
public:
        glm::vec3 Here;
        glm::mat4 Projection;
        glm::mat4 View;
        glm::mat4 Model;
        End(glm::vec3 H, glm::mat4 P, glm::mat4 V, glm::mat4 M);

};

End::End(glm::vec3 H, glm::mat4 P, glm::mat4 V, glm::mat4 M){
this->Here = H;
this->Projection = P;
this->View = V;
this->Model = M;
}


//Stores info about each evidence
class Evidence{
public:
	//If randomizing all positions, set this parameter
	// because will translate to -Vertices and then translate 
	// to +NextVertices
	float* NextVertices = NULL;
	float* Vertices;
	End* HPVM;
	bool Found = false;
	const char* Name;
	Model** M;
	Evidence(const char* Name,Model** Model,float* EvidenceVertices);
	~Evidence();
	void NoMore();
};

Evidence::Evidence(const char*N,Model** Model,float*V){
this->Name=N;
printf("\n Made MY Evidence:%s",N);
this->Vertices=V;
this->M=Model;
}

void Evidence::NoMore(){
*(this->M)=NULL;
printf("\n GoodBye Evidence:%s @ (%f,%f,%f)",this->Name,Vertices==NULL? 0.0f:Vertices[0],Vertices==NULL? 0.0f:Vertices[1],Vertices==NULL? 0.0f:Vertices[2]);
(this->Vertices)=NULL;
}

Evidence::~Evidence(){

//Goodbye Evidence!!
//if(Vertices!=NULL)
//printf("\n GoodBye Evidence:%s @ (%f,%f,%f)",this->Name,Vertices==NULL? 0.0f:Vertices[0],Vertices==NULL? 0.0f:Vertices[1],Vertices==NULL? 0.0f:Vertices[2]);

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

	//Open room when Evidence Door is clicked...
	bool Open = false;
		
	//Destructor
//	~Room();


};

Room::Room(float*Vertices,std::vector<Evidence>EvidenceVertices){
this->Vertices=Vertices;
this->Evidences=EvidenceVertices;
}
/*
Room::~Room(){

printf("\n NO ROOM");

}
*/
std::vector<Room> Rooms;

//Store Vertices for each room to then check if within a room
//For rendering efficiency and bounds checking
//Any point must be >= all lower plane values and <= all upper plane values
//Store in order of Lower plane (4 vertices = 12 floats)
//Then Upper plane (remaining 12 float values)
//In format x1,y1,z1,x2,y2,z2,....,z24
//std::vector<float> BedRoomVertices;
//bool InBedRoom=true;

//From Bathroom to kitchen...
bool WhichRoom[] = {
true,
false,
false,
false
};

float BedRoomVertices[]={
//BOTTOM FACE
-6.63,-0.1,-6.26,
-6.63,-0.1,6.317,
6.69,-0.1,6.317,
6.69,-0.1,-6.26,
//END BOTTOM FACE
//TOP FACE
-6.63,7.22,-6.26,
-6.63,7.22,6.317,
6.69,7.22,6.317,
6.69,7.22,-6.26,
//END TOP FACE
};

float BathRoomVertices[]={
//BOTTOM FACE
6.69, -0.1013, -6.2588,
6.69, -0.1005, 6.317,
19.624, -0.205, 6.362,
19.648, -0.056, -6.429,
//END BOTTOM FACE

//TOP FACE
6.69, 7.21, -6.2588,
6.69, 7.21, 6.317,
19.624, 7.21, 6.362,
19.648, 7.21, -6.429,
//END TOP FACE
};

float LivingRoomVertices[]={

//BOTTOM FACE
-6.63f,-0.1f,6.34f,
-6.63f,-0.1f,22.777f,
6.69f,-0.1f,22.777f,
6.69f,-0.1f,6.34f,
//END BOTTOM FACE

//TOP FACE
-6.63f,7.225f,6.34f,
-6.63f,7.225f,22.777f,
6.69f,7.225f,22.777f,
6.69f,7.225f,6.34f
//END TOP FACE

};
float KitchenVertices[]={
//BOTTOM FACE
-22.927f,-0.362f,6.625f,
-22.722f, -0.373f, 22.71f,
-6.3f, -0.09f, 22.763f,
-6.3f, -0.084f, 6.34f,
//END BOTTOM FACE
//TOP FACE
-22.927f,7.35f,6.625f,
-22.722f, 7.35f, 22.71f,
-6.3f, 7.35f, 22.763f,
-6.3f, 7.35f, 6.34f,
//END TOP FACE
};

float* Copy (float* F){

float* C = (float*)malloc(24*sizeof(float));

for(int i=0; i<=23;i+=1)
C[i]=F[i];

return C;
}

float* Copy2(float* F){
float* C = (float*)malloc(3*sizeof(float));

for(int i=0; i<=2;i+=1)
C[i]=F[i];

return C;
}



//All Rooms Stored
std::vector<float*> AllRooms;

bool firstMouse = true;

// timing
float deltaTime = 0.0f;
float PreDelta = 0.0f;
float DeltaMultiplier = 10.0f;

float lastFrame = 0.0f;

//Simple Check utilizing here to there case...
        //Use vertices of door between and furthest vertices in room to
//wards
bool WithinTransitionBounds(glm::vec3 Position,int Current,int Next){
printf("\n THE Position:%f,%f,%f\n",Position[0],Position[1],Position[2]);

float DoorMidPoint = 0.0f;
//Door is 0.21 from midpoint on both sides
float DoorLeft = -1.35f;
float DoorRight = 1.35f;

float TopTFace [] ={
//In
0.0f,0.0f,0.0f,
0.0f,0.0f,0.0f,

//Out
0.0f,0.0f,0.0f,
0.0f,0.0f,0.0f
};

float BottomTFace [] ={
//In
0.0f,0.0f,0.0f,
0.0f,0.0f,0.0f,

//Out
0.0f,0.0f,0.0f,
0.0f,0.0f,0.0f
};

//General Elevation is -0.1f - 7.22f

bool XAxis = false;

//02 20 01 11 13 31
if(Current==0){
//6.69,7.22,6.317,
//6.69,7.22,-6.26,
if(Next==2){
DoorMidPoint = (BedRoomVertices[8] + BedRoomVertices[11])/2.0f;
}
//-6.63,7.22,6.317,
//6.69,7.22,6.317,
else if(Next==1){
XAxis=true;
DoorMidPoint = (BedRoomVertices[3] + BedRoomVertices[6])/2.0f;
}
}
else if(Current==1){
if(Next==0){
XAxis=true;
DoorMidPoint = (BedRoomVertices[3] + BedRoomVertices[6])/2.0f;
}
else if(Next==3){
//-6.63f,-0.1f,6.34f,
//-6.63f,-0.1f,22.777f,
//DoorMidPoint = (LivingRoomVertices[2] + LivingRoomVertices[5])/2.0f;

DoorMidPoint = 12.07f;

}
}
else if(Current==2){
if(Next==0){
DoorMidPoint = (BedRoomVertices[8] + BedRoomVertices[11])/2.0f;
}
}
else if(Current==3){
if(Next==1){
//DoorMidPoint = (LivingRoomVertices[2] + LivingRoomVertices[5])/2.0f;
DoorMidPoint = 12.07f;
}
}
DoorLeft+=DoorMidPoint;
DoorRight+=DoorMidPoint;

printf("\n Door Midpoint:%f",DoorMidPoint);
printf("\n Door Left:%f, Door Right:%f",DoorLeft,DoorRight);

//Use Same WithinBounds Function for Current ROOM

bool Within = true;

printf("\nTRY Room:%d",Current);

//Current = Next;

for(int j=0; j<=23;j+=3){

float* Data = Copy(Rooms[Next].Vertices);

/*
printf("\nData:%f,%f,%f",Data[j],Data[j+1],Data[j+2]);

printf("\n VS POSITION:%f,%f,%f,",Position[0],Position[1],Position[2]);
*/

//float* Data = Copy(Rooms[Current].Vertices);

if(XAxis){

if(
j<=5 || (j>=12&&j<=17))
{
printf("\n j<=5 || (j>=12&&j<=17)");
Data[j] = DoorLeft;
}

else if((j>=6&&j<=8) || (j>=9&&j<=11) || (j>=18)){
printf("\n (j>=6&&j<=8) || (j>=9&&j<=11) || (j>=18)");
Data[j] = DoorRight;
}

}
else{

if(
j<=2 || (j>=9&&j<=11) || 
(j>=12 && j<=14) || (j>=21&&j<=23)
){
Data[j+2] = DoorLeft;
}

else if((j>=3&&j<=8) ||(j>=15&&j<=20)){
Data[j+2] = DoorRight;
}

}

printf("\nData:%f,%f,%f",Data[j],Data[j+1],Data[j+2]);

printf("\n VS POSITION:%f,%f,%f,",Position[0],Position[1],Position[2]);

bool AboveFirstPlane = (
Position[1]>=Data[j+1]
)
;

bool FirstPlane = 
(
(
//Check if not too tall or short
(
Position[1]>=Data[j+1]
)
&&
//And check if within 2d box
(

//gl
(Position[0]>=Data[j] && Position[2]<=Data[j+2] && j==3)
||
//(gg
(Position[0]>=Data[j] && Position[2]>=Data[j+2] && j==0)
||
//lg
(Position[0]<=Data[j] && Position[2]>=Data[j+2] && j==9)
||
//ll
(Position[0]<=Data[j] && Position[2]<=Data[j+2] && j==6)

)

)
&&j<=11
)
;

bool BelowSecondPlane = (
Position[1]<=Data[j+1]
);

bool SecondPlane = 
(
(//Check if not too tall or short

(
Position[1]<=Data[j+1]
)

&&

//And check if within 2d box
(

//gl
Position[0]>=Data[j] && Position[2]<=Data[j+2] && j==15
||
//gg
Position[0]>=Data[j] && Position[2]>=Data[j+2] && j==12
||
//lg
Position[0]<=Data[j] && Position[2]>=Data[j+2] && j==21
||
//ll
Position[0]<=Data[j] && Position[2]<=Data[j+2] && j==18

)

)
&&j>=12

);

printf("\n Above First Plane:%d",AboveFirstPlane);
printf("\n First Plane:%d",FirstPlane);

printf("\n Below Second Plane:%d",BelowSecondPlane);
printf("\n Second Plane:%d",SecondPlane);

//Checking if either above a plane or below a plane, because ether you are there, or you arent!
if(
(FirstPlane || SecondPlane) && (Rooms[Current].Open || Current==0)
)
{

//Within=Within && true;

Within = true;

}
else{
Within=false;
break;
}

}

if(Within){

printf("\n WITHIN!");

///////////////////////////// TO CHECK FAST, CHANGE Next to Current
//It will ALWAYS look to see if within the small bounding box of the door!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

Rooms[Next].Occupied=true;

Rooms[Next].Open=true;

for(int k=0; k<=Rooms.size()-1;k+=1){
	if(k!=Next){
		Rooms[k].Occupied=false;
	}
}

//Good... Moving From somewhere to Somewhere
/*
//Now check if transition from room to room!!
if(CurrentRoom!=-1 && CurrentRoom!=i){
	
	//Know that CurrentRoom is not match, so transitioned
	printf("\nTransitioned from Room:%d to Room:%d",CurrentRoom,i);
	//Simple Check utilizing here to there case...
	//Use vertices of door between and furthest vertices in room towards
	return WithinTransitionBounds(Position,CurrentRoom,i);	
}

*/
return true;

}

printf("\n NOT WITHIN TRANSITION ZONE!!!!");

Rooms[Current].Occupied = true;
Rooms[Next].Occupied = false;

return false;

}

//My Position will use camera's position
bool WithinBounds(glm::vec3 Position){
//printf("\n Camera Position:%f,%f,%f",camera.Position[0],camera.Position[1],camera.Position[2]);
printf("\n THE Position:%f,%f,%f\n",Position[0],Position[1],Position[2]);
//Check if either in bed room, living room, or bath room
//Check for all rooms
//IF in NEITHER ROOMS, return false!
//Otherwise Return True
/*if(MyPosition.size()!=3){
printf("\n Invalid Bound Vector");
return false;
}
*/

int CurrentRoom = -1;

//float * Position = MyPosition.data();

bool Within = true;

for(int i=0;i<Rooms.size();i+=1){

if(Rooms[i].Occupied){
CurrentRoom = i;
}

/*
if(CurrentRoom!=-1 && CurrentRoom!=i){

}
*/

printf("\nRoom:%d",i);
for(int j=0; j<=23;j+=3){

printf("\nData:%f,%f,%f",Rooms[i].Vertices[j],Rooms[i].Vertices[j+1],Rooms[i].Vertices[j+2]);

printf("\n VS POSITION:%f,%f,%f,",Position[0],Position[1],Position[2]);

float* Data = Rooms[i].Vertices;

bool AboveFirstPlane = (
Position[1]>=Data[j+1]
)
;

bool FirstPlane = 
(
(
//Check if not too tall or short
(
Position[1]>=Data[j+1]
)
&&
//And check if within 2d box
(

//gl
(Position[0]>=Data[j] && Position[2]<=Data[j+2] && j==3)
||
//(gg
(Position[0]>=Data[j] && Position[2]>=Data[j+2] && j==0)
||
//lg
(Position[0]<=Data[j] && Position[2]>=Data[j+2] && j==9)
||
//ll
(Position[0]<=Data[j] && Position[2]<=Data[j+2] && j==6)

)

)
&&j<=11
)
;

bool BelowSecondPlane = (
Position[1]<=Data[j+1]
);

bool SecondPlane = 
(
(//Check if not too tall or short

(
Position[1]<=Data[j+1]
)

&&

//And check if within 2d box
(

//gl
Position[0]>=Data[j] && Position[2]<=Data[j+2] && j==15
||
//gg
Position[0]>=Data[j] && Position[2]>=Data[j+2] && j==12
||
//lg
Position[0]<=Data[j] && Position[2]>=Data[j+2] && j==21
||
//ll
Position[0]<=Data[j] && Position[2]<=Data[j+2] && j==18

)

)
&&j>=12

);

printf("\n Above First Plane:%d",AboveFirstPlane);
printf("\n First Plane:%d",FirstPlane);

printf("\n Below Second Plane:%d",BelowSecondPlane);
printf("\n Second Plane:%d",SecondPlane);

//Checking if either above a plane or below a plane, because ether you are there, or you arent!
if(
(FirstPlane || SecondPlane) && (Rooms[i].Open || i==0)
)
{

//Within=Within && true;

Within = true;

}
else{
Within=false;
break;
}

}

if(Within){

printf("\n WITHIN!");

printf("\n Current ROom:%d,I:%d",CurrentRoom,i);

/*
Rooms[i].Occupied=true;

for(int k=0; k<=Rooms.size()-1;k+=1){
	if(k!=i){
		Rooms[k].Occupied=false;
	}
}
*/
//Good... Moving From somewhere to Somewhere

//Now check if transition from room to room!!
if(CurrentRoom!=i){
        if(CurrentRoom==-1){
	for(int i=0; i<Rooms.size();i+=1){
		if(Rooms[i].Occupied){
			CurrentRoom = i;
			break;
		}
	}
	if(CurrentRoom<0){
		printf("\n FAILED TO FIND CURRENT ROOM!!!!!!!!!!!!");
		exit(-1);
	}
	}	
	//Know that CurrentRoom is not match, so transitioned
	printf("\nTransitioned from Room:%d to Room:%d",CurrentRoom,i);
	//Simple Check utilizing here to there case...
	//Use vertices of door between and furthest vertices in room towards
	bool WTB = 
        WithinTransitionBounds(Position,CurrentRoom,i);	

	printf("\n Are We Within Transition Zone?:%d",WTB);
	return WTB;
}
else{

Rooms[i].Occupied=true;

for(int k=0; k<=Rooms.size()-1;k+=1){
        if(k!=i){
                Rooms[k].Occupied=false;
        }
}

}

return true;

}

}

printf("\n NOT WITHIN");

//If not within any room. Error
if(!WithinBounds(camera.Position)){
	printf("\n CRITICAL ERROR! Ended up outside of BOUNDS!!\nRestart game. Make sure that if you experience low frame rate, take you time and move slowly!!");
exit(-1);
}

return false;

}

bool PreProcessKeyboard(Camera_Movement direction, float deltaTime)
    {
	//if(PreDelta<=0.0f)
	//PreDelta = deltaTime/2.0f;
	float r = static_cast <float> (rand()) / static_cast <float> (RAND_MAX);
        PreDelta = 0.4f + r;

        float velocity = camera.GetMovementSpeed() * PreDelta;

        printf("\n VELOCITY!%f",velocity);

	glm::vec3 Position(camera.Position[0],camera.Position[1],camera.Position[2]);
/*
	if(StableMove){
        printf("\n STABLE 1 \n");
        camera.ProcessMouseMovement(0.0f,-MyLookX);
        }
*/
        if (direction == FORWARD)
            Position += camera.Front * velocity;
        else if (direction == BACKWARD)
            Position -= camera.Front * velocity;

        else if (direction == LEFT)
            Position -= camera.Right * velocity;
        else if (direction == RIGHT)
            Position += camera.Right * velocity;

        else if (direction==UP && !CantGoAgain[4] && (MyLookX==0 /*|| MyLookX==45 || MyLookX==-45*/))
            Position += camera.Up * velocity;
        else if (direction==DOWN && !CantGoAgain[5] && (MyLookX==0 /*|| MyLookX==45 || MyLookX==-45*/))
            Position -= camera.Up * velocity;
	else{
		return false;
	}

	printf("\n PREPROCESS Position To:%f,%f,%f TEST",Position[0],Position[1],Position[2]);

	//return //WithinBounds(ConvertP(Position));
	
	if(WithinBounds(Position)){
	
	if(StableMove && direction!=UP && direction!= DOWN){
        printf("\n STABLE 1 \n");
	camera.ProcessMouseMovement(0.0f,-MyLookX);
        }
        
	//camera.ProcessMouseMovement(0.0f,-MyLookX);
	//camera.Position = (Position);
	camera.ProcessKeyboard(direction,PreDelta);
	
 	if(StableMove && direction!=UP && direction!=DOWN){
        printf("\n STABLE 2 \n");
	camera.ProcessMouseMovement(0.0f,MyLookX);
        }
	
	//camera.ProcessMouseMovement(0.0f,-MyLookX);
	
	return true;
	
	}

	PreDelta = 0.0f;
/*
	if(StableMove){
        printf("\n STABLE 2 \n");
        camera.ProcessMouseMovement(0.0f,MyLookX);
        }
*/
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

//char* Name = "";
float Min = 10000.0f;
int I = -1;
int J = -1;

for(int i=0; i<Rooms.size();i+=1){
	printf("\n ROOM:%d",i);
	
	//char* Name = "";
	//float Min = 10000.0f;

	//if(Rooms[i].Occupied){
		for(int j=0; j<Rooms[i].Evidences.size();j+=1){
			Evidence E = Rooms[i].Evidences[j];					if(E.Vertices!=NULL && *(E.M)!=NULL){
			printf("\n Evidence %s Position:%f,%f,%f,",E.Name,E.Vertices[0],E.Vertices[1],E.Vertices[2]);
			printf("\nCam Position:%f,%f,%f",camera.Position[0],camera.Position[1],camera.Position[2]);
			float Distance = sqrt( 
			pow(camera.Position[0] - Rooms[i].Evidences[j].Vertices[0],2) + 
			pow(camera.Position[1] - Rooms[i].Evidences[j].Vertices[1],2) +
			pow(camera.Position[2] - Rooms[i].Evidences[j].Vertices[2],2)
			); 
			printf("Effective Distance:%f VS Detect Distance:%f",Distance,DetectionDistance);
			if(Distance<=DetectionDistance && Distance<=Min){
	/*			if(strcmp(Rooms[i].Evidences[j].Name,"DOOR")==0){
	printf("\n Remove a DOOR");
	Rooms[i].Open = true;
	}
	*/	
		Min = Distance;	
		//Name = Rooms[i].Evidences[j].Name;
		I = i;
		J = j;
		//return &(Rooms[i].Evidences[j]);
	
	}
			}

		}

		if(Min!=10000.0f && I!=-1 && J!=-1){
			 if(isPartOf(Rooms[I].Evidences[J].Name,"DOOR")){
        printf("\n Remove a DOOR");
        Rooms[I].Open = true;
        	}
		return &(Rooms[I].Evidences[J]);
		}
		//printf("\n No Evidence Found (WITHIN A DISTANCE)!");
		//return NULL;
}


printf("\n No rooms occupied?!?");

return NULL;;
}

bool RenderSave(int IDX,int I){
//Find Room that is available

//if inside bathroom, don't render kitchen!

//if inside kitchen, don't render bathroom and bedroom!

switch(IDX){

//BedRoom
case 0:

break;
//LivingRoom
case 1:

break;
//Bathroom
case 2:
return I!=3;
break;
//Kitchen
case 3:
return I!= 2 && I!=0;
break;	

default:
break;

}

return true;

}
/*
class End{
public:
	glm::vec3 Here;
	glm::mat4 Projection;
	glm::mat4 View;
	glm::mat4 Model;
	End(glm::vec3 H, glm::mat4 P, glm::mat4 V, glm::mat4 M);

};

End::End(glm::vec3 H, glm::mat4 P, glm::mat4 V, glm::mat4 M){
this->Here = H;
this->Projection = P;
this->View = V;
this->Model = M;
}
*/
/*
bool isPartOf(const char* w1, const char* w2)
{
    int i=0;
    int j=0;

    for(i;i < strlen(w1); i++)
    {
        if(w1[i] == w2[j])
        {
            j++;
        }
    }

    if(strlen(w2) == j)
        return true;
    else
        return false;
}
*/
bool ValidItem(const char* Item){

return !isPartOf(Item,"FINAL");

}


//Set up every single evidence model, proj, view, here
std::vector<End**> GetE(std::vector<Room> Rooms){

std::vector<End**> Evidences;

for(int i=0; i<Rooms.size();i+=1){
for(int j=0; j<Rooms[i].Evidences.size();j+=1){

if(Rooms[i].Evidences[j].Vertices!=NULL && ValidItem(Rooms[i].Evidences[j].Name)){

float* Vertices = Rooms[i].Evidences[j].Vertices;

glm::vec3 Here = glm::vec3(Vertices[0],Vertices[1],Vertices[2]);

glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);

        glm::mat4 view = camera.GetViewMatrix();

        // render the loaded model
        glm::mat4 model = glm::mat4(1.0f);

        //model = glm::translate(model, Here); // translate it down so it's at the center of the scene  

//Evidences.push_back(End(Here,projection,view,model));

Rooms[i].Evidences[j].HPVM = new End(Here,projection,view,model);
Evidences.push_back(&Rooms[i].Evidences[j].HPVM);

}
}
}

//printf("\n AL evidences:%d",Evidences.size());

return Evidences;

}

//le by their HERE vertices
//Then all you gotta do is for each evidence to render
//Check if their vertices match the HERE of Evidences
//if not, translate back to 0,0,0 then translate to evidences[k]
std::vector<End**> ShuffleEvidences(std::vector<End**> Evidences){

unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();

auto rng = std::default_random_engine {seed};

std::shuffle(std::begin(Evidences), std::end(Evidences), rng);

int Count = 0;

for(int i=0; i<Rooms.size();i+=1){
for(int j=0; j<Rooms[i].Evidences.size();j+=1){

if(Rooms[i].Evidences[j].Vertices!=NULL && ValidItem(Rooms[i].Evidences[j].Name)){

float* Vertices = Rooms[i].Evidences[j].Vertices;

Rooms[i].Evidences[j].HPVM = *(Evidences[Count]);

//Negative to origin
Rooms[i].Evidences[j].HPVM->Model = glm::translate(
Rooms[i].Evidences[j].HPVM->Model,-glm::vec3(Vertices[0],Vertices[1],Vertices[2]));

//Translate to HPVM->Here
Rooms[i].Evidences[j].HPVM->Model=glm::translate(
Rooms[i].Evidences[j].HPVM->Model,Rooms[i].Evidences[j].HPVM->Here);

//Re Set Vertices
Rooms[i].Evidences[j].Vertices[0] = Rooms[i].Evidences[j].HPVM->Here[0];

Rooms[i].Evidences[j].Vertices[1] = Rooms[i].Evidences[j].HPVM->Here[1];

Rooms[i].Evidences[j].Vertices[2] = Rooms[i].Evidences[j].HPVM->Here[2];

Count+=1;

if(Count>=Evidences.size()){
return Evidences;
}

}

}
}


return Evidences;

}

//Function returns std:: vector of FOUR positions
std::vector<End> EndWords(float DistFrom){

std::vector<End> Ends;

float ROT = 0.0f;

	for(int i=0; i<=4;i+=1){
	glm::vec3 Here;

	switch(i){	
	case 0:
		Here = glm::vec3(camera.Position[0], camera.Position[1], camera.Position[2]-DistFrom);
	break;
	case 1:
		Here = glm::vec3(camera.Position[0]+DistFrom, camera.Position[1], camera.Position[2]);
	break;
	case 2:
		Here = glm::vec3(camera.Position[0], camera.Position[1], camera.Position[2]+DistFrom);
	break;
	case 3:
		Here = glm::vec3(camera.Position[0]-DistFrom, camera.Position[1], camera.Position[2]);
	break;
	}
        glm::mat4 projection2 = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);

        glm::mat4 view2 = camera.GetViewMatrix();

        //endShader.setMat4("projection", projection2);
        //endShader.setMat4("view", view2);

        // render the loaded model
        glm::mat4 model2 = glm::mat4(1.0f);
        model2 = glm::translate(model2, Here); // translate it down so it's at the center of the scene          
        model2 = glm::rotate(model2,glm::radians(90.0f+(ROT==90 || ROT==270? ROT-180:ROT)),glm::vec3(0.0f,1.0f,0.0f));
        //endShader.setMat4("model", model3);

	Ends.push_back(End(Here,projection2,view2,model2));

	ROT+=90.0f;

	//printf("\n ROT:%f",ROT);

	}

return Ends;

}

End CreateEnd(glm::vec3 Here){

	glm::mat4 projection2 = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);

        glm::mat4 view2 = camera.GetViewMatrix();

        //endShader.setMat4("projection", projection2);
        //endShader.setMat4("view", view2);

        // render the loaded model
        glm::mat4 model2 = glm::mat4(1.0f);

	//model2 = glm::translate(model2, Here)
	
	return End(Here,projection2,view2,model2);
}

int randRange(int min, int max){
//return min+(rand()%static_cast<int>(max-min+1));
srand(time(0));
return rand() % (4);
}


int Dist(int Start, int End,int HowFar){

if(Start==End)
return 0;

int Order[]={
2,
0,
1,
3
};

int IdxStart = -1;
int IdxEnd = -1;
for(int i=0;i<=3;i+=1){
if(Start==Order[i])
IdxStart = i;
else if(End==Order[i])
IdxEnd = i;
}

if(abs(IdxStart-IdxEnd)<=HowFar)
printf("Distance from %dto %d:%d",Start,End,abs(IdxStart-IdxEnd));

return abs(IdxStart-IdxEnd);

}

//Gather all adjacent rooms to room given...
//Room 0 is BedRoom
//1 is LivingRoom
//2 is BathRoom
//3 is Kitchen
std::vector<int> AllAdjacents(int RoomNum,int HowFar){
int Order[]={
2,
0,
1,
3
};

std::vector<int> AllOthersFar;

int Far = 0;

printf("\n All Rooms Adjacent to:%d\n",RoomNum);

// and i>=

for(int i=0; i<4 && Far<HowFar;i+=1){

int D = Dist(RoomNum,Order[i],HowFar);

if(Order[i]!=RoomNum && D<=HowFar){
        printf("\n%d",Order[i]);
        AllOthersFar.push_back(Order[i]);
        Far+=1;
}

}

return AllOthersFar;

}


//Choose adjacent rooms
std::vector<int> GetRooms(int NumRooms){
std::vector<int> Rooms;

int StartRoom = randRange(0,5);

printf("\n Start Room:%d",StartRoom);

Rooms.push_back(StartRoom);

std::vector<int> MoreRooms;

switch(NumRooms){

case 1:
Rooms.push_back(StartRoom);
break;

case 2:
MoreRooms = AllAdjacents(StartRoom,1);
break;

case 3:
MoreRooms = AllAdjacents(StartRoom,2);
break;

case 4:
MoreRooms = AllAdjacents(StartRoom,3);
break;

default:
return Rooms;
}

for(int i=0; i<MoreRooms.size();i+=1){
Rooms.push_back(MoreRooms[i]);
}

return Rooms;

}

/*
//Gather all adjacent rooms to room given...
//Room 0 is BedRoom
//1 is LivingRoom
//2 is BathRoom
//3 is Kitchen
std::vector<int> AllAdjacents(int RoomNum,int HowFar){
int Order[]={
2,
0,
1,
3
}

std::vector<int> AllOthersFar;

int Far = 0;

printf("\n All Rooms Adjacent to:%d\n",RoomNum);

for(int i=0; i<4 && Far<HowFar;i+=1){
if(Order[i]!=RoomNum){
	printf("\n%d",Order[i]);
	AllOthersFar.push_back(Order[i]);
	Far+=1;
}
}

return AllOthersFar;

}
*/

char* sub(char* A,int Start){
char* next = A+Start;
return next;
}


bool Container[]={
false,
false,
false,
false
};

void ContainsInt(std::vector<int>R,int I){

for(int i=0 ;i<R.size();i+=1){
Container[R[i]]=true;
//if(R[i]==I)
//return true;
}

//return false;
}

/*
bool ValidItem(const char* Item){

return strcmp(Item,"DOOR")!=0;

}
*/
int main(/*int argc, char** argv*/)
{
    int argc = 0;
    char** argv = NULL;
    printf("\n Number of Arguments:%d",argc);     
    
    int Args[]={
	//Level Index
	-1,
	//Seed Index
	-1
    };

    //If user enters a valid integer...
  
    if(argc==2){
	if(isPartOf(argv[1],"Level=")){
		printf("\n%c",*(*(argv+1)+6));
		//sprintf(*(*(argv+1)+6),"%d",Args[0]);
		Args[0] = atoi(sub(argv[1],6));
	}
	else if(isPartOf(argv[1],"Seed=")){
	 	//sprintf(((*argv+1)+5),"%d",Args[1]);
		Args[1] = atoi(sub(argv[1],5));
	}	
    }
    else if(argc==3){
	for(int i=1;i<=2;i+=1){
		/*char A[]=
		{
		*((*argv+1)+5)
		};
		*/
		if(isPartOf(argv[i],"Seed=")){
			 //sprintf((*(argv+1)+5),"%d",Args[1]);
			Args[1] = atoi(sub(argv[i],5));
		}	
		else if(isPartOf(argv[i],"Level=")){
			 //sprintf((*(argv+1)+6),"%d",Args[0]);
			Args[0] = atoi(sub(argv[i],6));
		}
	}
    }

    printf("\n%d,%d\n",Args[0],Args[1]);

    std::vector<int>RoomOrder;
    /*if(argc>1)
    sprintf(argv[1],"%d",Seed);
    */
	
    if(Args[1]>=0){
    printf("\n YOU Confirm Randomized Game");
    }
    if(Args[0]>=1){
    RoomOrder = GetRooms(Args[0]);
    printf("\n YOU Chose A Game with (Adjacent) Rooms");
    }
    else{
    //This is the room you start in
    RoomOrder.push_back(0);
    //These are all other rooms
    RoomOrder.push_back(0);
    RoomOrder.push_back(1);
    RoomOrder.push_back(2);
    RoomOrder.push_back(3);
    }
   
    //return 0;
 
    Animated KitchenDroplet(KitchenWaterStart,KitchenWaterEnd,10,0);
    
    std::vector<Evidence> BedRoomItems;
    //BathRoom
    std::vector<Evidence> BathRoomItems;
    //Living
    std::vector<Evidence> LivingRoomItems;
    //Kitchen
    std::vector<Evidence> KitchenItems;
    //STATIC ITEMS

    //ContainsInt(RoomOrder,0);
    
    Model*EmptyBedRoom=NULL;
    Model* Bed;
    Model* Furniture;
    Model* MoreBedRoomItems;
    
    if(Container[0]){
    //BEDROOM
    EmptyBedRoom = new Model(FileSystem::getPath("../BedRoom/EmptyRoom.obj"),"../BedRoom");
    BedRoomItems.push_back(Evidence("STATIC_EmptyRoom",&EmptyBedRoom,NULL)); 
    Bed = new Model(FileSystem::getPath("../BedRoom/Bed.obj"),"../BedRoom");
    BedRoomItems.push_back(Evidence("STATIC_Bed",&Bed,NULL));
    Furniture = new Model(FileSystem::getPath("../BedRoom/Furniture.obj"),"../BedRoom");
    BedRoomItems.push_back(Evidence("STATIC_FURNITURE",&Furniture,NULL));
    MoreBedRoomItems = new Model(FileSystem::getPath("../BedRoom/MoreItems.obj"),"../BedRoom");
    BedRoomItems.push_back(Evidence("STATIC_MoreBR",&MoreBedRoomItems,NULL));
    //END BEDROOM
    }
    
    if(Container[2]){
    //BATHROOM
    Model* Dresser = new Model(FileSystem::getPath("../BathRoom/bathroomDresser.obj"),"../BathRoom");
    BathRoomItems.push_back(Evidence("STATIC_Dresser",&Dresser,NULL));
/*
<<<<<<< HEAD
    
     
=======
>>>>>>> c6abcd87be8673c5af71129e04aeb22205e71717
*/
    Model* Window = new Model(FileSystem::getPath("../BathRoom/window.obj"),"../BathRoom");
    BathRoomItems.push_back(Evidence("STATIC_Window",&Window,NULL));
    
    Model* Tub = new Model(FileSystem::getPath("../BathRoom/bathtub.obj"),"../BathRoom");
    BathRoomItems.push_back(Evidence("STATIC_Tub",&Tub,NULL));
    
    Model* Bathroom = new Model(FileSystem::getPath("../BathRoom/Bathroom.obj"),"../BathRoom");
    BathRoomItems.push_back(Evidence("STATIC_Bathroom",&Bathroom,NULL));
    
    Model* Toilet = new Model(FileSystem::getPath("../BathRoom/toilet.obj"),"../BathRoom");
    BathRoomItems.push_back(Evidence("STATIC_Toilet",&Toilet,NULL));
    Model* Shower = new Model(FileSystem::getPath("../BathRoom/shower.obj"),"../BathRoom");
//<<<<<<< HEAD
    BathRoomItems.push_back(Evidence("STATIC_Bathroom",&Shower,NULL));
    
//=======
    //BathRoomItems.push_back(Evidence("STATIC_Shower",&Shower,NULL));
    
    Model* Handsoap = new Model(FileSystem::getPath("../BathRoom/handsoap.obj"),"../BathRoom");
    BathRoomItems.push_back(Evidence("STATIC_Handsoap",&Handsoap,NULL));
    Model* ToothpasteandBrush = new Model(FileSystem::getPath("../BathRoom/ToothpasteandBrush.obj"),"../BathRoom");
    BathRoomItems.push_back(Evidence("STATIC_ToothpasteandBrush",&ToothpasteandBrush,NULL));
    
//>>>>>>> c6abcd87be8673c5af71129e04aeb22205e71717
    //END BATH ROOM
    }
    
    if(Container[3]){
    //KITCHEN
    
    Model* Cabinets = new Model(FileSystem::getPath("../Kitchen/cabinets.obj"),"../Kitchen");
    KitchenItems.push_back(Evidence("STATIC_Cabinets",&Cabinets,NULL));
    Model* Chairs = new Model(FileSystem::getPath("../Kitchen/kitchenChairs.obj"),"../Kitchen");
    KitchenItems.push_back(Evidence("STATIC_Chairs",&Chairs,NULL));
    
    Model* Dishes = new Model(FileSystem::getPath("../Kitchen/dishes.obj"),"../Kitchen");
    KitchenItems.push_back(Evidence("STATIC_Dishes",&Dishes,NULL));
    Model* Blender = new Model(FileSystem::getPath("../Kitchen/blender.obj"),"../Kitchen");
    KitchenItems.push_back(Evidence("STATIC_Blender",&Blender,NULL));
    Model* CeilingHoods = new Model(FileSystem::getPath("../Kitchen/ceilingHoods.obj"),"../Kitchen");
    KitchenItems.push_back(Evidence("STATIC_CeilingHoods",&CeilingHoods,NULL));
    Model* Fridge = new Model(FileSystem::getPath("../Kitchen/fridge.obj"),"../Kitchen");
    KitchenItems.push_back(Evidence("STATIC_Fridge",&Fridge,NULL));
    Model* Island = new Model(FileSystem::getPath("../Kitchen/island.obj"),"../Kitchen");
    KitchenItems.push_back(Evidence("STATIC_Island",&Island,NULL));
    Model* Kitchen = new Model(FileSystem::getPath("../Kitchen/Kitchen.obj"),"../Kitchen");
    KitchenItems.push_back(Evidence("STATIC_Kitchen",&Kitchen,NULL));
    Model* Pot = new Model(FileSystem::getPath("../Kitchen/pot.obj"),"../Kitchen");
    KitchenItems.push_back(Evidence("STATIC_Pot",&Pot,NULL));
    Model* Stove = new Model(FileSystem::getPath("../Kitchen/stove.obj"),"../Kitchen");
    KitchenItems.push_back(Evidence("STATIC_Stove",&Stove,NULL));
    Model* Teapot = new Model(FileSystem::getPath("../Kitchen/teapot.obj"),"../Kitchen");
    KitchenItems.push_back(Evidence("STATIC_Teapot",&Teapot,NULL));
    Model* Toaster = new Model(FileSystem::getPath("../Kitchen/toaster.obj"),"../Kitchen");
    KitchenItems.push_back(Evidence("STATIC_Toaster",&Toaster,NULL));
    Model* Trashcan = new Model(FileSystem::getPath("../Kitchen/trashcan.obj"),"../Kitchen");
    KitchenItems.push_back(Evidence("STATIC_Trashcan",&Trashcan,NULL));
    
    /*Model* Window = new Model(FileSystem::getPath("../Kitchen/windows.obj"),"../Kitchen");
    KitchenItems.push_back(Evidence("STATIC_Window",&Window,NULL));
    */
    //END KITCHEN
    }


    if(Container[1]){    
    //LIVING ROOM
	Model* EmptyLivingRoom = new Model(FileSystem::getPath("../LivingRoom/EmptyLivingRoom.obj"),"../LivingRoom");
	LivingRoomItems.push_back(Evidence("STATIC_FirstPart",&EmptyLivingRoom,NULL));
	
    	Model* ChessTable = new Model(FileSystem::getPath("../LivingRoom/ChessTable.obj"),"../LivingRoom");
	LivingRoomItems.push_back(Evidence("STATIC_ChessTable",&ChessTable,NULL));

	Model* CouchAnd =  new Model(FileSystem::getPath("../LivingRoom/CouchAnd.obj"),"../LivingRoom");
	LivingRoomItems.push_back(Evidence("STATIC_CouchAnd",&CouchAnd,NULL));
	
		
	Model* DiningTable =  new Model(FileSystem::getPath("../LivingRoom/DiningTable.obj"),"../LivingRoom");
	LivingRoomItems.push_back(Evidence("STATIC_DiningTable",&DiningTable,NULL));
	
	
	/*Model* DiningTable2 =  new Model(FileSystem::getPath("../LivingRoom/DiningTable2.obj"),"../LivingRoom");
	LivingRoomItems.push_back(Evidence("STATIC_DiningTable2",&DiningTable2,NULL));
	*/
	
	Model* DiningTable3 =  new Model(FileSystem::getPath("../LivingRoom/DiningTable3.obj"),"../LivingRoom");
	LivingRoomItems.push_back(Evidence("STATIC_DiningTable3",&DiningTable3,NULL));
	
	Model* FirePlace =  new Model(FileSystem::getPath("../LivingRoom/FirePlace.obj"),"../LivingRoom");
	LivingRoomItems.push_back(Evidence("STATIC_FirePlace",&FirePlace,NULL));
  	 Model* FirePlaceFire =  new Model(FileSystem::getPath("../LivingRoom/FirePlaceFire.obj"),"../LivingRoom");
        LivingRoomItems.push_back(Evidence("STATIC_FirePlaceFire",&FirePlaceFire,NULL));
	
    //END LIVING ROOM
    }

    //END STATIC ITEMS

    //EVIDENCE


    //BedRoom Evidence
    //std::vector<Evidence> BRE;     
        
    //BedRoom Evidence!!!!!!!!!!!!!!!!!!11


    if(Container[0]){
    Model* BloodyAxe = new Model(FileSystem::getPath("../BedRoom/BloodyAxe.obj"),"../BedRoom");
    float BloodyAxePosition[] = {
	-5.81f,
	0.126f,
	-6.05f
    };
    BedRoomItems.push_back(Evidence("BRE_BloodyAxe",&BloodyAxe,BloodyAxePosition));
    }    

    /*
    //SOME DOORS
    Model* DoorToBathRoom = new Model(FileSystem::getPath("../BedRoom/DoorToBathRoom.obj"),"../BedRoom");
    float DoorToBathRoomPosition[]={
	6.6867f,
	1.851f,
	0.815f
    }; 
    *///BathRoomItems.push_back(Evidence("BathRoomDOOR_FINAL",&DoorToBathRoom,DoorToBathRoomPosition/*ContainsInt(RoomOrder,2) ? DoorToBathRoomPosition:NULL*/));
    /*  
    Model* DoorToLivingRoom = new Model(FileSystem::getPath("../BedRoom/DoorToLivingRoom.obj"),"../BedRoom");
    float DoorToLivingRoomPosition[]={
	0.796f,
	1.94f,
	6.368f
    };
    *///LivingRoomItems.push_back(Evidence("LivingRoomDOOR_FINAL",&DoorToLivingRoom,DoorToLivingRoomPosition/*ContainsInt(RoomOrder,1) ? DoorToLivingRoomPosition:NULL*/));
    //END SOME DOORS
    
    if(Container[0]){

    Model* Phone = new Model(FileSystem::getPath("../BedRoom/Phone.obj"),"../BedRoom");
    float PhonePosition[]={
	-0.426f,
	1.507f,
	-4.876f
    };

    BedRoomItems.push_back(Evidence("BRE_Phone",&Phone,PhonePosition));
    Model * RightShoe = new Model(FileSystem::getPath("../BedRoom/RightShoe.obj"),"../BedRoom");
     /*float RightShoePosition[]={
	-0.019f,
	0.04f,
	-3.369f	
    };
    */
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
    BedRoomItems.push_back(Evidence("FINAL",&RightShoe,ShoePosition)); 

    Model* Step = new Model(FileSystem::getPath("../BedRoom/Step.obj"),"../BedRoom");
    float StepPosition[]={
	3.877f,
	-0.136f,
	-4.16f
    };
    BedRoomItems.push_back(Evidence("FINAL",&Step,StepPosition));	 
    Model* TvGun = new Model(FileSystem::getPath("../BedRoom/TvGun.obj"),"../BedRoom");
    float TvGunPosition[] = {
	6.145f,
	1.45f,
	5.377f
    };
    BedRoomItems.push_back(Evidence("BRE_TvGun",&TvGun,TvGunPosition));
    //Rooms.push_back(Room(BedRoomVertices,BedRoomItems));
    //END BEDROOM EVIDENCE!!!
    }

    if(Container[2]){
    //BATHROOM EVIDENCE
    
    Model* BloodRug = new Model(FileSystem::getPath("../BathRoom/rug.obj"),"../BathRoom");
    float BloodRugPosition[]={
	13.19f,
	0.1f,
	0.11f
    };
	
    BathRoomItems.push_back(Evidence("FINAL",&BloodRug,BloodRugPosition));
    
    Model* Candle = new Model(FileSystem::getPath("../BathRoom/candle.obj"),"../BathRoom");
    float CandlePosition[] = {
	18.933f,
	0.207,
	-5.768f
    };
    BathRoomItems.push_back(Evidence("BRE_Candle",&Candle,CandlePosition));
    
    Model* Chain = new Model(FileSystem::getPath("../BathRoom/olympicChain.obj"),"../BathRoom");
    float ChainPosition[] = {
	18.933f,
	0.206,
	-5.768f
    };
    
    BathRoomItems.push_back(Evidence("BRE_Chain",&Chain,NULL));
   
    Model* Needle = new Model(FileSystem::getPath("../BathRoom/needle.obj"),"../BathRoom");
    float NeedlePosition[] = {
	13.719f,
	0.743f,
	4.78f
    };
    BathRoomItems.push_back(Evidence("BRE_Needle",&Needle,NeedlePosition));
      

   Model* Key = new Model(FileSystem::getPath("../BathRoom/key.obj"),"../BathRoom");
    float KeyPosition[] = {
	18.933f,
	0.206,
	-5.768f
    };
    BathRoomItems.push_back(Evidence("BRE_Key",&Key,KeyPosition));
    
    Model* Hairbrush = new Model(FileSystem::getPath("../BathRoom/hairbrush.obj"),"../BathRoom");
    float HairbrushPosition[] = {
	13.406f,
	3.416,
	-5.69f
    };
    BathRoomItems.push_back(Evidence("BRE_Hairbrush",&Hairbrush,HairbrushPosition));
    
    Model* Cup = new Model(FileSystem::getPath("../BathRoom/cup.obj"),"../BathRoom");
    float CupPosition[] = {
	17.77f,
	3.165f,
	-2.664f
    };
    BathRoomItems.push_back(Evidence("BRE_Cup",&Cup,CupPosition));
    Model* Mirror = new Model(FileSystem::getPath("../BathRoom/bathroomMirror.obj"),"../BathRoom");
    float MirrorPosition[] = {
	19.346f,
	4.870f,
	0.2945f
    };
    BathRoomItems.push_back(Evidence("FINAL",&Mirror,MirrorPosition));
    //END BATHROOM EVIDENCE!!!
    }


    if(Container[3]){
    //KITCHEN EVIDENCE
    
    Model* TeddyBear = new Model(FileSystem::getPath("../Kitchen/bear.obj"),"../Kitchen");
    float TeddyBearPosition[] = {
	-14.128,
	6.105,
	14.06
    };
    KitchenItems.push_back(Evidence("BRE_TeddyBear",&TeddyBear,TeddyBearPosition));
    /*
    Model* Ball = new Model(FileSystem::getPath("../Kitchen/ball.obj"),"../Kitchen");
    float BallPosition[] = {
	7.38f,
	-5.99f,
	1.34f
    };
    KitchenItems.push_back(Evidence("BRE_Ball",&Ball,BallPosition));
    */
    Model* Rope = new Model(FileSystem::getPath("../Kitchen/rope.obj"),"../Kitchen");
    float RopePosition[] = {
	-13.12f,
	2.085f,
	22.29f
    };
    KitchenItems.push_back(Evidence("BRE_Rope",&Rope,RopePosition));
    
    Model* Bottle = new Model(FileSystem::getPath("../Kitchen/cleaner.obj"),"../Kitchen");
    float BottlePosition[] = {
	-19.356f,
	0.489f,
	20.499f
    };
    KitchenItems.push_back(Evidence("BRE_Bottle",&Bottle,BottlePosition));
    
    /*Model* Knife = new Model(FileSystem::getPath("../Kitchen/knife.obj"),"../Kitchen");
    float KnifePosition[] = {
	-14.84f,
	1.5079,
	14.7f
    };
    */

    /*KitchenItems.push_back(Evidence("BRE_Knife",&Knife,KnifePosition));
    */
    Model* FootPrints = new Model(FileSystem::getPath("../Kitchen/footprints.obj"),"../Kitchen");
    float FootPrintsPosition[] = {
	-18.33f,
	-0.017f,
	12.593f
    };
    KitchenItems.push_back(Evidence("FINAL",&FootPrints,FootPrintsPosition));
    
    //END KITCHEN EVIDENCE
    }

    //LIVING ROOM EVIDENCE
    //SOME DOOR
    /*Model* DoorToKitchen = new Model(FileSystem::getPath("../LivingRoom/DoorToKitchen.obj"),"../LivingRoom");
    float DoorToKitchenPosition[] = {
        -7.212f,
        1.992f,
        12.07f
    };
    */
//KitchenItems.push_back(Evidence("KitchenDOOR_FINAL",&DoorToKitchen,//DoorToKitchenPosition/*ContainsInt(RoomOrder,3) ? DoorToKitchenPosition:NULL*/));	
    //END SOME DOOR
   if(Container[1]){
	 Model* LivingRoomBones = new Model(FileSystem::getPath("../LivingRoom/LivingRoomBones.obj"),"../LivingRoom");
    float LivingRoomBonesPosition[]={
        1.13f,
        -0.025,
        22.26f
    };
	LivingRoomItems.push_back(Evidence("LRE_Bones",&LivingRoomBones,LivingRoomBonesPosition));
	Model* LivingRoomFoot = new Model(FileSystem::getPath("../LivingRoom/LivingRoomFoot.obj"),"../LivingRoom");
    float LivingRoomFootPosition[]={
        -5.65f,
        -0.1124f,
        14.361f
    };
        LivingRoomItems.push_back(Evidence("LRE_Foot",&LivingRoomFoot,LivingRoomFootPosition));
    //UPDATE VERTICES
    Model* LivingRoomPlate = new Model(FileSystem::getPath("../LivingRoom/LivingRoomPlate.obj"),"../LivingRoom");
    float LivingRoomPlatePosition[]={
        -1.15f,
        -0.06f,
        15.8f
    };
        LivingRoomItems.push_back(Evidence("LRE_Plate",&LivingRoomPlate,LivingRoomPlatePosition));
    Model* LivingRoomWrench = new Model(FileSystem::getPath("../LivingRoom/LivingRoomWrench.obj"),"../LivingRoom");
    float LivingRoomWrenchPosition[]={
        -3.03f,
        -0.066f,
        20.65f
    };
        LivingRoomItems.push_back(Evidence("LRE_Wrench",&LivingRoomWrench,LivingRoomWrenchPosition));
 
	//END LIVING ROOM EVIDENCE
    }
    //END EVIDENCE

    //MOVABLE ITEMS
    /*Model* Fan = new Model(FileSystem::getPath("../BedRoom/fan.obj"),"../BedRoom");
    Model* KitchenWaterDroplet = new Model(FileSystem::getPath("../Kitchen/KitchenWaterDroplet.obj"),"../Kitchen");
    */
    //BedRoomItems.push_back(Evidence("FAN",&Fan,NULL)); 
    //Model* Phone2 = new Model(FileSystem::getPath("../BedRoom/Phone.obj"),"../BedRoom");
    //BedRomItems.push_back(Evidence("
    //END MOVABLE ITEMS
   
    //Model Room
  
    Rooms.push_back(Room(BedRoomVertices,BedRoomItems));  

//<<<<<<< HEAD
    Rooms.push_back(Room(LivingRoomVertices,LivingRoomItems));
    Rooms.push_back(Room(BathRoomVertices,BathRoomItems));
    Rooms.push_back(Room(KitchenVertices,KitchenItems));    
//=======
    //Rooms.push_back(Room(BathRoomVertices,BathRoomItems));
    //Rooms.push_back(Room(KitchenVertices,KitchenItems));    
//>>>>>>> c6abcd87be8673c5af71129e04aeb22205e71717
    // draw in wireframe
    //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    
    // render loop
    
    // -----------

    Rooms[0].Occupied = true;
    //Because check if room is opened lol
    //Rooms[1].Open = true;
    
    printf("\n WITHIN BOUNDS:%d",WithinBounds(camera.Position));

    //WithinBounds(camera.Position);
    //return 0;  

    Model* YouWin = new Model(FileSystem::getPath("../BedRoom/YOUWIN.obj"),"../BedRoom");
    float YouWinStart[]=
{
0.0f,0.0f,0.0f
};
    float YouWinEnd[]={
	0.0f,-1000000000.0f,0.0f
    };    
  
    Animated * YOUWIN = NULL;    

    //Animated * FAN = NULL;

    End FAN = CreateEnd(glm::vec3(0.0f,0.0f,0.0f));
    End KWD = CreateEnd(glm::vec3(0.0f,0.0f,0.0f));
    float TotalRot = 0.0f;
    //Animated YOUWIN(YouWinStart,YouWinEnd,10,0);
  
    std::vector<End**> AllEvidences = GetE(Rooms);
    
    //if(Args[1]>=0) 
    //AllEvidences = ShuffleEvidences(AllEvidences);
 
    //bool Already[23];

    printf("\n MADE IT");


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
    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Find All Hidden Items!", NULL, NULL);
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
    Shader endShader("shader.vs", "shader.frag");
    Shader fanShader("shader.vs", "shader.frag");
    Shader kwdShader("shader.vs", "shader.frag"); 
    // load/store models
    // -----------


    //return 0;
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
         
	//END STATIC ITEMS

 	//EVIDENCE
 	//bool NoEvidence = true;
	int Count = 0;
        int WithinIDX = -1;
	int ECount = 0;
	for(int i=0; i<Rooms.size();i+=1){
	    if(Rooms[i].Occupied){
		WithinIDX = i;
	    }
	    //printf("\nIDX:%d",i);
	    for(int j=0; j<Rooms[i].Evidences.size();j+=1){
		/*if(Rooms[i].Evidences[j].Vertices!=NULL){
			ECount+=1;
		}
		*/
		//printf("\n SOME EVIDENCE");
		Model** M = NULL;
		
		if(Rooms[i].Evidences[j].M!=NULL)
			M = (Rooms[i].Evidences[j].M);
	
		bool SHUFF = false;	
  	        if(M!=NULL)
		    if(*M!=NULL){
			//NoEvidence = false;   
			if(Rooms[i].Evidences[j].Vertices!=NULL){
					Count+=1;
			}	
	    			End * EE = Rooms[i].Evidences[j].HPVM;
				float* Vertices = Rooms[i].Evidences[j].Vertices;
					
				if(EE!=NULL){
				//Negative to origin
				//EE->Model = glm::translate(EE->Model,-glm::vec3(Vertices[0],0.0f,Vertices[2]));
				//Translate to HPVM->Here
				//EE->Model=glm::translate(EE->Model,EE->Here);
				ourShader.setMat4("model",EE->Model);
				}
				else
				ourShader.setMat4("model",model);
				
				if(M!=NULL)
				if(*M!=NULL)
				//if(**M!=NULL)
				(*M)->Draw(ourShader);
	    		
		}
		//}
	    	/*if(Rooms[i].Evidences[j].Vertices!=NULL){
                        ECount+=1;
                }
		*/
		}	   
	}

	if(Count<=0){
		printf("\n !!!!!!!!!!!!!!!!!!!YOU WIN!!!!!!!!!!!!!!!!!\n");
		endShader.use();
		// view/projection transformations
	/*
   	glm::vec3 Here(camera.Position[0], camera.Position[1], camera.Position[2]-2.0f);

        glm::mat4 projection2 = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);

        glm::mat4 view2 = camera.GetViewMatrix();

        endShader.setMat4("projection", projection2);
        endShader.setMat4("view", view2);

        // render the loaded model
        glm::mat4 model2 = glm::mat4(1.0f);
        model2 = glm::translate(model2, Here); // translate it down so it's at the center of the scene	   	
	model2 = glm::rotate(model2,glm::radians(90.0f),glm::vec3(0.0f,1.0f,0.0f));	
	*/
	/*
	//MyLookY
	glm::mat4 model3 = glm::translate(model2,camera.Position);
	model3 = glm::rotate(model3,glm::radians(MyLookY*(3.14159f/180.0f) ),glm::vec3(0.0f,1.0f,0.0f));
	model3 = glm::translate(model2,-camera.Position);
	*/
        /*//MyLookX
	model2 = glm::translate(model2,Here);
	model2 = glm::rotate(model2,glm::radians(MyLookX),glm::vec3(1.0f,0.0f,0.0f));
	model2 = glm::translate(model2,-Here);
	*/
	//model = glm::scale(model, glm::vec3(0.2f, 0.2f, 0.2f));       // it's a bit too big for our scene, so scale it down
        //endShader.setMat4("model", model2);
	
		if(YOUWIN==NULL){
			YOUWIN=new Animated(YouWinStart,YouWinEnd,10000,0);
		}	
		else{
			if(YOUWIN->Update()){
				printf("YOUUUUUUUUUUUUUUUUUUUUUUUUUUUU");
				break;
			}
		}

		std::vector<End> Ends = EndWords(3.0f);
		
		for(int i=0; i<Ends.size();i+=1){	
		endShader.setMat4("projection", Ends[i].Projection);
	        endShader.setMat4("view", Ends[i].View);
		endShader.setMat4("model", Ends[i].Model);
		YouWin->Draw(endShader);
		}
		
		//YOUWIN->draw();
		
		//break;
	}
	else{
		printf("\n Evidences left:%d",Count);
	}

	//Shader.use();
	//ANIMATIONS

	//Update Positions...
	KitchenDroplet.Update();
	//End Update Positions
	
	//Shader Draw
	//FAN
	fanShader.use();
	fanShader.setMat4("projection", projection);
        fanShader.setMat4("view", view);
	FAN.Model= glm::translate(FAN.Model,glm::vec3(-1.037,6.08,-1.022));
        FAN.Model = glm::rotate(FAN.Model,glm::radians(deltaTime*50.0f),glm::vec3(0.0f,1.0f,0.0f));
	FAN.Model= glm::translate(FAN.Model,-glm::vec3(-1.037,6.08,-1.022));
	fanShader.setMat4("model", FAN.Model);	
	//Fan->Draw(fanShader);
	//END FAN

	//DROPLET
	kwdShader.use();
	
	kwdShader.setMat4("projection", projection);
        kwdShader.setMat4("view", view);	
	
	//TEMPLATE
	deltaTime = deltaTime / 1000;
	
	//printf("\n Delta * 50.0f: %f",deltaTime*50.0f);	
	
	if(deltaTime*50.0f<=1.0 && TotalRot<=1.0){
	KWD.Model= glm::translate(KWD.Model,glm::vec3(1219.0f,-47.0f,-25.22f));
        KWD.Model = glm::rotate(KWD.Model,glm::radians(deltaTime*50.0f),glm::vec3(1.0f,0.0f,1.0f));
        KWD.Model= glm::translate(KWD.Model,-glm::vec3(1219.0f,-47.0f,-25.22f));
	TotalRot+=(deltaTime*50.0f);
	}
	else{
		KWD.Model= glm::translate(KWD.Model,glm::vec3(1219.0f,-47.0f,-25.22f));
        KWD.Model = glm::rotate(KWD.Model,glm::radians(-TotalRot),glm::vec3(1.0f,0.0f,1.0f));
        KWD.Model= glm::translate(KWD.Model,-glm::vec3(1219.0f,-47.0f,-25.22f));	
	TotalRot = 0.0f;
	}
	
	/*KWD.Model = glm::translate(KWD.Model,
	glm::vec3(
	KitchenDroplet.Positions[KitchenDroplet.TimeCounter][0]
	,
	KitchenDroplet.Positions[KitchenDroplet.TimeCounter][2]
	,
	-KitchenDroplet.Positions[KitchenDroplet.TimeCounter][1]
	));
	*/
	//WD.Model = glm::scale(KWD.Model, glm::vec3(2.0f, 2.0f, 2.0f));       // it's a bit too big for our scene, so scale it down
	
	kwdShader.setMat4("model",KWD.Model);
  	
	//KitchenWaterDroplet->Draw(kwdShader);
	
	//Positions[TimeCounter] is next  positions in 3d!!!
	//END DROPLET
	//End Shader Draw

	//END ANIMATIONS

	//BEDROOM EVIDENCE
	
	//
        // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
        // -------------------------------------------------------------------------------

        glfwSwapBuffers(window);
        //glfwPollEvents();
    	glfwWaitEvents(); 
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
    /*if(StableMove){
	camera.ProcessMouseMovement(0.0f,-MyLookX);
    }
    */
    if(key==GLFW_KEY_UP && (action==GLFW_PRESS || Keys[key]) && !CantGoAgain[0])
    {
      Keys[key]=true;
      //printf("Pressed Up\n"); 
      if(PreProcessKeyboard(FORWARD,deltaTime)){
      //camera.ProcessKeyboard(FORWARD,PreDelta);
      PreDelta = 0.0f;
      //UpdateGoAgain(0);
      MyZ-=1;
      }
      else
	UpdateGoAgain(0);

      Keys[key]=false;
    }
    else if(key==GLFW_KEY_UP){
      Keys[key]=false;
      //printf("End Press Up\n");
    }

    if(key==GLFW_KEY_DOWN && (action==GLFW_PRESS||Keys[key]) && !CantGoAgain[1])
    {
      Keys[key]=true;     
      //printf("Pressed Down\n");
      if(PreProcessKeyboard(BACKWARD,deltaTime)){
      //camera.ProcessKeyboard(BACKWARD,PreDelta);
      PreDelta = 0.0f;
      MyZ+=1;
      }
      else
	UpdateGoAgain(1);

      Keys[key]=false;
    }
    else if(key==GLFW_KEY_DOWN){
      Keys[key]=false;
      //printf("End Press Down\n");
    }

    if(key==GLFW_KEY_LEFT && (action==GLFW_PRESS || Keys[key]) && !CantGoAgain[2])
    {
      Keys[key]=true;
      //printf("Pressed Left\n");
      if(PreProcessKeyboard(LEFT,PreDelta)){
      //camera.ProcessKeyboard(LEFT,deltaTime);
       PreDelta = 0.0f;
      MyX-=1;
      }
      else
	UpdateGoAgain(2);

      Keys[key]=false;
    }
    else if(key==GLFW_KEY_LEFT){
      Keys[key]=false;
      //printf("End Press Left\n");
    
    }

    if(key==GLFW_KEY_RIGHT && (action==GLFW_PRESS||Keys[key]) && !CantGoAgain[3])
    {
      Keys[key]=true;
      //printf("Pressed Right\n");
      if(PreProcessKeyboard(RIGHT,PreDelta)){
      //camera.ProcessKeyboard(RIGHT,);
       PreDelta = 0.0f;

      MyX+=1;
      }
	else
	UpdateGoAgain(3);
      Keys[key]=false;
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
      camera.ProcessMouseMovement(0.0f,22.5f); 
      MyLookX+=22.5f;
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

      camera.ProcessMouseMovement(0.0f,-22.5f);
      MyLookX-=22.5f;
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

       float r = static_cast <float> (rand()) / static_cast <float> (RAND_MAX);

      camera.ProcessMouseMovement(22.0f + r,0.0f);

      MyLookY+=22.0f+r;
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

       float r = static_cast <float> (rand()) / static_cast <float> (RAND_MAX);

      camera.ProcessMouseMovement(-23.0f+r,0.0f);

      MyLookY+=(-23.0f+r);
    }
    else if(key==GLFW_KEY_A){
      Keys[key]=false;
      //printf("End Press LOOK LEFT\n");
    }
    /*
    if(StableMove){
        camera.ProcessMouseMovement(0.0f,MyLookX);
    }
    */
   //printf("\nCam Position:%f,%f,%f",camera.Position[0],camera.Position[1],camera.Position[2]);
    /*if(StableMove){
        printf("\n STABLE 2 \n");
        camera.ProcessMouseMovement(0.0f,MyLookX);
        }
	*/
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
    if(yoffset>0){
    if(PreProcessKeyboard(UP,deltaTime)){
    //camera.ProcessKeyboard(UP,deltaTime);
    
    }
	else
	UpdateGoAgain(4);
    }
    else
    if(PreProcessKeyboard(DOWN,deltaTime)){
    //camera.ProcessKeyboard(DOWN,deltaTime);
    
    }
	else
	UpdateGoAgain(5);
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
