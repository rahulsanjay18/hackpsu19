
#include <string>
#include <sstream>

#include <Urho3D/Core/CoreEvents.h>
#include <Urho3D/Engine/Application.h>
#include <Urho3D/Engine/Engine.h>
#include <Urho3D/Input/Input.h>
#include <Urho3D/Input/InputEvents.h>
#include <Urho3D/Resource/ResourceCache.h>
#include <Urho3D/Resource/XMLFile.h>
#include <Urho3D/IO/Log.h>
#include <Urho3D/UI/UI.h>
#include <Urho3D/UI/Text.h>
#include <Urho3D/UI/Font.h>
#include <Urho3D/UI/Button.h>
#include <Urho3D/UI/UIEvents.h>
#include <Urho3D/Scene/Scene.h>
#include <Urho3D/Scene/SceneEvents.h>
#include <Urho3D/Graphics/Graphics.h>
#include <Urho3D/Graphics/Camera.h>
#include <Urho3D/Graphics/Geometry.h>
#include <Urho3D/Graphics/Renderer.h>
#include <Urho3D/Graphics/DebugRenderer.h>
#include <Urho3D/Graphics/Octree.h>
#include <Urho3D/Graphics/Light.h>
#include <Urho3D/Graphics/Model.h>
#include <Urho3D/Graphics/StaticModel.h>
#include <Urho3D/Graphics/StaticModelGroup.h>
#include <Urho3D/Graphics/Material.h>
#include <Urho3D/Graphics/Skybox.h>
#include "GraphGenerator.h"

#include <fstream>
#include <iostream>
#include <string>
#include <stdlib.h>
using namespace Urho3D;

class MyApp : public Application
{
public:
    int framecount_;
    int graphNum = 0; //Graphnumber custom
    int cameraRadius = 3;
    float time_=0;
    SharedPtr<Text> text_;
    SharedPtr<Scene> scene_;
    SharedPtr<Node> boxNode_;
	static const int res = 20;
	Node* lines[res];
	StaticModel* objects[res];
	Node* grid[res*res];
	StaticModel* surface[res*res];
	SharedPtr<Node> cameraNode_;
	ResourceCache* cache;
	SharedPtr<Viewport> viewport[4];
	SharedPtr<Node> cameraNodes[4];
	Camera* cameras[4];
	SharedPtr<Viewport> viewports[4];
	Renderer* renderer;
	Renderer* renderers[4];

    MyApp(Context * context) : Application(context),framecount_(0),time_(0)
    {
    }

    virtual void Setup()
    {
        engineParameters_["FullScreen"]=false;
        engineParameters_["WindowWidth"]=1280;
        engineParameters_["WindowHeight"]=720;
        engineParameters_["WindowResizable"]=true;
    }

	virtual void Start()
	{
		cache = GetSubsystem<ResourceCache>();

		// Let's use the default style that comes with Urho3D.
		GetSubsystem<UI>()->GetRoot()->SetDefaultStyle(cache->GetResource<XMLFile>("UI/DefaultStyle.xml"));

		scene_ = new Scene(context_);
		// Let the scene have an Octree component!
		scene_->CreateComponent<Octree>();
		// Let's add an additional scene component for fun.
		scene_->CreateComponent<DebugRenderer>();
		//Create the sky background
		//Find out how to change the background
		Node* skyNode = scene_->CreateChild("Sky");
		skyNode->SetScale(500.0f); //The scale does not matter
		Skybox* skybox = skyNode->CreateComponent<Skybox>();
		skybox->SetModel(cache->GetResource<Model>("Models/Box.mdl"));
		skybox->SetMaterial(cache->GetResource<Material>("Materials/Skybox.xml"));
		
		// Init grid for surfaces		
		StaticModelGroup* group = new StaticModelGroup(context_);
		for (int i = 0; i < res*res; i++)
		{
			float step = 2.0f / res;
			grid[i] = scene_->CreateChild("Box");
			grid[i]->SetPosition(Vector3(0, 0, 0));
			grid[i]->SetScale(Vector3(step, step, step));
			surface[i] = grid[i]->CreateComponent<StaticModel>();
			surface[i]->SetModel(cache->GetResource<Model>("Models/Box.mdl"));
			surface[i]->SetMaterial(cache->GetResource<Material>("Materials/Stone.xml"));
			surface[i]->SetCastShadows(false);
			group->AddInstanceNode(grid[i]);
		}

		// We need a camera from which the viewport can render.
		cameraNode_ = scene_->CreateChild("Camera");
		Camera* camera = cameraNode_->CreateComponent<Camera>();
		camera->SetFarClip(2000);

		for (int i = 0; i < 4; i++) {
			cameraNodes[i] = scene_->CreateChild("Camera");
			cameras[i] = cameraNodes[i]->CreateComponent<Camera>();
			camera->SetFarClip(2000);
		}

		cameraNodes[0]->Translate(Vector3(0, 0, -cameraRadius));
		cameraNodes[1]->Translate(Vector3(-cameraRadius, 0, 0));
		cameraNodes[2]->Translate(Vector3(0, 0, cameraRadius));
		cameraNodes[3]->Translate(Vector3(cameraRadius, 0, 0));
		
		renderer = GetSubsystem<Renderer>();
		/*Layout of everything(camera, viewports, etc.)
			2
		1		3
			0
		*/
		std::ifstream myfile("/home/pi/Desktop/3dGraph/demo/rect.txt");
		std::string line;
		std::string lines[4];
		//char *results;
		int index = 0;
		int nums[4][4] = {
                	{400, 400, 800, 700},
                        {100, 150, 400, 550},
                        {400, 0, 800, 300},
                        {800, 150, 1100, 550}
                };
		if (myfile.is_open()){
			while(getline(myfile,line)){
				lines[index] = line;
				index++;
			}
			for (int i = 0; i < 4; i++){
                        	char* stuffArr = (char*) lines[i].c_str();
                        	nums[i][0] = atoi(strtok(stuffArr, " "));
                                for (int j = 1; j < 4; j++){
                        		nums[i][j] = atoi(strtok(NULL, " "));
                                }
                        }
			myfile.close();
		}
		IntRect rect0(nums[0][0], nums[0][1], nums[0][2], nums[0][3]);
		IntRect rect1(nums[1][0], nums[1][1], nums[1][2], nums[1][3]);
		IntRect rect2(nums[2][0], nums[2][1], nums[2][2], nums[2][3]);
		IntRect rect3(nums[3][0], nums[3][1], nums[3][2], nums[3][3]);

		viewports[0] = new Viewport(context_, scene_, cameraNodes[0]->GetComponent<Camera>(), rect0);
		viewports[1] = new Viewport(context_, scene_, cameraNodes[1]->GetComponent<Camera>(), rect1);
		viewports[2] = new Viewport(context_, scene_, cameraNodes[2]->GetComponent<Camera>(), rect2);
		viewports[3] = new Viewport(context_, scene_, cameraNodes[3]->GetComponent<Camera>(), rect3);
		renderer->SetViewport(0, viewports[0]);
		renderer->SetViewport(1, viewports[1]);
		renderer->SetViewport(2, viewports[2]);
		renderer->SetViewport(3, viewports[3]);

		{
			Node* lightNode1 = scene_->CreateChild();
			lightNode1->SetDirection(Vector3::FORWARD);
			lightNode1->Yaw(50);     // horizontal
			lightNode1->Pitch(10);   // vertical
			Light* light1 = lightNode1->CreateComponent<Light>();
			light1->SetLightType(LIGHT_DIRECTIONAL);
			light1->SetBrightness(1.6);
			light1->SetColor(Color(1.0, .6, 0.3, 1));
			light1->SetCastShadows(true);
			Node* lightNode2 = scene_->CreateChild();
			lightNode2->SetDirection(Vector3::FORWARD);
			lightNode2->Yaw(-50);     // horizontal
			lightNode2->Pitch(-10);   // vertical
			Light* light2 = lightNode2->CreateComponent<Light>();
			light2->SetLightType(LIGHT_DIRECTIONAL);
			light2->SetBrightness(1.6);
			light2->SetColor(Color(1.0, .6, 0.3, 1));
			light2->SetCastShadows(true);
		}

		float step = 2.0f / res;
		float t = time_;
		for (int z = 0, i = 0; z < res; z++)
		{
			float v = (z + 0.5f) * step - 1.0f;
			for (int x = 0; x < res; x++, i++)
			{
				float u = (x + 0.5f) * step - 1.0f;
				Vector3 nah(0,0,0);
				grid[i]->SetPosition(nah);
			}
		}
		SubscribeToEvent(E_BEGINFRAME, URHO3D_HANDLER(MyApp, HandleBeginFrame));
		SubscribeToEvent(E_KEYDOWN, URHO3D_HANDLER(MyApp, HandleKeyDown));
		SubscribeToEvent(E_UPDATE, URHO3D_HANDLER(MyApp, HandleUpdate));
		SubscribeToEvent(E_POSTUPDATE, URHO3D_HANDLER(MyApp, HandlePostUpdate));
		SubscribeToEvent(E_RENDERUPDATE, URHO3D_HANDLER(MyApp, HandleRenderUpdate));
		SubscribeToEvent(E_POSTRENDERUPDATE, URHO3D_HANDLER(MyApp, HandlePostRenderUpdate));
		SubscribeToEvent(E_ENDFRAME, URHO3D_HANDLER(MyApp, HandleEndFrame));
    }

    virtual void Stop()
    {
    }

    /**
    * Every frame's life must begin somewhere. Here it is.
    */
    void HandleBeginFrame(StringHash eventType,VariantMap& eventData)
    {
        // We really don't have anything useful to do here for this example.
        // Probably shouldn't be subscribing to events we don't care about.
    }

    /**
    * Input from keyboard is handled here. I'm assuming that Input, if
    * available, will be handled before E_UPDATE.
    */
    void HandleKeyDown(StringHash eventType,VariantMap& eventData)
    {
		using namespace KeyDown;
		int key = eventData[P_KEY].GetInt();
		if (key == KEY_ESCAPE)
			engine_->Exit();

		if (key == KEY_TAB)    // toggle mouse cursor when pressing tab
		{
			GetSubsystem<Input>()->SetMouseVisible(!GetSubsystem<Input>()->IsMouseVisible());
			GetSubsystem<Input>()->SetMouseGrabbed(!GetSubsystem<Input>()->IsMouseGrabbed());
		}
		if (key == KEY_I){
			graphNum++;
			graphNum = graphNum % 5;
		}
    }

    /**
    * You can get these events from when ever the user interacts with the UI.
    */
    void HandleClosePressed(StringHash eventType,VariantMap& eventData)
    {
        engine_->Exit();
    }
    /**
    * Your non-rendering logic should be handled here.
    * This could be moving objects, checking collisions and reaction, etc.
    */
    void HandleUpdate(StringHash eventType,VariantMap& eventData)
    {
    	float timeStep=eventData[Update::P_TIMESTEP].GetFloat();
        framecount_++;
        time_+=timeStep;
        // Movement speed as world units per second
        float MOVE_SPEED=10.0f;
        // Mouse sensitivity as degrees per M_PIxel
        const float MOUSE_SENSITIVITY=0.1f;
        Input* input=GetSubsystem<Input>();
        //if(input->GetQualifierDown(1))  // 1 is shift, 2 is ctrl, 4 is alt
        MOVE_SPEED/=10;
		if(input->GetKeyDown(KEY_Q)){
			cameraNodes[0]->Translate(Vector3(0,0,1)*MOVE_SPEED*timeStep); 
            cameraNodes[1]->Translate(Vector3(0,0,1)*MOVE_SPEED*timeStep);
            cameraNodes[2]->Translate(Vector3(0,0,1)*MOVE_SPEED*timeStep);
            cameraNodes[3]->Translate(Vector3(0,0,1)*MOVE_SPEED*timeStep);
		}
		if(input->GetKeyDown(KEY_E)){
			cameraNodes[0]->Translate(Vector3(0,0,-1)*MOVE_SPEED*timeStep); 
            cameraNodes[1]->Translate(Vector3(0,0,-1)*MOVE_SPEED*timeStep);
            cameraNodes[2]->Translate(Vector3(0,0,-1)*MOVE_SPEED*timeStep);
            cameraNodes[3]->Translate(Vector3(0,0,-1)*MOVE_SPEED*timeStep);
		}
        if(input->GetKeyDown(KEY_W)){
    		cameraNodes[0]->SetPosition(-(Quaternion(1,0,0) * -cameraNodes[0]->GetWorldPosition()));
    		cameraNodes[1]->SetPosition(-(Quaternion(0,0,1) * -cameraNodes[1]->GetWorldPosition()));
    		cameraNodes[2]->SetPosition(-(Quaternion(1,0,0) * -cameraNodes[2]->GetWorldPosition()));
    		cameraNodes[3]->SetPosition(-(Quaternion(0,0,1) * -cameraNodes[3]->GetWorldPosition()));
        }
		if(input->GetKeyDown(KEY_S)){
    		cameraNodes[0]->SetPosition(-(Quaternion(-1,0,0) * -cameraNodes[0]->GetWorldPosition()));
    		cameraNodes[1]->SetPosition(-(Quaternion(0,0,-1) * -cameraNodes[1]->GetWorldPosition()));
    		cameraNodes[2]->SetPosition(-(Quaternion(-1,0,0) * -cameraNodes[2]->GetWorldPosition()));
    		cameraNodes[3]->SetPosition(-(Quaternion(0,0,-1) * -cameraNodes[3]->GetWorldPosition()));
		}
		if(input->GetKeyDown(KEY_A)){
    		cameraNodes[0]->SetPosition(-(Quaternion(0,1,0) * -cameraNodes[0]->GetWorldPosition()));
    		cameraNodes[1]->SetPosition(-(Quaternion(0,1,0) * -cameraNodes[1]->GetWorldPosition()));
    		cameraNodes[2]->SetPosition(-(Quaternion(0,1,0) * -cameraNodes[2]->GetWorldPosition()));
    		cameraNodes[3]->SetPosition(-(Quaternion(0,1,0) * -cameraNodes[3]->GetWorldPosition()));
		}
		if(input->GetKeyDown(KEY_D)){
    		cameraNodes[0]->SetPosition(-(Quaternion(0,-1,0) * -cameraNodes[0]->GetWorldPosition()));
    		cameraNodes[1]->SetPosition(-(Quaternion(0,-1,0) * -cameraNodes[1]->GetWorldPosition()));
    		cameraNodes[2]->SetPosition(-(Quaternion(0,-1,0) * -cameraNodes[2]->GetWorldPosition()));
    		cameraNodes[3]->SetPosition(-(Quaternion(0,-1,0) * -cameraNodes[3]->GetWorldPosition()));
		}
		cameraNodes[0]->LookAt(Vector3(0,0,0));
		cameraNodes[1]->LookAt(Vector3(0,0,0));
		cameraNodes[2]->LookAt(Vector3(0,0,0));
		cameraNodes[3]->LookAt(Vector3(0,0,0));
		cameraNodes[1]->Roll(90);
		cameraNodes[2]->Roll(180);
		cameraNodes[3]->Roll(270);
		/* This code treats the camera as a player and rotates around the camera's axis
		   Included incase it is to be used at a later date
		if(input->GetKeyDown(KEY_A)){
			cameraNodes[0]->Rotate(Quaternion(-1,Vector3(0,1,0))*MOVE_SPEED*timeStep);
            cameraNodes[1]->Rotate(Quaternion(-1,Vector3(0,1,0))*MOVE_SPEED*timeStep);
            cameraNodes[2]->Rotate(Quaternion(-1,Vector3(0,1,0))*MOVE_SPEED*timeStep);
            cameraNodes[3]->Rotate(Quaternion(-1,Vector3(0,1,0))*MOVE_SPEED*timeStep);
		}
		if(input->GetKeyDown(KEY_D)){
			cameraNodes[0]->Rotate(Quaternion(1,Vector3(0,1,0))*MOVE_SPEED*timeStep); 
            cameraNodes[1]->Rotate(Quaternion(1,Vector3(0,1,0))*MOVE_SPEED*timeStep);
            cameraNodes[2]->Rotate(Quaternion(1,Vector3(0,1,0))*MOVE_SPEED*timeStep);
            cameraNodes[3]->Rotate(Quaternion(1,Vector3(0,1,0))*MOVE_SPEED*timeStep);
		}*/
		float step = 2.0f / res;
        float t = time_;
        for (int z = 0, i = 0; z < res; z++)
        {
        	float v = (z + 0.5f) * step - 1.0f;
                for (int x = 0; x < res; x++, i++)
                {
                	float u = (x + 0.5f) * step - 1.0f;
			 Vector3 nah(0,0,0);
                         switch(graphNum){
				case 0:
                                     nah = MultiSine2DFunction(u,v,t);
                                     break;
                                case 1:
                                     nah = sphere(u,v,t);
                                     break;
                                case 2:
                                     nah = Sine(u,v,t);
                                     break;
                                case 3:
                                     nah = Torus(u,v,t);
                                     break;
                                case 4:
                                     nah = Ripple(u,v,t);
				     break;
				default:
				     break;
			};
                        grid[i]->SetPosition(nah);
                }
        }
    }
  
    void HandlePostUpdate(StringHash eventType,VariantMap& eventData)
    {
        // We really don't have anything useful to do here for this example.
        // Probably shouldn't be subscribing to events we don't care about.
    }
   
    void HandleRenderUpdate(StringHash eventType, VariantMap & eventData)
    {
        // We really don't have anything useful to do here for this example.
        // Probably shouldn't be subscribing to events we don't care about.
    }
   
    void HandlePostRenderUpdate(StringHash eventType, VariantMap & eventData)
    {
        // We could draw some debuggy looking thing for the octree.
        // scene_->GetComponent<Octree>()->DrawDebugGeometry(true);
    }
   
    void HandleEndFrame(StringHash eventType,VariantMap& eventData)
    {
        // We really don't have anything useful to do here for this example.
        // Probably shouldn't be subscribing to events we don't care about.
    }
};

URHO3D_DEFINE_APPLICATION_MAIN(MyApp)
