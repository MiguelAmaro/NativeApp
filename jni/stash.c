{
  
#if 0
  M4Identity(&Projection);
  M4Scale   (&Projection, (v3f){0.2f, 0.5f, 1.0f});
  M4ShearX (&Projection, 0.0f);
  M4Ortho(&Projection,
          0.0f,  GlobalRes.x,
          0.0f,  GlobalRes.y, 0.0f, 1.0f);
  
  //uniforms: ui
  glUniform1fv(glGetUniformLocation(Ctx->ShaderId,   "URect"), 4*Count, UIRects->comp);
  glUniform4fv(glGetUniformLocation(Ctx->ShaderId,  "UColor"), 1*Count, UIColors->comp);
  
  
  
  //instance buffer
  u32 IInitialOffset = sizeof(r2f);
  u32 IStride = sizeof(r2f) + sizeof(v4f);
  glVertexAttribBinding(2, 1);
  glBindBuffer(GL_ARRAY_BUFFER, Ctx->IBufferId);
  glVertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE, IStride*sizeof(float), NULL);
  glVertexAttribPointer(3, 4, GL_FLOAT, GL_FALSE, IStride*sizeof(float), (void*)IInitialOffset);
  glVertexAttribDivisor(2, 1);
  glVertexAttribDivisor(3, 1);
  glEnableVertexAttribArray(2);
  glEnableVertexAttribArray(3);
  
  const char *GLGetAccessAsString(GLenum Access)
  {
    /*
  SymbolToStringHelper(GL_STREAM_READ, Usage)
                            SymbolToStringHelper(GL_STREAM_COPY, Usage)
                            SymbolToStringHelper(GL_STATIC_READ, Usage)
                            SymbolToStringHelper(GL_STATIC_COPY, Usage)
                            SymbolToStringHelper(GL_DYNAMIC_READ, Usage)
                            SymbolToStringHelper(GL_DYNAMIC_COPY, Usage)
  */
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
  GLint  Access = 0;
  GLint  IsMapped = 0;
  GLint  MapLength = 0;
  GLint  MapOffset = 0;
  glGetBufferParameteriv(GL_ARRAY_BUFFER, GL_BUFFER_MAPPED, &IsMapped);
  glGetBufferParameteriv(GL_ARRAY_BUFFER, GL_BUFFER_MAP_OFFSET, &MapOffset);
  glGetBufferParameteriv(GL_ARRAY_BUFFER, GL_BUFFER_MAP_LENGTH, &MapLength);
  glGetBufferParameteriv(GL_ARRAY_BUFFER, GL_BUFFER_ACCESS_FLAGS, &Access);
  
  //vertex buffer
  u32 VInitialOffset = sizeof(v2f);
  u32 VStride        = sizeof(v2f) + sizeof(v3f); //pos + color
  glBindBuffer(GL_ARRAY_BUFFER, Ctx->VBufferId);
  GLLogBufferState();
  glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, VStride, NULL);
  glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, VStride, (void*)VInitialOffset);
  glEnableVertexAttribArray(0);
  glEnableVertexAttribArray(1);
  //instance buffer
  u32 IInitialOffset = sizeof(r2f);
  u32 IStride = sizeof(struct ui_attribs);
  glBindBuffer(GL_ARRAY_BUFFER, Ctx->IBufferId);
  glVertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE, IStride, NULL);
  glVertexAttribPointer(3, 4, GL_FLOAT, GL_FALSE, IStride, (void*)IInitialOffset);
  glEnableVertexAttribArray(2);
  glEnableVertexAttribArray(3);
  glVertexAttribDivisor(2, 1);
  glVertexAttribDivisor(3, 1);
  //-
  
  
  //vertex buffer
  u32 VInitialOffset = sizeof(v2f);
  u32 VStride        = sizeof(v2f) + sizeof(v3f); //pos + color
  glBindVertexBuffer(0, GL_ARRAY_BUFFER, Ctx->VBufferId, VStride);
  //glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, VStride, NULL);
  //glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, VStride, (void*)VInitialOffset);
  glVertexAttribFormat(0, 2, GL_FLOAT, GL_FALSE, 0);
  glVertexAttribFormat(1, 3, GL_FLOAT, GL_FALSE, VInitialOffset);
  glVertexAttribBinding(0,0);
  glVertexAttribBinding(1,0);
  //instance buffer
  u32 IInitialOffset = sizeof(r2f);
  u32 IStride = sizeof(struct ui_attribs);
  glBindVertexBuffer(1, GL_ARRAY_BUFFER, Ctx->IBufferId, IStride);
  glVertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE, IStride, NULL);
  glVertexAttribPointer(3, 4, GL_FLOAT, GL_FALSE, IStride, (void*)IInitialOffset);
  //glVertexAttribFormat(2, 4, GL_FLOAT, GL_FALSE, offsetof(struct ui_attribs, Rect));
  //glVertexAttribFormat(3, 4, GL_FLOAT, GL_FALSE, offsetof(struct ui_attribs, Color));
  glVertexAttribBinding(2,1);
  glVertexAttribBinding(3,1);
  glVertexAttribDivisor(2, 1);
  glVertexAttribDivisor(3, 1);
  
  //enable attibutes
  glEnableVertexAttribArray(0);
  glEnableVertexAttribArray(1);
  glEnableVertexAttribArray(3);
  glEnableVertexAttribArray(2);
  //GLint LayoutId, GLint VertexBufferId, u32 *Stride
  //glVertexAttribFormat(0, 2, GL_FLOAT, GL_FALSE, 0);
  //glVertexAttribFormat(1, 3, GL_FLOAT, GL_FALSE, VInitialOffset);
  //glVertexAttribFormat(2, 4, GL_FLOAT, GL_FALSE, offsetof(struct ui_attribs, Rect));
  //glVertexAttribFormat(3, 4, GL_FLOAT, GL_FALSE, offsetof(struct ui_attribs, Color));
  
  //glBindBuffer(GL_ARRAY_BUFFER, Ctx->VBufferId);
  //glBindBuffer(GL_ARRAY_BUFFER, Ctx->IBufferId);
#endif
  // the vbuffer being bound to 1 slot vs ibuffer bound to 2 slots problem
  // - removing the mapping code dosnt stop the vertex buffer from being unbounded
  // - removing the BindBuffer(Inst & Vert) and BindVertexBuffer(0/1 Inst & Vert) calls
  //   yielded the Inst Buffer bound to slots 2 & 3 for but Vert Buffer bound to just slot 0 and
  //   inst data is garbage.
  // - removing BindVertexArray() call leaves attributes disabled with no buffers bound
  // - 
  // - 
  // - 
  // - 
  // the VertexAttribFormat causing renderdoc to crash problem
  // - adding the buffer bounding call on the draw did not solve the crash
  // - removing the insnceddraw path stoped the crashes so something is wrong there
  //   and VertexAttribFormat calls seem to be correct
  // - 
  // - 
  // - 
  // the buffermap failing problem
  // - 
  // - 
  // - 
  // - 
  // binding vertexbuffers is an invalid operation problem
  // - 
  // - 
  // - 
  // - 
  // - 
  //
  //
  GLClearErrors();
  LOG("%s: count: %d", __FUNCTION__, Count);
  //LOG("size of ui attrib: %d\n", sizeof(struct ui_attribs));
  //LOG("gl version: %s\n",glGetString(GL_VERSION));
  Element->Rect.min.x = GlobalTouchPos.x-HalfDim.x;
  Element->Rect.min.y = GlobalTouchPos.y-HalfDim.y;
  Element->Rect.max.x = GlobalTouchPos.x+HalfDim.x;
  Element->Rect.max.y = GlobalTouchPos.y+HalfDim.y;
  
  //oddly enough taking the no instance draw call path keep both buffers bounded correctly
  GfxCtxDraw(&Engine->GfxCtx, GlobalUIState.Elements[0]);
}

