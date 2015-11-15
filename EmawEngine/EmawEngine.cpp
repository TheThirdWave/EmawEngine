// EMAW_ENGINE.cpp : Defines the entry point for the application.
//

#include "stdafx.h"
#include "EmawEngine.h"
#include "Texture.h"
#include "FrameCounter.h"
#include "Test.h"
#include "AssetManager.h"
#include "NetworkManager.h"
#include "GameNode.h"
#include "ModelNode.h"
#include "gdiPrimerNode.h"
#include "InputManager.h"
#include "VoxelMap.h"
#include "AudioEasyAccess.h"
#include <DirectXMath.h>
#include "VoxelCollision.h" //TEMP
#include "CollisionManager.h"
#include <string>
#define MAX_LOADSTRING 100

// Global Variables:
HINSTANCE hInst;								// current instance
HWND hWnd;										// window handle
TCHAR szTitle[MAX_LOADSTRING];					// The title bar text
TCHAR szWindowClass[MAX_LOADSTRING];			// the main window class name
GraphicsDeviceInterface gdi;					// the Graphics Device Inteface
WindowSize wind;								// window size object

// Forward declarations of functions included in this code module:
ATOM				MyRegisterClass(HINSTANCE hInstance);
BOOL				InitInstance(HINSTANCE, int);
LRESULT CALLBACK	WndProc(HWND, UINT, WPARAM, LPARAM);

int APIENTRY _tWinMain(_In_ HINSTANCE hInstance,
	_In_opt_ HINSTANCE hPrevInstance,
	_In_ LPTSTR    lpCmdLine,
	_In_ int       nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

	// TODO: Place code here.
	MSG msg;

	// Initialize global strings
	LoadString(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
	LoadString(hInstance, IDC_EMAW_ENGINE, szWindowClass, MAX_LOADSTRING);
	MyRegisterClass(hInstance);

	// Perform application initialization:
	if (!InitInstance(hInstance, nCmdShow))
	{
		return FALSE;
	}

	// Perform graphics initialization
	if (!gdi.Initialize(hWnd, &wind))
	{
		return FALSE;
	}

	// Perform frame count initialization
	FrameCounter fc;
	unsigned int fps = 0;

	// Perform AssetManager initialization
	AssetManager* assetManager = AssetManager::getInstance();
	assetManager->setGraphicsDevice(&gdi);

	// Perform InputManager initialization
	InputManager* inputManager = InputManager::getInstance();
	inputManager->registerWindow(hWnd);

	//Perform sound initialization
	AudioManager* am = AudioManager::getInstance();
	(AudioRenderer::Instance())->setSoundSystem(am);
	//AudioEasyAccess::getInstance()->playMusic("background", "music/Tictac_-_Estrade.mp3");

	// Network Initialization
	NetworkManager* networkManager = NetworkManager::getInstance();
	networkManager->connect();

	// make mouse invisible
	ShowCursor(false);

	bool paused = false;

	// TEST CODE!!!
	// =========================================================================
	VoxelMap* worldGenerator = new VoxelMap("testmap","blarghggfkjnle", 7);

	GameNode* root = new GameNode();
	root->setGraphicsDeviceInterface(&gdi);
	gdiPrimerNode* voxelPrimer = new gdiPrimerNode();
	voxelPrimer->setGraphicsDeviceInterface(&gdi);
	voxelPrimer->setShader((ShaderAsset *)assetManager->load("VoxShader.geo"));
	root->addChild(voxelPrimer);
	gdiPrimerNode* modelPrimer = new gdiPrimerNode();
	modelPrimer->setGraphicsDeviceInterface(&gdi);
	modelPrimer->setShader((ShaderAsset *)assetManager->load("Shaders.col"));
	root->addChild(modelPrimer);
	bool renderVoxels = false;
	if (renderVoxels) {
		VoxelChunkNode* world[7][7];
		for (int i = 0; i < worldGenerator->map.size; i++)
		{
			for (int j = 0; j < worldGenerator->map.size; j++)
			{
				world[i][j] = new VoxelChunkNode(i, j);
				world[i][j]->setGraphicsDeviceInterface(&gdi);
				world[i][j]->loadTextures();
				world[i][j]->loadChunkBuffer(worldGenerator);
				voxelPrimer->addChild(world[i][j]);
			}
		}
	}

	bool do_cube = false;
	bool do_pill = false;
	bool do_cat = true;
	bool do_ship = false;
	bool do_teapot = true;
	if (do_cube) {
		Model* cube = new Model();
		cube->load("models/obj-models/cube-tex.obj");
		cube->LoadTexture(gdi.m_Device, "textures/x.png");

		ModelNode* cubeNode = new ModelNode(cube);
		cubeNode->setGraphicsDeviceInterface(&gdi);

//		modelPrimer->addChild(cubeNode);
	}
	if (do_pill) {
		Model* cap = new Model();
		cap->load("models/obj-models/capsule.obj");
		cap->LoadTexture(gdi.m_Device, "textures/capsule0.jpg");

		ModelNode* capNode = new ModelNode(cap);
		capNode->setGraphicsDeviceInterface(&gdi);

//		modelPrimer->addChild(capNode);
	}
	if (do_cat) {
		Model* cat = new Model();
		cat->load("models/obj-models/cat.obj");
		cat->LoadTexture(gdi.m_Device, "textures/cat-flipped.png");

		ModelNode* catNode = new ModelNode(cat);
		catNode->setGraphicsDeviceInterface(&gdi);

		modelPrimer->addChild(catNode);
	}
	if (do_ship) {
		Model* ship = new Model();
		ship->load("models/obj-models/SpaceShip_FULL.obj");
		ship->LoadTexture(gdi.m_Device, "textures/cat-flipped.png");

		ModelNode* shipNode = new ModelNode(ship);
		shipNode->setGraphicsDeviceInterface(&gdi);

		modelPrimer->addChild(shipNode);
	}
	if (do_teapot)
	{
		Model* teapot = new Model();
		teapot->load("models/obj-models/utah-teapot.obj");
		teapot->LoadTexture(gdi.m_Device, "textures/cat-flipped.png");

		ModelNode* teapotNode = new ModelNode(teapot);
		teapotNode->setGraphicsDeviceInterface(&gdi);
		teapotNode->setPosition(0, 0, 1.0f);

		modelPrimer->addChild(teapotNode);
	}
	//Controls the camera, WASD to move along the xz plane, Space and Ctrl to move up and down.
	Player* player = new Player(&gdi, worldGenerator);

	modelPrimer->addChild(player->node);

	
	//CollisionManager::getInstance()->addMovingCollidable(player);
	//CollisionManager::getInstance()->loadLevel(1);

	gdi.SetSceneGraphRoot(root);
	gdi.SetCamera(player->getCamera());

	Transform* identity = new Transform();

	// =========================================================================
	// TEST CODE!!!



	//Main game loop:
	while(true)
	{
		
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);

			if (msg.message == WM_QUIT) {
				break;
			}
			else {
				switch (msg.message) {
					// Keyboard messages
					case WM_KEYDOWN:
						inputManager->handleKeyDownMessage(msg.wParam);
						break;
					case WM_KEYUP:
						inputManager->handleKeyUpMessage(msg.wParam);
						break;
					// Mouse button messages
					case WM_LBUTTONDOWN:
						inputManager->handleMouseDownMessage(msg.wParam, MOUSEBUTTON_LEFT);
						break;
					case WM_LBUTTONUP:
						inputManager->handleMouseUpMessage(msg.wParam, MOUSEBUTTON_LEFT);
						break;
					case WM_MBUTTONDOWN:
						inputManager->handleMouseDownMessage(msg.wParam, MOUSEBUTTON_MID);
						break;
					case WM_MBUTTONUP:
						inputManager->handleMouseUpMessage(msg.wParam, MOUSEBUTTON_MID);
						break;
					case WM_RBUTTONDOWN:
						inputManager->handleMouseDownMessage(msg.wParam, MOUSEBUTTON_RIGHT);
						break;
					case WM_RBUTTONUP:
						inputManager->handleMouseUpMessage(msg.wParam, MOUSEBUTTON_RIGHT);
						break;
					// Mouse move messages
					case WM_MOUSEMOVE:
						inputManager->handleMouseMoveMessage(msg.lParam);
				}
			}
		}
		else
		{
			if (inputManager->keyPressed(Esc)) {
				paused = !paused;
				if (paused) {
					ShowCursor(true);
				}
				else {
					ShowCursor(false);
				}
			}
			if (paused) {
				
			} else {
				//base->resetTransformMatrix();
				player->updatePlayer(hWnd, fc.GetElapsedFrameTime());
				root->update(identity->getTransformMatrix());
				
				//TEST CODE FOR NETWORK
				if (inputManager->keyPressed(W))
					networkManager->addInput("wPressed");
				networkManager->update(10);

				if (renderVoxels)
				{
					CollisionManager::getInstance()->checkCollisions();
					CollisionManager::getInstance()->checkVMapCollision(&player->box, worldGenerator);
				}
				//base2->rotateX(0.0005);
				//base->rotateY(0.0005);
			}


			// TODO: Update
			gdi.NextFrame();

			// Update frame counter
			fc.Update();
			Vector * v = player->getPosition();
			Vector * v2 = player->getLastPosition();
			string tes = std::to_string(v->x) + " " + std::to_string(v->y) + " " + std::to_string(v->z) + " " + std::to_string(v2->x) + " " + std::to_string(v2->y) + " " + std::to_string(v2->z);
			wstring te = std::wstring(tes.begin(), tes.end());
			wstring test = fc.GetFps();
			SetWindowText(hWnd, (LPCWSTR)&te[0]);

			// Update the input
			inputManager->update();
		}
	}

	// Perform graphics cleanup
	gdi.Shutdown();

	return (int) msg.wParam;
}



//
//  FUNCTION: MyRegisterClass()
//
//  PURPOSE: Registers the window class.
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
	WNDCLASSEX wcex;

	wcex.cbSize = sizeof(WNDCLASSEX);

	wcex.style			= CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc	= WndProc;
	wcex.cbClsExtra		= 0;
	wcex.cbWndExtra		= 0;
	wcex.hInstance		= hInstance;
	wcex.hIcon			= LoadIcon(hInstance, MAKEINTRESOURCE(IDI_EMAW_ENGINE));
	wcex.hCursor		= LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground	= (HBRUSH)(COLOR_WINDOW+1);
	wcex.lpszMenuName	= MAKEINTRESOURCE(IDC_EMAW_ENGINE);
	wcex.lpszClassName	= szWindowClass;
	wcex.hIconSm		= LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

	return RegisterClassEx(&wcex);
}

//
//   FUNCTION: InitInstance(HINSTANCE, int)
//
//   PURPOSE: Saves instance handle and creates main window
//
//   COMMENTS:
//
//        In this function, we save the instance handle in a global variable and
//        create and display the main program window.
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   hInst = hInstance; // Store instance handle in our global variable

   // using windows size object to create correctly sized window
   // removed thick frame so window is not resizable
   RECT wr = { 0, 0, wind.getWidth(), wind.getHeight() };
   AdjustWindowRect(&wr, WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX, FALSE);

   hWnd = CreateWindow(
	   szWindowClass,
	   szTitle,
	   WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX,
	   CW_USEDEFAULT,
	   0,
	   wr.right - wr.left,
	   wr.bottom - wr.top,
	   NULL,
	   NULL,
	   hInstance,
	   NULL);

   if (!hWnd)
   {
      return FALSE;
   }

   ShowWindow(hWnd, nCmdShow);
   UpdateWindow(hWnd);

   return TRUE;
}

//
//  FUNCTION: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  PURPOSE:  Processes messages for the main window.
//
//  WM_COMMAND	- process the application menu
//  WM_PAINT	- Paint the main window
//  WM_DESTROY	- post a quit message and return
//	WM_KEYDOWN	- process keydown events
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	int wmId, wmEvent;
	PAINTSTRUCT ps;
	HDC hdc;

	switch (message)
	{
	case WM_COMMAND:
		wmId    = LOWORD(wParam);
		wmEvent = HIWORD(wParam);
		// Parse the menu selections:
		switch (wmId)
		{
		case IDM_EXIT:
			DestroyWindow(hWnd);
			break;
		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
		}
		break;
	case WM_PAINT:
		hdc = BeginPaint(hWnd, &ps);
		// TODO: Add any drawing code here...
		EndPaint(hWnd, &ps);
		break;
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}
