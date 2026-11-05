#include "spice/runtime.hpp"
#include <stdexcept>
#ifdef _WIN32
#define NOMINMAX
#include <windows.h>
#else
#include <dlfcn.h>
#endif

namespace spice {
Library::Library(const std::vector<std::string>& names) {
  for (const auto& name : names) {
#ifdef _WIN32
    handle_ = reinterpret_cast<void*>(LoadLibraryExA(name.c_str(), nullptr, LOAD_LIBRARY_SEARCH_DEFAULT_DIRS));
#else
    handle_ = dlopen(name.c_str(), RTLD_NOW | RTLD_LOCAL);
#endif
    if (handle_) break;
  }
  if (!handle_) throw std::runtime_error("Missing native runtime: " + names.front());
}
Library::~Library() {
  if (!handle_) return;
#ifdef _WIN32
  FreeLibrary(reinterpret_cast<HMODULE>(handle_));
#else
  dlclose(handle_);
#endif
}
void* Library::symbol(const char* name) const {
#ifdef _WIN32
  auto p = reinterpret_cast<void*>(GetProcAddress(reinterpret_cast<HMODULE>(handle_), name));
#else
  auto p = dlsym(handle_, name);
#endif
  if (!p) throw std::runtime_error(std::string("Missing runtime symbol: ") + name);
  return p;
}
Sdl::Sdl() :
#ifdef SKIFREE_WEBOS
  imageCodec({"libpng16.so.16"}),
#endif
  lib({"libSDL2-2.0.so.0", "libSDL2.so", "SDL2.dll", "libSDL2.dylib"}) {
#define LOAD(name) name = lib.get<decltype(name)>("SDL_" #name)
  LOAD(SetMainReady);
  LOAD(Init);
  LOAD(Quit);
  LOAD(GetError);
  LOAD(GetPrefPath);
  Free = lib.get<decltype(Free)>("SDL_free");
  LOAD(GetVersion);
  LOAD(CreateWindow);
  LOAD(DestroyWindow);
  LOAD(GL_SetAttribute);
  LOAD(GL_CreateContext);
  LOAD(GL_DeleteContext);
  LOAD(GL_GetProcAddress);
  LOAD(GL_SetSwapInterval);
  LOAD(GL_GetSwapInterval);
  LOAD(GL_GetDrawableSize);
  LOAD(GL_SwapWindow);
  LOAD(GetWindowSize);
  LOAD(SetWindowFullscreen);
  LOAD(GetCurrentDisplayMode);
  LOAD(PollEvent);
  LOAD(GetKeyboardState);
  LOAD(PushEvent);
  LOAD(WaitEventTimeout);
  LOAD(GetScancodeName);
  LOAD(NumJoysticks);
  LOAD(IsGameController);
  LOAD(GameControllerOpen);
  LOAD(GameControllerClose);
  LOAD(GameControllerGetJoystick);
  LOAD(JoystickInstanceID);
#undef LOAD
  SDL_version version{};
  GetVersion(&version);
  if (version.major != 2 || (version.minor == 0 && version.patch < 10))
    throw std::runtime_error("SDL2 2.0.10 or newer runtime required");
}
}
