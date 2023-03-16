#ifndef GFX_H
#define GFX_H

#include <EGL/egl.h>
#include <GLES2/gl2.h>

typedef struct gfx_ctx gfx_ctx;
struct gfx_ctx
{
  GLuint VBufferId;
  GLuint IBufferId;
  GLuint ShaderId;
};

gfx_ctx GfxCtxInit(void)
{
  gfx_ctx Result = {0};
  return Result;
}
void GfxCtxDraw(gfx_ctx *Ctx, ui Element)
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
  glDrawArrays(GL_TRIANGLES, 0, 6);
  return;
}
#if 0
void GfxCtxDrawInstanced(gfx_ctx *Ctx, ui Element, u32 Count)
{
  gl_InstanceID;
  //instance buffer
  glEnableVertexAttribArray(2);
  glBindBuffer(GL_ARRAY_BUFFER, instanceVBO);
  glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
  glBindBuffer(GL_ARRAY_BUFFER, 0);	
  glVertexAttribDivisor(2, 1);  
  glDrawArraysInstanced(GL_TRIANGLES, 0, 6, Count);
  return;
}
#endif
u32 GfxShaderProgramCreate(const char *VertShaderSrc, s32 VertShaderSrcLength,
                           const char *FragShaderSrc, s32 FragShaderSrcLength)
{
  //Vert Compilation
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
  
  glBindAttribLocation(ShaderProgramId, 0, "APosition");
  glBindAttribLocation(ShaderProgramId, 1, "AColor");
  glLinkProgram(ShaderProgramId);
  
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
  glBufferData(GL_ARRAY_BUFFER, Size*Count, Data, GL_STATIC_DRAW);
  return InstanceBufferId;
}
#if 0
u32 GfxStorageBufferCreate(void *Data, u32 Size, u32 Count)
{
  GLuint ssbo;
  glGenBuffers(1, &ssbo);
  glBindBuffer(GL_SHADER_STORAGE_BUFFER, ssbo);
  glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(data), data​, GLenum usage); 
  //sizeof(data) only works for statically sized C/C++ arrays.
  glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 3, ssbo);
  return;
}
#endif
#endif //GFX_H
