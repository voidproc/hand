#pragma once

//#define USE_RESOURCE
#ifdef USE_RESOURCE
#define RES(path) (Resource(path))
#else
#define RES(path) (path)
#endif
