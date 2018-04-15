#pragma once

#ifdef CAM_HOOK_EXPORT
#define CAMHOOK_EXPORT __declspec(dllexport)
#else
//#define CAMHOOK_EXPORT __declspec(dllimport)
#define CAMHOOK_EXPORT
#endif