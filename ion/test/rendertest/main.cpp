#include "RenderTest.h"
#include <ion/core/time/Time.h>
#include <ion/core/debug/Debug.h>

#include "arch/gdb.h"

int main(char** args, int numargs)
{
	gdb_init();

	ion::debug::Log("HELLO WORLD");

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