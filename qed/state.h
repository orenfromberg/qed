#ifndef _STATE_H_
#define _STATE_H_

typedef struct
{
  // Current Values and Associated Data
  GLfloat _glCurrentColor[4];
  GLfloat _glCurrentSecondaryColor[4];
  GLint _glCurrentIndex[1];
  GLfloat _glCurrentTextureCoords[4];
  GLfloat _glCurrentNormal[3];
  GLfloat _glCurrentFogCoord[1];
  GLfloat _glCurrentRasterPosition[4];
  GLfloat _glCurrentRasterDistance[1];
  GLfloat _glCurrentRasterColor[4];
  GLfloat _glCurrentRasterSecondaryColor[4];
  GLint _glCurrentRasterIndex[1];
  GLfloat _glCurrentRasterTextureCoords[4];
  GLboolean _glCurrentRasterPositionValid;
  GLboolean _glEdgeFlag;

  // Vertex Array Data State
  GLint _glClientActiveTexture[1];
  GLint _glArrayBufferBinding[1];
  GLboolean _glPrimitiveRestart;
  GLint _glPrimitiveRestartIndex[1];

  // Vertex Array Object State
  GLboolean _glVertexArray;
  GLint _glVertexArrayBinding[1];
  GLint _glVertexArraySize[1];
  GLint _glVertexArrayType[1];
} State;

void get_initial_state (State * s)
{
  // Current Values and Associated Data
  glGetFloatv (GL_CURRENT_COLOR, s->_glCurrentColor);
  glGetFloatv (GL_CURRENT_SECONDARY_COLOR, s->_glCurrentSecondaryColor);
  glGetIntegerv (GL_CURRENT_INDEX, s->_glCurrentIndex);
  glGetFloatv (GL_CURRENT_TEXTURE_COORDS, s->_glCurrentTextureCoords);
  glGetFloatv (GL_CURRENT_NORMAL, s->_glCurrentNormal);
  glGetFloatv (GL_CURRENT_FOG_COORD, s->_glCurrentFogCoord);
  glGetFloatv (GL_CURRENT_RASTER_POSITION, s->_glCurrentRasterPosition);
  glGetFloatv (GL_CURRENT_RASTER_DISTANCE, s->_glCurrentRasterDistance);
  glGetFloatv (GL_CURRENT_RASTER_COLOR, s->_glCurrentRasterColor);
  glGetFloatv (GL_CURRENT_RASTER_SECONDARY_COLOR, s->_glCurrentRasterSecondaryColor);
  glGetIntegerv (GL_CURRENT_RASTER_INDEX, s->_glCurrentRasterIndex);
  glGetFloatv (GL_CURRENT_RASTER_TEXTURE_COORDS, s->_glCurrentRasterTextureCoords);
  glGetBooleanv (GL_CURRENT_RASTER_POSITION_VALID, &s->_glCurrentRasterPositionValid);
  glGetBooleanv (GL_EDGE_FLAG, &s->_glEdgeFlag);

  // Vertex Array Data State
  glGetIntegerv (GL_CLIENT_ACTIVE_TEXTURE, s->_glClientActiveTexture);
  glGetIntegerv (GL_ARRAY_BUFFER_BINDING, s->_glArrayBufferBinding);
  //glGetBooleanv (GL_PRIMITIVE_RESTART, &s->_glPrimitiveRestart);
  //glGetIntegerv (GL_PRIMITIVE_RESTART_INDEX, s->_glPrimitiveRestartIndex);

  // Vertex Array Object State
  glGetBooleanv (GL_VERTEX_ARRAY, &s->_glVertexArray);
  //glGetIntegerv (GL_VERTEX_ARRAY_BINDING, s->_glVertexArrayBinding);
  glGetIntegerv (GL_VERTEX_ARRAY_SIZE, s->_glVertexArraySize);
  glGetIntegerv (GL_VERTEX_ARRAY_TYPE, s->_glVertexArrayType);
}


#endif // _STATE_H_
