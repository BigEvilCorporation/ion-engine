#include "RenderTest.h"
#include "Core/Time.h"

int main(char** args, int numargs)
{
	RenderTest app;
	
	if(app.Initialise())
	{
		float deltaTime = 0.0f;
		bool run = true;
		while(run)
		{
			u64 startTicks = ion::time::GetSystemTicks();

			if(run = app.Update(deltaTime))
			{
				app.Render();
			}

			u64 endTicks = ion::time::GetSystemTicks();
			deltaTime = (float)ion::time::TicksToSeconds(endTicks - startTicks);
		}

		app.Shutdown();
	}
}