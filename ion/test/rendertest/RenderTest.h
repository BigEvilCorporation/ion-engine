#include <ion/core/bootstrap/Application.h>
#include <ion/core/Version.h>
#include <ion/core/debug/Debug.h>
#include <ion/core/thread/Thread.h>
#include <ion/core/thread/Sleep.h>
#include <ion/core/time/Time.h>
#include <ion/io/FileSystem.h>
#include <ion/io/ResourceManager.h>
#include <ion/io/ResourceHandle.h>
#include <ion/maths/Vector.h>
#include <ion/maths/Matrix.h>
#include <ion/input/Keyboard.h>
#include <ion/input/Mouse.h>
#include <ion/input/Gamepad.h>
#include <ion/renderer/Window.h>
#include <ion/renderer/Renderer.h>
#include <ion/renderer/Viewport.h>
#include <ion/renderer/CoordSys.h>
#include <ion/renderer/Colour.h>
#include <ion/renderer/Camera.h>
#include <ion/renderer/Primitive.h>
#include <ion/renderer/Texture.h>
#include <ion/renderer/Sprite.h>

#if defined ION_RENDERER_SHADER
#include <ion/renderer/Shader.h>
#endif

class RenderTest : public ion::framework::Application
{
public:
	RenderTest();
	~RenderTest();

	bool Initialise();
	void Shutdown();
	bool Update(float deltaTime);
	void Render();

	ion::io::FileSystem* mFileSystem;
	ion::io::ResourceManager* mResourceManager;

	ion::input::Keyboard* mKeyboard;
	ion::input::Mouse* mMouse;
	ion::input::Gamepad* mGamepad;
	
	ion::render::Window* mWindow;
	ion::render::Renderer* mRenderer;
	ion::render::Viewport* mViewport;
	ion::render::Camera* mCamera;

	ion::render::Primitive* mQuad;
	ion::render::Primitive* mBox;
	ion::render::Primitive* mSphere;

#if defined ION_RENDERER_SHADER
	ion::render::Shader* mShaderDefaultV;
	ion::render::Shader* mShaderDefaultF;

	ion::render::Shader::ParamHndl<ion::Matrix4> mShaderParamWorldMtx;
	ion::render::Shader::ParamHndl<ion::Matrix4> mShaderParamWorldViewProjMtx;
	ion::render::Shader::ParamHndl<ion::render::Texture> mShaderParamTexture;
#endif

	ion::io::ResourceHandle<ion::render::Texture> mTexture;
	ion::render::Sprite* mSprite;
	ion::render::SpriteAnimation* mSpriteAnim;
	ion::render::AnimationTrackInt* mSpriteAnimTrack;

	float mCameraPitch;
	float mCameraYaw;
	float mCameraSpeed;
	float mMouseSensitivity;

	float mLightSin;
	float mLightCos;

	u64 mStartTicks;
	u32 mFrameCount;
};