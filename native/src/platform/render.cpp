// Adapted from Spice Home C++ src/render.cpp, MIT, Copyright (c) 2026 Spice Home contributors.
// Retains its GLES2 function loader and quad/PNG path; drops launcher art, fonts and UI.
#include "spice/render.hpp"
#include <png.h>
#include <cstring>
#include <stdexcept>

namespace spice {
namespace {
constexpr GLenum GL_VENDOR_ = 0x1f00, GL_RENDERER_ = 0x1f01, GL_VERSION_ = 0x1f02,
  GL_MAX_TEXTURE_SIZE_ = 0xd33, GL_BLEND_ = 0xbe2, GL_DEPTH_TEST_ = 0xb71,
  GL_SRC_ALPHA_ = 0x302, GL_ONE_MINUS_SRC_ALPHA_ = 0x303, GL_COLOR_BUFFER_BIT_ = 0x4000,
  GL_VERTEX_SHADER_ = 0x8b31, GL_FRAGMENT_SHADER_ = 0x8b30, GL_COMPILE_STATUS_ = 0x8b81,
  GL_LINK_STATUS_ = 0x8b82, GL_ARRAY_BUFFER_ = 0x8892, GL_DYNAMIC_DRAW_ = 0x88e8,
  GL_FLOAT_ = 0x1406, GL_TRIANGLES_ = 4, GL_TEXTURE_2D_ = 0xde1, GL_TEXTURE0_ = 0x84c0,
  GL_RGBA_ = 0x1908, GL_UNSIGNED_BYTE_ = 0x1401, GL_TEXTURE_MIN_FILTER_ = 0x2801,
  GL_TEXTURE_MAG_FILTER_ = 0x2800, GL_NEAREST_ = 0x2600, GL_TEXTURE_WRAP_S_ = 0x2802,
  GL_TEXTURE_WRAP_T_ = 0x2803, GL_CLAMP_TO_EDGE_ = 0x812f, GL_UNPACK_ALIGNMENT_ = 0xcf5,
  GL_PACK_ALIGNMENT_ = 0xd05;
constexpr std::size_t ImageBudget = 32 * 1024 * 1024;
}
Gl::Gl(Sdl& sdl) {
#define LOAD(name) name = reinterpret_cast<decltype(name)>(sdl.GL_GetProcAddress("gl" #name)); if (!name) throw std::runtime_error("Missing GL symbol: gl" #name)
  LOAD(GetString); LOAD(GetIntegerv); LOAD(GetError); LOAD(Viewport); LOAD(ClearColor);
  LOAD(Clear); LOAD(Enable); LOAD(Disable); LOAD(BlendFunc); LOAD(CreateShader);
  LOAD(ShaderSource); LOAD(CompileShader); LOAD(GetShaderiv); LOAD(GetShaderInfoLog);
  LOAD(DeleteShader); LOAD(CreateProgram); LOAD(AttachShader); LOAD(BindAttribLocation);
  LOAD(LinkProgram); LOAD(GetProgramiv); LOAD(GetProgramInfoLog); LOAD(UseProgram);
  LOAD(DeleteProgram); LOAD(GetUniformLocation); LOAD(Uniform1i); LOAD(Uniform1f);
  LOAD(Uniform2f); LOAD(Uniform4f); LOAD(GenBuffers); LOAD(BindBuffer); LOAD(BufferData);
  LOAD(BufferSubData); LOAD(DeleteBuffers); LOAD(EnableVertexAttribArray);
  LOAD(VertexAttribPointer); LOAD(DrawArrays); LOAD(GenTextures); LOAD(BindTexture);
  LOAD(TexParameteri); LOAD(TexImage2D); LOAD(DeleteTextures); LOAD(ActiveTexture);
  LOAD(PixelStorei); LOAD(ReadPixels); LOAD(Finish);
#undef LOAD
}
Color Color::hex(const std::string& text) {
  if (text.size() != 7 || text.front() != '#') return {};
  try {
    std::size_t used = 0;
    const auto v = std::stoul(text.substr(1), &used, 16);
    if (used != 6) return {};
    return {((v >> 16) & 255) / 255.f, ((v >> 8) & 255) / 255.f, (v & 255) / 255.f, 1};
  } catch (...) { return {}; }
}
GLuint Renderer::compile(GLenum type, const std::string& source) {
  const auto id = gl_.CreateShader(type);
  const char* p = source.c_str();
  gl_.ShaderSource(id, 1, &p, nullptr);
  gl_.CompileShader(id);
  int ok = 0;
  gl_.GetShaderiv(id, GL_COMPILE_STATUS_, &ok);
  if (!ok) {
    std::array<char, 2048> log{};
    gl_.GetShaderInfoLog(id, static_cast<GLsizei>(log.size()), nullptr, log.data());
    gl_.DeleteShader(id);
    throw std::runtime_error(std::string("Shader failed: ") + log.data());
  }
  return id;
}
Renderer::Renderer(Sdl& sdl, const std::filesystem::path& assets) : gl_(sdl) {
  GLuint vs = 0, fs = 0;
  try {
    const auto str = [&](GLenum token) {
      const auto* p = gl_.GetString(token);
      return p ? std::string(reinterpret_cast<const char*>(p)) : "unknown";
    };
    vendor = str(GL_VENDOR_); renderer = str(GL_RENDERER_); version = str(GL_VERSION_);
    is_es = version.find("OpenGL ES") != std::string::npos;
#ifdef SKIFREE_WEBOS
    if (!is_es) throw std::runtime_error("webOS requires an actual OpenGL ES context");
#endif
    gl_.GetIntegerv(GL_MAX_TEXTURE_SIZE_, &texture_limit_);
    if (texture_limit_ < 512) throw std::runtime_error("GPU texture limit is too small");
    const std::string prefix = is_es
      ? "#version 100\nprecision mediump float;\n#define VARY_PREC mediump\n"
      : "#version 120\n#define VARY_PREC\n";
    const std::string vertex = (is_es
      ? "#version 100\nprecision highp float;\n#define VARY_PREC mediump\n" : prefix) + R"(
attribute vec2 a_position;
attribute vec2 a_uv;
varying VARY_PREC vec2 v_uv;
uniform vec2 u_view;
void main() {
  v_uv = a_uv;
  gl_Position = vec4(a_position.x / u_view.x * 2.0 - 1.0,
                     1.0 - a_position.y / u_view.y * 2.0, 0.0, 1.0);
}
)";
    const std::string fragment = prefix + R"(
varying VARY_PREC vec2 v_uv;
uniform sampler2D u_texture;
uniform vec4 u_color;
uniform vec2 u_size;
uniform float u_radius;
uniform float u_border;
uniform float u_aa;
uniform int u_mode;
void main() {
  vec4 col = u_color;
  if (u_mode == 1) col *= texture2D(u_texture, v_uv);
  if (u_radius > 0.0 || u_border > 0.0) {
    vec2 p = abs((v_uv - 0.5) * u_size) - (u_size * 0.5 - vec2(u_radius));
    float d = length(max(p, 0.0)) + min(max(p.x, p.y), 0.0) - u_radius;
    float coverage = 1.0 - smoothstep(-u_aa, u_aa, d);
    if (u_border > 0.0) coverage *= smoothstep(-u_border-u_aa, -u_border+u_aa, d);
    col.a *= coverage;
  }
  gl_FragColor = col;
}
)";
    vs = compile(GL_VERTEX_SHADER_, vertex);
    fs = compile(GL_FRAGMENT_SHADER_, fragment);
    program_ = gl_.CreateProgram();
    gl_.AttachShader(program_, vs); gl_.AttachShader(program_, fs);
    gl_.BindAttribLocation(program_, 0, "a_position");
    gl_.BindAttribLocation(program_, 1, "a_uv");
    gl_.LinkProgram(program_);
    gl_.DeleteShader(vs); vs = 0; gl_.DeleteShader(fs); fs = 0;
    int linked = 0;
    gl_.GetProgramiv(program_, GL_LINK_STATUS_, &linked);
    if (!linked) {
      std::array<char, 2048> log{};
      gl_.GetProgramInfoLog(program_, static_cast<GLsizei>(log.size()), nullptr, log.data());
      throw std::runtime_error(std::string("GL program failed: ") + log.data());
    }
    gl_.UseProgram(program_);
    mode_ = gl_.GetUniformLocation(program_, "u_mode");
    color_ = gl_.GetUniformLocation(program_, "u_color");
    size_ = gl_.GetUniformLocation(program_, "u_size");
    radius_ = gl_.GetUniformLocation(program_, "u_radius");
    border_ = gl_.GetUniformLocation(program_, "u_border");
    aa_ = gl_.GetUniformLocation(program_, "u_aa");
    gl_.Uniform1i(gl_.GetUniformLocation(program_, "u_texture"), 0);
    gl_.Uniform2f(gl_.GetUniformLocation(program_, "u_view"), DesignWidth, DesignHeight);
    gl_.GenBuffers(1, &vbo_);
    gl_.BindBuffer(GL_ARRAY_BUFFER_, vbo_);
    gl_.BufferData(GL_ARRAY_BUFFER_, 24 * sizeof(float), nullptr, GL_DYNAMIC_DRAW_);
    const std::array<unsigned char, 4> white{255, 255, 255, 255};
    white_ = upload(1, 1, white.data());
    if (!assets.empty()) {
      std::vector<std::filesystem::path> files;
      for (const auto& entry : std::filesystem::directory_iterator(assets)) {
        if (entry.is_regular_file() && entry.path().extension() == ".png") files.push_back(entry.path());
        if (files.size() > 512) throw std::runtime_error("Sprite file count exceeds budget");
      }
      std::sort(files.begin(), files.end());
      for (const auto& file : files) loadImage(file.stem().string(), file);
    }
  } catch (...) {
    if (vs) gl_.DeleteShader(vs);
    if (fs) gl_.DeleteShader(fs);
    release();
    throw;
  }
}
void Renderer::release() noexcept {
  for (const auto& [name, t] : images_) { (void)name; gl_.DeleteTextures(1, &t.id); }
  images_.clear(); image_bytes_ = 0;
  if (white_.id) gl_.DeleteTextures(1, &white_.id);
  if (vbo_) gl_.DeleteBuffers(1, &vbo_);
  if (program_) gl_.DeleteProgram(program_);
  white_ = {}; vbo_ = 0; program_ = 0;
}
Renderer::~Renderer() { release(); }
Texture Renderer::upload(int w, int h, const unsigned char* data) {
  Texture t;
  t.w = w; t.h = h; t.bytes = static_cast<std::size_t>(w) * static_cast<std::size_t>(h) * 4;
  gl_.GenTextures(1, &t.id); gl_.ActiveTexture(GL_TEXTURE0_); gl_.BindTexture(GL_TEXTURE_2D_, t.id);
  gl_.PixelStorei(GL_UNPACK_ALIGNMENT_, 1);
  gl_.TexParameteri(GL_TEXTURE_2D_, GL_TEXTURE_MIN_FILTER_, GL_NEAREST_);
  gl_.TexParameteri(GL_TEXTURE_2D_, GL_TEXTURE_MAG_FILTER_, GL_NEAREST_);
  gl_.TexParameteri(GL_TEXTURE_2D_, GL_TEXTURE_WRAP_S_, GL_CLAMP_TO_EDGE_);
  gl_.TexParameteri(GL_TEXTURE_2D_, GL_TEXTURE_WRAP_T_, GL_CLAMP_TO_EDGE_);
  gl_.TexImage2D(GL_TEXTURE_2D_, 0, GL_RGBA_, w, h, 0, GL_RGBA_, GL_UNSIGNED_BYTE_, data);
  if (gl_.GetError() != 0) {
    gl_.DeleteTextures(1, &t.id);
    throw std::runtime_error("GPU texture upload failed");
  }
  return t;
}
Texture Renderer::png(const std::filesystem::path& path) {
  png_image image{}; image.version = PNG_IMAGE_VERSION;
  if (!png_image_begin_read_from_file(&image, path.string().c_str()))
    throw std::runtime_error("Cannot decode sprite: " + path.filename().string());
  image.format = PNG_FORMAT_RGBA;
  if (image.width == 0 || image.height == 0 || image.width > static_cast<unsigned>(texture_limit_) ||
      image.height > static_cast<unsigned>(texture_limit_) || image.width > 8192 || image.height > 8192 ||
      static_cast<std::uint64_t>(image.width) * image.height * 4 > ImageBudget - image_bytes_) {
    png_image_free(&image);
    throw std::runtime_error("Sprite exceeds texture budget: " + path.filename().string());
  }
  std::vector<unsigned char> data(PNG_IMAGE_SIZE(image));
  if (!png_image_finish_read(&image, nullptr, data.data(), 0, nullptr)) {
    png_image_free(&image);
    throw std::runtime_error("Sprite decode failed: " + path.filename().string());
  }
  const int w = static_cast<int>(image.width), h = static_cast<int>(image.height);
  png_image_free(&image);
  return upload(w, h, data.data());
}
void Renderer::loadImage(const std::string& name, const std::filesystem::path& path) {
  if (name.empty() || images_.find(name) != images_.end() || images_.size() >= 512)
    throw std::runtime_error("Image name duplicate/empty or sprite count exceeded: " + name);
  const Texture t = png(path);
  try { images_.emplace(name, t); }
  catch (...) { gl_.DeleteTextures(1, &t.id); throw; }
  image_bytes_ += t.bytes;
}
void Renderer::removeImage(const std::string& name) {
  const auto found = images_.find(name);
  if (found == images_.end()) return;
  gl_.DeleteTextures(1, &found->second.id);
  image_bytes_ -= found->second.bytes;
  images_.erase(found);
}
void Renderer::loadPixels(const std::string& name, int width, int height, const unsigned char* rgba) {
  if (name.empty() || images_.find(name) != images_.end() || images_.size() >= 512)
    throw std::runtime_error("Image name duplicate/empty or sprite count exceeded: " + name);
  if (!rgba || width <= 0 || height <= 0 || width > texture_limit_ || height > texture_limit_ ||
      width > 8192 || height > 8192 ||
      static_cast<std::uint64_t>(width) * static_cast<std::uint64_t>(height) * 4 > ImageBudget - image_bytes_)
    throw std::runtime_error("Pixel image exceeds texture budget: " + name);
  const Texture t = upload(width, height, rgba);
  try { images_.emplace(name, t); }
  catch (...) { gl_.DeleteTextures(1, &t.id); throw; }
  image_bytes_ += t.bytes;
}
Point Renderer::imageSize(const std::string& name) const {
  const auto& t = images_.at(name);
  return {static_cast<float>(t.w), static_cast<float>(t.h)};
}
void Renderer::begin(const Viewport& viewport, Color clear) {
  transform(); view_ = viewport; draws = 0;
  gl_.Viewport(0, 0, view_.drawable_w, view_.drawable_h);
  gl_.ClearColor(clear.r, clear.g, clear.b, clear.a); gl_.Clear(GL_COLOR_BUFFER_BIT_);
  gl_.Viewport(static_cast<int>(view_.left), static_cast<int>(view_.top),
               static_cast<int>(DesignWidth * view_.scale), static_cast<int>(DesignHeight * view_.scale));
  gl_.Disable(GL_DEPTH_TEST_); gl_.Enable(GL_BLEND_);
  gl_.BlendFunc(GL_SRC_ALPHA_, GL_ONE_MINUS_SRC_ALPHA_); gl_.UseProgram(program_);
  gl_.BindBuffer(GL_ARRAY_BUFFER_, vbo_);
  gl_.EnableVertexAttribArray(0); gl_.EnableVertexAttribArray(1);
  gl_.VertexAttribPointer(0, 2, GL_FLOAT_, 0, 4 * sizeof(float), nullptr);
  gl_.VertexAttribPointer(1, 2, GL_FLOAT_, 0, 4 * sizeof(float), reinterpret_cast<void*>(2 * sizeof(float)));
}
void Renderer::end() {
  if (gl_.GetError() != 0) throw std::runtime_error("GLES2 drawing failed");
}
void Renderer::quad(Rect r, int mode, Color c, float radius, float stroke, GLuint tex,
                    const std::array<float, 4>& uv) {
  if (r.w <= 0 || r.h <= 0 || c.a <= 0) return;
  c.a *= opacity_;
  const float x = r.x + offset_x_, y = r.y + offset_y_, x2 = x + r.w, y2 = y + r.h;
  const std::array<float, 24> vertices{
    x,y,uv[0],uv[1], x2,y,uv[2],uv[1], x2,y2,uv[2],uv[3],
    x,y,uv[0],uv[1], x2,y2,uv[2],uv[3], x,y2,uv[0],uv[3]};
  gl_.Uniform1i(mode_, mode); gl_.Uniform4f(color_, c.r, c.g, c.b, c.a);
  gl_.Uniform2f(size_, r.w, r.h); gl_.Uniform1f(radius_, radius); gl_.Uniform1f(border_, stroke);
  gl_.Uniform1f(aa_, .9f / view_.scale);
  gl_.ActiveTexture(GL_TEXTURE0_); gl_.BindTexture(GL_TEXTURE_2D_, tex ? tex : white_.id);
  gl_.BufferSubData(GL_ARRAY_BUFFER_, 0, sizeof(vertices), vertices.data());
  gl_.DrawArrays(GL_TRIANGLES_, 0, 6); ++draws;
}
void Renderer::rect(Rect r, Color c, float radius, float stroke) { quad(r, 0, c, radius, stroke, white_.id); }
void Renderer::image(const std::string& name, Rect destination, float alpha, float radius) {
  quad(destination, 1, {1, 1, 1, alpha}, radius, 0, images_.at(name).id);
}
void Renderer::sprite(const std::string& name, Rect source, Rect destination, float alpha) {
  const auto& t = images_.at(name);
  if (source.x < 0 || source.y < 0 || source.w <= 0 || source.h <= 0 ||
      source.x + source.w > t.w || source.y + source.h > t.h)
    throw std::runtime_error("Sprite source rectangle outside image: " + name);
  quad(destination, 1, {1, 1, 1, alpha}, 0, 0, t.id,
       {source.x / t.w, source.y / t.h, (source.x + source.w) / t.w, (source.y + source.h) / t.h});
}
void Renderer::line(float x, float y, float x2, float y2, Color c, float thickness) {
  x += offset_x_; x2 += offset_x_; y += offset_y_; y2 += offset_y_; c.a *= opacity_;
  const float dx = x2 - x, dy = y2 - y, length = std::sqrt(dx * dx + dy * dy);
  if (length < .001f || thickness <= 0 || c.a <= 0) return;
  const float nx = -dy / length * thickness * .5f, ny = dx / length * thickness * .5f;
  const std::array<float, 24> vertices{
    x+nx,y+ny,0,0, x2+nx,y2+ny,1,0, x2-nx,y2-ny,1,1,
    x+nx,y+ny,0,0, x2-nx,y2-ny,1,1, x-nx,y-ny,0,1};
  gl_.Uniform1i(mode_, 0); gl_.Uniform4f(color_, c.r, c.g, c.b, c.a);
  gl_.Uniform2f(size_, length, thickness); gl_.Uniform1f(radius_, 0); gl_.Uniform1f(border_, 0);
  gl_.ActiveTexture(GL_TEXTURE0_); gl_.BindTexture(GL_TEXTURE_2D_, white_.id);
  gl_.BufferSubData(GL_ARRAY_BUFFER_, 0, sizeof(vertices), vertices.data());
  gl_.DrawArrays(GL_TRIANGLES_, 0, 6); ++draws;
}
void Renderer::screenshot(const std::filesystem::path& output) {
  const int w = view_.drawable_w, h = view_.drawable_h;
  if (w > 8192 || h > 8192) throw std::runtime_error("Screenshot exceeds pixel budget");
  if (!output.parent_path().empty()) std::filesystem::create_directories(output.parent_path());
  std::vector<unsigned char> data(static_cast<std::size_t>(w) * static_cast<std::size_t>(h) * 4), flipped(data.size());
  gl_.Finish(); gl_.PixelStorei(GL_PACK_ALIGNMENT_, 1);
  gl_.ReadPixels(0, 0, w, h, GL_RGBA_, GL_UNSIGNED_BYTE_, data.data());
  if (gl_.GetError() != 0) throw std::runtime_error("Framebuffer read failed");
  for (int y = 0; y < h; ++y)
    std::memcpy(flipped.data() + static_cast<std::size_t>(y) * w * 4,
                data.data() + static_cast<std::size_t>(h - 1 - y) * w * 4, static_cast<std::size_t>(w) * 4);
  png_image image{}; image.version = PNG_IMAGE_VERSION;
  image.width = static_cast<unsigned>(w); image.height = static_cast<unsigned>(h); image.format = PNG_FORMAT_RGBA;
  if (!png_image_write_to_file(&image, output.string().c_str(), 0, flipped.data(), 0, nullptr))
    throw std::runtime_error("Screenshot write failed");
}
}
