#pragma once

#if _LIB
#ifdef _DEBUG
#define LIB_NAME_ENDER "d"
#else
#define LIB_NAME_ENDER
#endif

#pragma comment(lib, "coreclr/hostfxr.lib")
#pragma comment(lib, "coreclr/hostpolicy.lib")
#pragma comment(lib, "coreclr/nethost.lib")

#pragma comment(lib, "coral/Coral.Native" LIB_NAME_ENDER ".lib")
#endif
