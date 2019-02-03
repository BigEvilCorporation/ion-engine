#include "RenderTest.h"

#include <sstream>

RenderTest::RenderTest()
	: ion::framework::Application("RenderTest")
{
	mRenderer = NULL;

	mKeyboard = NULL;
	mMouse = NULL;
	mGamepad = NULL;

	mCameraPitch = 0.0f;
	mCameraYaw = 0.0f;
	mCameraSpeed = 10.0f;
	mMouseSensitivity = 0.005f;

	mLightSin = 0.0f;
	mLightCos = 0.0f;

	mFrameCount = 0;
}

RenderTest::~RenderTest()
{
}

bool RenderTest::Initialise()
{
	std::stringstream windowTitle;
	windowTitle << "ion::engine - build " << ion::sVersion.Major << "." << ion::sVersion.Minor << "." << ion::sVersion.Build;

	//Create resource manager
	mResourceManager = new ion::io::ResourceManager();
	mResourceManager->SetResourceDirectory<ion::render::Texture>("textures");

	//Create window, viewport, renderer, scene, camera and viewport
	mWindow = ion::render::Window::Create(windowTitle.str(), 1280, 720, false);
	mViewport = new ion::render::Viewport(1280, 720, ion::render::Viewport::eOrtho2DNormalised);
	mRenderer = ion::render::Renderer::Create(mWindow->GetDeviceContext());

	//Create camera
	mCamera = new ion::render::Camera();

	//Create input handlers
	mKeyboard = new ion::input::Keyboard();
	mMouse = new ion::input::Mouse();
	mGamepad = new ion::input::Gamepad();

	mQuad = new ion::render::Quad(ion::render::Quad::xz, ion::Vector2(0.5f, 0.5f));
	mBox = new ion::render::Box(ion::Vector3(0.5f, 0.5f, 0.5f), ion::Vector3(0.0f, 0.0f, 0.0f));
	mSphere = new ion::render::Sphere(1.0f, 32, 32);

	//Create and load shaders
#if defined ION_RENDERER_SHADER
	mShaderDefaultV = ion::render::Shader::Create();
	mShaderDefaultF = ion::render::Shader::Create();
	mShaderDefaultV->Load("../shaders/default.cgfx", "VertexProgram", ion::render::Shader::Vertex);
	mShaderDefaultF->Load("../shaders/default.cgfx", "FragmentProgram", ion::render::Shader::Fragment);

	//Get shader params
	mShaderParamWorldMtx = mShaderDefaultV->CreateParamHndl<ion::Matrix4>("gWorldMatrix");
	mShaderParamWorldViewProjMtx = mShaderDefaultV->CreateParamHndl<ion::Matrix4>("gWorldViewProjectionMatrix");
	mShaderParamTexture = mShaderDefaultF->CreateParamHndl<ion::render::Texture>("gDiffuseTexture");
#endif

	//Create and load texture
	mTexture = mResourceManager->GetResource<ion::render::Texture>("placeholder256.ion.texture");

	//Create sprite
	mSprite = new ion::render::Sprite(ion::render::Sprite::eRender2D, ion::Vector2(0.5f, 0.5f), 0.3f, 2, 2, mTexture, *mResourceManager);
	mSprite->SetPosition(ion::Vector3(1.0f, 1.0f, 1.0f));

	//Create sprite animation
	mSpriteAnim = new ion::render::SpriteAnimation(*mSprite);
	mSpriteAnimTrack = new ion::render::AnimationTrackInt();
	mSpriteAnimTrack->InsertKeyframe(ion::render::Keyframe<int>(0, 0));
	mSpriteAnimTrack->InsertKeyframe(ion::render::Keyframe<int>(1, 1));
	mSpriteAnimTrack->InsertKeyframe(ion::render::Keyframe<int>(2, 2));
	mSpriteAnimTrack->InsertKeyframe(ion::render::Keyframe<int>(3, 3));
	mSpriteAnimTrack->InsertKeyframe(ion::render::Keyframe<int>(4, 3));
	mSpriteAnim->SetAnimationTrack(*mSpriteAnimTrack);
	mSpriteAnim->SetLength(4.0f);
	mSpriteAnim->SetPlaybackSpeed(2.0f);
	mSpriteAnim->SetState(ion::render::Animation::ePlaying);

	//Set input cooperative level with renderer window
	mKeyboard->SetCooperativeWindow(ion::input::Keyboard::Exclusive);
	mMouse->SetCooperativeWindow(ion::input::Mouse::Exclusive);

	//Initialise FPS timer
	mStartTicks = ion::time::GetSystemTicks();
	
	return true;
}

void RenderTest::Shutdown()
{
	if(mKeyboard)
		delete mKeyboard;

	if(mMouse)
		delete mMouse;

	if(mGamepad)
		delete mGamepad;

#if defined ION_RENDERER_SHADER
	if(mShaderDefaultV)
		delete mShaderDefaultV;

	if(mShaderDefaultF)
		delete mShaderDefaultF;
#endif

	if(mRenderer)
		delete mRenderer;
}

bool RenderTest::Update(float deltaTime)
{
	mKeyboard->Update();
	mMouse->Update();
	mGamepad->Update();

	//Get state of escape key and gamepad back/select button, for exit
	bool exit = mKeyboard->KeyDown(ion::input::Keycode::ESCAPE);
	exit |= mGamepad->ButtonDown(ion::input::GamepadButtons::SELECT);

	//Create camera move vector from WASD state
	ion::Vector3 cameraMoveVector;
	if(mKeyboard->KeyDown(ion::input::Keycode::W))
		cameraMoveVector.z -= mCameraSpeed * deltaTime;
	if(mKeyboard->KeyDown(ion::input::Keycode::S))
		cameraMoveVector.z += mCameraSpeed * deltaTime;
	if(mKeyboard->KeyDown(ion::input::Keycode::A))
		cameraMoveVector.x -= mCameraSpeed * deltaTime;
	if(mKeyboard->KeyDown(ion::input::Keycode::D))
		cameraMoveVector.x += mCameraSpeed * deltaTime;

	//Get mouse deltas
	float mouseDeltaX = (float)mMouse->GetDeltaX();
	float mouseDeltaY = (float)mMouse->GetDeltaY();

	//Move, pitch and yaw camera
	mCameraYaw -= mouseDeltaX * mMouseSensitivity;
	mCameraPitch += mouseDeltaY * mMouseSensitivity;
	ion::Quaternion cameraQuatYaw(mCameraYaw, ion::render::coordsys::Up);
	ion::Quaternion cameraQuatPitch(mCameraPitch, ion::render::coordsys::Right);
	mCamera->SetOrientation(cameraQuatYaw * cameraQuatPitch);
	mCamera->Move(cameraMoveVector);

	mSpriteAnim->Update(deltaTime);

	//Update renderer
	exit |= !mRenderer->Update(0.0f);

	//Update FPS display
	if(mFrameCount++ % 100 == 0)
	{
		//Get 100-frame end time and diff
		u64 endTicks = ion::time::GetSystemTicks();
		u64 diffTicks = endTicks - mStartTicks;

		//Calc frame time and frames per second
		float frameTime = (float)ion::time::TicksToSeconds(diffTicks) / 100.0f;
		float framesPerSecond = 1.0f / frameTime;

		//Set window title
		std::stringstream text;
		text.setf(std::ios::fixed, std::ios::floatfield);
		text.precision(2);
		text << "FPS: " << framesPerSecond;
		mWindow->SetTitle(text.str().c_str());

		//Reset timer
		mStartTicks = ion::time::GetSystemTicks();
	}

	return !exit;
}

void RenderTest::Render()
{
	mRenderer->BeginFrame(*mViewport, mWindow->GetDeviceContext());

	ion::Matrix4 inverseCameraMtx = mCamera->GetTransform().GetInverse();
	ion::Matrix4 objectMtx;
	ion::Matrix4 modelViewProjectionMtx = objectMtx * inverseCameraMtx * mRenderer->GetProjectionMatrix();

	mRenderer->ClearColour();
	mRenderer->ClearDepth();

	mSprite->Render(*mRenderer, *mCamera);

	mRenderer->SwapBuffers();
	mRenderer->EndFrame();
}