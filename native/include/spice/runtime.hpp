#pragma once
// SDL window/input/runtime subset adapted from Spice Home C++ (MIT).
#include "native_abi.hpp"
#include <string>
#include <vector>

namespace spice {
class Library {
  void* handle_ = nullptr;
public:
  explicit Library(const std::vector<std::string>& names);
  ~Library();
  Library(const Library&) = delete;
  Library& operator=(const Library&) = delete;
  void* symbol(const char* name) const;
  template<class T> T get(const char* name) const { return reinterpret_cast<T>(symbol(name)); }
};
struct Sdl {
#ifdef SKIFREE_WEBOS
  // SDL-webOS explicitly dlopens this codec during its startup. Load it from
  // the executable's $ORIGIN/lib RUNPATH before SDL's own dlopen call.
  Library imageCodec;
#endif
  Library lib;
  void (*SetMainReady)();
  int (*Init)(Uint32);
  void (*Quit)();
  const char* (*GetError)();
  char* (*GetPrefPath)(const char*, const char*);
  void (*Free)(void*);
  void (*GetVersion)(SDL_version*);
  SDL_Window* (*CreateWindow)(const char*, int, int, int, int, Uint32);
  void (*DestroyWindow)(SDL_Window*);
  int (*GL_SetAttribute)(SDL_GLattr, int);
  SDL_GLContext (*GL_CreateContext)(SDL_Window*);
  void (*GL_DeleteContext)(SDL_GLContext);
  void* (*GL_GetProcAddress)(const char*);
  int (*GL_SetSwapInterval)(int);
  int (*GL_GetSwapInterval)();
  void (*GL_GetDrawableSize)(SDL_Window*, int*, int*);
  void (*GL_SwapWindow)(SDL_Window*);
  void (*GetWindowSize)(SDL_Window*, int*, int*);
  int (*SetWindowFullscreen)(SDL_Window*, Uint32);
  int (*GetCurrentDisplayMode)(int, SDL_DisplayMode*);
  int (*PollEvent)(SDL_Event*);
  const Uint8* (*GetKeyboardState)(int*);
  int (*PushEvent)(SDL_Event*);
  int (*WaitEventTimeout)(SDL_Event*, int);
  const char* (*GetScancodeName)(SDL_Scancode);
  int (*NumJoysticks)();
  SDL_bool (*IsGameController)(int);
  SDL_GameController* (*GameControllerOpen)(int);
  void (*GameControllerClose)(SDL_GameController*);
  SDL_Joystick* (*GameControllerGetJoystick)(SDL_GameController*);
  SDL_JoystickID (*JoystickInstanceID)(SDL_Joystick*);
  Sdl();
};
}
