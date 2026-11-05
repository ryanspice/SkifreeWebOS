#pragma once
// Raw GLES2 quad renderer adapted from Spice Home C++ (MIT).
// SDL owns only the window, GL context and input. No SDL_Renderer or font engine.
#include "spice/core.hpp"
#include "spice/runtime.hpp"
#include <cstdint>
#include <map>

namespace spice {
#ifdef _WIN32
#define SPICE_GL_CALL __stdcall
#else
#define SPICE_GL_CALL
#endif
using GLenum = unsigned;
using GLuint = unsigned;
using GLint = int;
using GLsizei = int;
using GLboolean = unsigned char;
using GLfloat = float;
using GLchar = char;
using GLsizeiptr = std::ptrdiff_t;
struct Gl {
  const unsigned char* (SPICE_GL_CALL *GetString)(GLenum);
  void (SPICE_GL_CALL *GetIntegerv)(GLenum, GLint*);
  GLenum (SPICE_GL_CALL *GetError)();
  void (SPICE_GL_CALL *Viewport)(GLint, GLint, GLsizei, GLsizei);
  void (SPICE_GL_CALL *ClearColor)(GLfloat, GLfloat, GLfloat, GLfloat);
  void (SPICE_GL_CALL *Clear)(GLenum);
  void (SPICE_GL_CALL *Enable)(GLenum);
  void (SPICE_GL_CALL *Disable)(GLenum);
  void (SPICE_GL_CALL *BlendFunc)(GLenum, GLenum);
  GLuint (SPICE_GL_CALL *CreateShader)(GLenum);
  void (SPICE_GL_CALL *ShaderSource)(GLuint, GLsizei, const GLchar* const*, const GLint*);
  void (SPICE_GL_CALL *CompileShader)(GLuint);
  void (SPICE_GL_CALL *GetShaderiv)(GLuint, GLenum, GLint*);
  void (SPICE_GL_CALL *GetShaderInfoLog)(GLuint, GLsizei, GLsizei*, GLchar*);
  void (SPICE_GL_CALL *DeleteShader)(GLuint);
  GLuint (SPICE_GL_CALL *CreateProgram)();
  void (SPICE_GL_CALL *AttachShader)(GLuint, GLuint);
  void (SPICE_GL_CALL *BindAttribLocation)(GLuint, GLuint, const GLchar*);
  void (SPICE_GL_CALL *LinkProgram)(GLuint);
  void (SPICE_GL_CALL *GetProgramiv)(GLuint, GLenum, GLint*);
  void (SPICE_GL_CALL *GetProgramInfoLog)(GLuint, GLsizei, GLsizei*, GLchar*);
  void (SPICE_GL_CALL *UseProgram)(GLuint);
  void (SPICE_GL_CALL *DeleteProgram)(GLuint);
  GLint (SPICE_GL_CALL *GetUniformLocation)(GLuint, const GLchar*);
  void (SPICE_GL_CALL *Uniform1i)(GLint, GLint);
  void (SPICE_GL_CALL *Uniform1f)(GLint, GLfloat);
  void (SPICE_GL_CALL *Uniform2f)(GLint, GLfloat, GLfloat);
  void (SPICE_GL_CALL *Uniform4f)(GLint, GLfloat, GLfloat, GLfloat, GLfloat);
  void (SPICE_GL_CALL *GenBuffers)(GLsizei, GLuint*);
  void (SPICE_GL_CALL *BindBuffer)(GLenum, GLuint);
  void (SPICE_GL_CALL *BufferData)(GLenum, GLsizeiptr, const void*, GLenum);
  void (SPICE_GL_CALL *BufferSubData)(GLenum, std::ptrdiff_t, GLsizeiptr, const void*);
  void (SPICE_GL_CALL *DeleteBuffers)(GLsizei, const GLuint*);
  void (SPICE_GL_CALL *EnableVertexAttribArray)(GLuint);
  void (SPICE_GL_CALL *VertexAttribPointer)(GLuint, GLint, GLenum, GLboolean, GLsizei, const void*);
  void (SPICE_GL_CALL *DrawArrays)(GLenum, GLint, GLsizei);
  void (SPICE_GL_CALL *GenTextures)(GLsizei, GLuint*);
  void (SPICE_GL_CALL *BindTexture)(GLenum, GLuint);
  void (SPICE_GL_CALL *TexParameteri)(GLenum, GLenum, GLint);
  void (SPICE_GL_CALL *TexImage2D)(GLenum, GLint, GLint, GLsizei, GLsizei, GLint, GLenum, GLenum, const void*);
  void (SPICE_GL_CALL *DeleteTextures)(GLsizei, const GLuint*);
  void (SPICE_GL_CALL *ActiveTexture)(GLenum);
  void (SPICE_GL_CALL *PixelStorei)(GLenum, GLint);
  void (SPICE_GL_CALL *ReadPixels)(GLint, GLint, GLsizei, GLsizei, GLenum, GLenum, void*);
  void (SPICE_GL_CALL *Finish)();
  explicit Gl(Sdl& sdl);
};
struct Color {
  float r = 1, g = 1, b = 1, a = 1;
  Color opacity(float x) const { return {r, g, b, a * x}; }
  static Color hex(const std::string& s);
};
struct Texture { GLuint id = 0; int w = 0, h = 0; std::size_t bytes = 0; };

class Renderer {
  Gl gl_;
  GLuint program_ = 0, vbo_ = 0;
  Texture white_;
  GLint mode_ = 0, color_ = 0, size_ = 0, radius_ = 0, border_ = 0, aa_ = 0;
  std::map<std::string, Texture> images_;
  std::size_t image_bytes_ = 0;
  int texture_limit_ = 2048;
  Viewport view_;
  float offset_x_ = 0, offset_y_ = 0, opacity_ = 1;
  GLuint compile(GLenum type, const std::string& source);
  Texture upload(int w, int h, const unsigned char* bytes);
  Texture png(const std::filesystem::path& path);
  void release() noexcept;
  void quad(Rect rect, int mode, Color color, float radius, float stroke, GLuint texture,
            const std::array<float, 4>& uv = {0, 0, 1, 1});
public:
  std::string vendor, renderer, version;
  bool is_es = false;
  int draws = 0;
  // Loads immediate *.png children, using the filename stem as the image name.
  // Pass an empty path to load explicitly with loadImage(). No recursive discovery.
  Renderer(Sdl& sdl, const std::filesystem::path& assets);
  ~Renderer();
  Renderer(const Renderer&) = delete;
  Renderer& operator=(const Renderer&) = delete;
  void transform(float x = 0, float y = 0, float opacity = 1) {
    offset_x_ = x; offset_y_ = y; opacity_ = opacity;
  }
  void begin(const Viewport& viewport, Color clear = {1, 1, 1, 1});
  void end();
  void rect(Rect bounds, Color color, float radius = 0, float stroke = 0);
  void line(float x1, float y1, float x2, float y2, Color color, float thickness = 2);
  void loadImage(const std::string& name, const std::filesystem::path& path);
  void removeImage(const std::string& name);
  void loadPixels(const std::string& name, int width, int height, const unsigned char* rgba);
  bool hasImage(const std::string& name) const { return images_.find(name) != images_.end(); }
  Point imageSize(const std::string& name) const;
  // Full image; nearest filtering; no fill-cropping or launcher-specific artwork.
  void image(const std::string& name, Rect destination, float alpha = 1, float radius = 0);
  void sprite(const std::string& name, Rect sourcePixels, Rect destination, float alpha = 1);
  void screenshot(const std::filesystem::path& output);
  std::size_t texture_bytes() const { return image_bytes_ + white_.bytes; }
  const Viewport& viewport() const { return view_; }
};
}
