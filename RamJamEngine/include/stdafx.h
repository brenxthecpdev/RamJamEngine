// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#pragma once

#include "targetver.h"

#include "RjeConfig.h"

#if PLATFORM == PLATFORM_WIN32
#	define WIN32_LEAN_AND_MEAN             // Exclude rarely-used stuff from Windows headers
// Windows Header Files:
#	include <windows.h>
#	include <windowsx.h>
#endif

#if defined(DEBUG) || defined(_DEBUG)
#define _CRTDBG_MAP_ALLOC
#include <crtdbg.h>
#endif


// RunTime Header Files
#include <stdlib.h>
#include <malloc.h>
#include <memory.h>
#include <tchar.h>
#include <cassert>
#include <ctime>
#include <algorithm>
#include <string>
#include <sstream>
#include <fstream>
#include <vector>

#include "Resource.h"

//////////////////////////////////////////////////////////////////////////
// Core
#include "Globals.h"
#include "Memory.h"
#include "IniFile.h"
#include "Types.h"
#include "Singleton.h"
#include "Debug.h"
#include "Profiler.h"
#include "Timer.h"
#include "Input.h"
#include "Color.h"
//////////////////////////////////////////////////////////////////////////
// RapidXML
#include "rapidxml.hpp"
#include "rapidxml_iterators.hpp"
#include "rapidxml_print.hpp"
#include "rapidxml_utils.hpp"
//////////////////////////////////////////////////////////////////////////
// AntTweakBar
#include "AntTweakBar.h"
//////////////////////////////////////////////////////////////////////////
// Math
#include "MathHelper.h"
//////////////////////////////////////////////////////////////////////////
// Application Programming Interface
#include "GraphicAPI.h"
#if (RJE_GRAPHIC_API == DIRECTX_11)
#	include "DX11RenderingAPI.h"
#else
#	include "OglWrapper.h"
#endif
//////////////////////////////////////////////////////////////////////////
// RamJam Engine SubCores
#include "Scene.h"
#include "System.h"
#include "Console.h"
#include "Transform.h"
#include "MaterialFactory.h"
