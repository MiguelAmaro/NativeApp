#ifndef GFX_H
#define GFX_H

#include <EGL/egl.h>
#include <GLES3/gl31.h>

// NOTE(MIGUEL): Canonical Vertex.. for now
//-TYPES
typedef struct vertex vertex;
struct vertex
{ v2f Pos; v3f Color; };
//-TYPES

typedef struct gfx_ctx gfx_ctx;
struct gfx_ctx
{
  GLuint LayoutId;
  u32 VStride;
  u32 IStride;
  GLuint VBufferId;
  GLuint IBufferId;
  GLuint SBufferId;
  GLuint ShaderId;
};
gfx_ctx GfxCtxInit(void)
{
  gfx_ctx Result = {0};
  return Result;
}
void GLClearErrors(void)
{
  while(GL_NO_ERROR != glGetError());
  return;
}
#define SymbolToStringHelper(x, y) y==x?#x:
void GLPrintLastError(const char *CallerName, const char *Message)
{
  GLenum ErrorCode = glGetError();
  const char *Error = (SymbolToStringHelper(GL_NO_ERROR, ErrorCode)
                       SymbolToStringHelper(GL_INVALID_ENUM, ErrorCode)
                       SymbolToStringHelper(GL_INVALID_VALUE, ErrorCode)
                       SymbolToStringHelper(GL_INVALID_OPERATION, ErrorCode)
                       SymbolToStringHelper(GL_INVALID_FRAMEBUFFER_OPERATION, ErrorCode)
                       SymbolToStringHelper(GL_OUT_OF_MEMORY, ErrorCode)
                       "undefined error\n");
  LOG("%s: [%s] %s\n", CallerName, Message, Error);
  return;
}
const char *GLGetUsageAsString(GLenum Usage)
{
  const char *AsString = (SymbolToStringHelper(GL_STREAM_DRAW, Usage)
                          SymbolToStringHelper(GL_STREAM_READ, Usage)
                          SymbolToStringHelper(GL_STREAM_COPY, Usage)
                          SymbolToStringHelper(GL_STATIC_DRAW, Usage)
                          SymbolToStringHelper(GL_STATIC_READ, Usage)
                          SymbolToStringHelper(GL_STATIC_COPY, Usage)
                          SymbolToStringHelper(GL_DYNAMIC_DRAW, Usage)
                          SymbolToStringHelper(GL_DYNAMIC_READ, Usage)
                          SymbolToStringHelper(GL_DYNAMIC_COPY, Usage)
                          "undefined usage");
  return AsString;
}
const char *GLGetAccessAsString(GLenum Access)
{
  
  const char *AsString = (SymbolToStringHelper(GL_STATIC_COPY, Access)
                          SymbolToStringHelper(GL_MAP_READ_BIT, Access)
                          SymbolToStringHelper(GL_MAP_WRITE_BIT, Access)
                          SymbolToStringHelper(GL_MAP_INVALIDATE_RANGE_BIT, Access)
                          SymbolToStringHelper(GL_MAP_INVALIDATE_BUFFER_BIT, Access)
                          SymbolToStringHelper(GL_MAP_FLUSH_EXPLICIT_BIT, Access)
                          SymbolToStringHelper(GL_MAP_UNSYNCHRONIZED_BIT, Access)
                          "undefined access");
  return AsString;
}
void GLLogBufferState()
{
  GLint  Access = 0;
  GLint  IsMapped = 0;
  GLint  MapLength = 0;
  GLint  MapOffset = 0;
  GLint  Size = 0;
  GLint  Usage = 0;
  glGetBufferParameteriv(GL_ARRAY_BUFFER, GL_BUFFER_ACCESS_FLAGS, &Access);
  glGetBufferParameteriv(GL_ARRAY_BUFFER, GL_BUFFER_USAGE, &Usage);
  glGetBufferParameteriv(GL_ARRAY_BUFFER, GL_BUFFER_MAPPED, &IsMapped);
  glGetBufferParameteriv(GL_ARRAY_BUFFER, GL_BUFFER_MAP_OFFSET, &MapOffset);
  glGetBufferParameteriv(GL_ARRAY_BUFFER, GL_BUFFER_MAP_LENGTH, &MapLength);
  glGetBufferParameteriv(GL_ARRAY_BUFFER, GL_BUFFER_SIZE, &Size);
  LOG("\nbuffer log:\n"
      "    access: %s\n"
      "    usage: %s\n"
      "    is mapped: %s\n"
      "    map offset: %d\n"
      "    map length: %d\n"
      "    buffer size: %d\n",
      GLGetAccessAsString(Access),
      GLGetUsageAsString(Usage),
      IsMapped?"yes":"no",
      MapOffset, MapLength, Size);
  return;
}
void GfxCtxDraw(gfx_ctx *Ctx, ui_elm Element)
{
  glUniform2fv(glGetUniformLocation(Ctx->ShaderId, "UWinRes"), 1, GlobalRes.comp);
  glUniform1fv(glGetUniformLocation(Ctx->ShaderId,   "URect"), 4, Element.Rect.comp);
  glUniform4fv(glGetUniformLocation(Ctx->ShaderId,  "UColor"), 1, Element.Color.comp);
  glUseProgram(Ctx->ShaderId);
  
  //Give gl buffer id and deffine the attirbutes of the vbuffer (stride, offeset)
  glBindBuffer(GL_ARRAY_BUFFER, Ctx->VBufferId);
  glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, (2+3)*sizeof(float), NULL);
  glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, (2+3)*sizeof(float), (void*)(2*sizeof(float)));
  glEnableVertexAttribArray(0);
  glEnableVertexAttribArray(1);
  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_SRC_ALPHA); //GL_ONE_MINUS_SRC_ALPHA
  glDrawArrays(GL_TRIANGLES, 0, 6);
  return;
}
void GfxCtxDrawBucketInstanced(gfx_ctx *Ctx, draw_bucket *Bucket)
{
  //map data to inst buffer
  glBindBuffer(GL_ARRAY_BUFFER, Ctx->IBufferId);
  struct ui_attribs *GLIBuffer = glMapBufferRange(GL_ARRAY_BUFFER, 0,
                                                  Bucket->Count*sizeof(quad_attribs),
                                                  GL_MAP_WRITE_BIT | GL_MAP_INVALIDATE_BUFFER_BIT);
  if(GLIBuffer)
  {
    LOG("poiner exists!!!!");
    memcpy(GLIBuffer, Bucket->QuadAttribs, Bucket->Count*sizeof(quad_attribs));
  }else
  {
    GLPrintLastError(ThisFuncionAsString(), "buffer mapping");
  }
  glUnmapBuffer(GL_ARRAY_BUFFER);
  //uniforms
  glUniform2fv(glGetUniformLocation(Ctx->ShaderId, "UWinRes"), 1, GlobalRes.comp);
  //bind layout and buffers
  glBindVertexArray(Ctx->LayoutId);
  //post draw effects
  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA); 
  glDrawArraysInstanced(GL_TRIANGLES, 0, 6, Bucket->Count);
  return;
}
u32 GfxShaderProgramCreate(const char *VertShaderSrc, s32 VertShaderSrcLength,
                           const char *FragShaderSrc, s32 FragShaderSrcLength)
{
  //Vert Compilationd
  GLuint VertShader = glCreateShader(GL_VERTEX_SHADER);
  glShaderSource(VertShader, 1, (const GLchar **)&VertShaderSrc, &VertShaderSrcLength);
  glCompileShader(VertShader);
  //Frag Compilation
  GLuint FragShader = glCreateShader(GL_FRAGMENT_SHADER);
  glShaderSource(FragShader, 1, (const GLchar **)&FragShaderSrc, &FragShaderSrcLength);
  glCompileShader(FragShader);
  //Mergs Shaders as program
  GLuint ShaderProgramId = glCreateProgram();
  glAttachShader(ShaderProgramId, VertShader);
  glAttachShader(ShaderProgramId, FragShader);
  //input assembler per vertex/instance data
  glBindAttribLocation(ShaderProgramId, 0, "APosition");
  glBindAttribLocation(ShaderProgramId, 1, "AColor");
  glBindAttribLocation(ShaderProgramId, 2, "AUIRect");
  glBindAttribLocation(ShaderProgramId, 3, "AUIColor");
  glLinkProgram(ShaderProgramId);
  //cleanup
  glDeleteShader(VertShader);
  glDeleteShader(FragShader);
  return ShaderProgramId;
}
u32 GfxVertexBufferCreate(void *Data, u32 Size, u32 Count)
{
  GLuint VertBufferId;
  glGenBuffers(1, &VertBufferId);
  glBindBuffer(GL_ARRAY_BUFFER, VertBufferId);
  glBufferData(GL_ARRAY_BUFFER, Size*Count, Data, GL_STATIC_DRAW);
  return VertBufferId;
}
u32 GfxInstanceBufferCreate(void *Data, u32 Size, u32 Count)
{
  GLuint InstanceBufferId;
  glGenBuffers(1, &InstanceBufferId);
  glBindBuffer(GL_ARRAY_BUFFER, InstanceBufferId);
  glBufferData(GL_ARRAY_BUFFER, Size*Count, Data, GL_DYNAMIC_DRAW);
  return InstanceBufferId;
}
u32 GfxVertexLayoutCreate(gfx_ctx *Ctx)
{
  u32 LayoutId = 0;
  //strides
  u32 VStride = sizeof(vertex);
  u32 IStride = sizeof(quad_attribs);
  //vertex buffer
  glGenVertexArrays(1, &LayoutId);
  glBindVertexArray(LayoutId);
  //enable attibutes
  glEnableVertexAttribArray(0);
  glEnableVertexAttribArray(1);
  glEnableVertexAttribArray(2);
  glEnableVertexAttribArray(3);
  u32 a = offsetof(vertex, Pos);
  u32 b = offsetof(vertex, Color);
  u32 c = offsetof(quad_attribs, Rect);
  u32 d = offsetof(quad_attribs, Color);
  //pos
  glVertexAttribFormat (0, 2, GL_FLOAT, GL_FALSE, a); 
  glVertexAttribBinding(0, 0);
  glVertexAttribDivisor(0, 0);
  //col
  glVertexAttribFormat (1, 3, GL_FLOAT, GL_FALSE, b);
  glVertexAttribBinding(1, 1);
  glVertexAttribDivisor(1, 0);
  //ui_rect
  glVertexAttribFormat (2, 4, GL_FLOAT, GL_FALSE, c);
  glVertexAttribBinding(2, 2);
  glVertexAttribDivisor(2, 1);
  //ui_color
  glVertexAttribFormat (3, 4, GL_FLOAT, GL_FALSE, 0);
  glVertexAttribBinding(3, 3);
  glVertexAttribDivisor(3, 1);
  //vbind
  glBindVertexBuffer(0, Ctx->VBufferId, 0, VStride);
  glBindVertexBuffer(1, Ctx->VBufferId, b, VStride);
  glBindVertexBuffer(2, Ctx->IBufferId, 0, IStride);
  glBindVertexBuffer(3, Ctx->IBufferId, d, IStride);
  glBindVertexArray(0);
  return LayoutId;
}
void GfxClearScreen(f32 r, f32 g, f32 b, f32 a)
{
  glClearColor(r,g,b,a);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  return;
}
#endif //GFX_H
