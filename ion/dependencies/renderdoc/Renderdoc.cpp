#include "Renderdoc.h"

#include "api/app/renderdoc_app.h"

#include <windows.h>
#include <assert.h>
#include <cstdio>
#include <string>

#define RENDERDOC_LOAD_PASSIVE 1

namespace renderdoc
{
    RENDERDOC_API_1_1_2* rdoc_api = NULL;

    void Init()
    {
#if RENDERDOC_LOAD_PASSIVE
        if (HMODULE mod = GetModuleHandleA("renderdoc.dll"))
#else
        char cwd[MAX_PATH];
        char dll[MAX_PATH];
        GetCurrentDirectoryA(MAX_PATH, cwd);
        sprintf_s(dll, MAX_PATH, "%s\\renderdoc.dll", cwd);
        if (HMODULE mod = LoadLibraryA(dll))
#endif
        {
            pRENDERDOC_GetAPI RENDERDOC_GetAPI =
                (pRENDERDOC_GetAPI)GetProcAddress(mod, "RENDERDOC_GetAPI");
            int ret = RENDERDOC_GetAPI(eRENDERDOC_API_Version_1_1_2, (void**)&rdoc_api);
            assert(ret == 1);
        }
        else
        {
            wchar_t err[1024];
            memset(err, 0, 1024);
            FormatMessageW(FORMAT_MESSAGE_FROM_SYSTEM, NULL, GetLastError(), MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), err, 1024, NULL);
            MessageBoxW(NULL, err, (LPCWSTR)L"Could not load renderdoc.dll", MB_OK);
        }
    }

    void StartCapture()
    {
        if (rdoc_api)
            rdoc_api->StartFrameCapture(NULL, NULL);
    }

    void EndCapture()
    {
        if (rdoc_api)
            rdoc_api->EndFrameCapture(NULL, NULL);
    }
}