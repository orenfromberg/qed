#include "demo.h"

bool qed::util::Shader::loadFile (const char * file)
{
	FILE *f = fopen(file, "rb");
	if (f == NULL)
	{
		printf("Error: Unable to locate shader file."); 
		return false;
	}
	fseek(f, 0, SEEK_END);
	long size = ftell(f);
	fseek(f, 0, SEEK_SET);
	_code = new char[size+1];
	fread(_code, size, 1, f);
	fclose(f);
	_code[size] = '\0';
	return true;
}

bool qed::util::Texture2D::loadTGAFile (const char * file)
{
	if(!(_surface = IMG_LoadTGA_RW(SDL_RWFromFile(file, "rb"))))
	{
		printf("IMG_LoadTGA_RW: %s\n", IMG_GetError());
		// handle error
		return false;
	}
	// Check that the image's width is a power of 2
	if ( (_surface->w & (_surface->w - 1)) != 0 ) {
		printf("warning: image.bmp's width is not a power of 2\n");
	}

	// Also check if the height is a power of 2
	if ( (_surface->h & (_surface->h - 1)) != 0 ) {
		printf("warning: image.bmp's height is not a power of 2\n");
	}

	// get the number of channels in the SDL surface
	_nOfColors = _surface->format->BytesPerPixel;
	if (_nOfColors == 4)     // contains an alpha channel
	{
		if (_surface->format->Rmask == 0x000000ff)
			_texture_format = GL_RGBA;
		else
			_texture_format = GL_BGRA;
	} 
	else if (_nOfColors == 3)     // no alpha channel
	{
		if (_surface->format->Rmask == 0x000000ff)
			_texture_format = GL_RGB;
		else
			_texture_format = GL_BGR;
	} 
	else 
	{
		printf("warning: the image is not truecolor..  this will probably break\n");
		// this error should not go unhandled
		return false;
	}
	return true;
}

bool qed::Shader::loadFile (const char * file, GLenum type)
{
	util::Shader shader;
	if (!shader.loadFile (file))
		return false;
	_type = type;
	_shader = glCreateShader (_type);
	const char * vs = shader.getCode ();
	glShaderSource (_shader, 1, &vs, NULL);
	glCompileShader (_shader);
	return true;
}

bool qed::Texture2D::init (const char * file)
{
	util::Texture2D tex;
	if (!tex.loadTGAFile (file))
		return false;
	glGenTextures (1, &_texture);
	glBindTexture (GL_TEXTURE_2D, _texture);
	glTexImage2D( GL_TEXTURE_2D, 0, tex.getInternalFormat (),
		tex.getWidth (), tex.getHeight (), 0, tex.getFormat (),
		GL_UNSIGNED_BYTE, tex.getPixels () );
	glBindTexture (GL_TEXTURE_2D, 0);
	return true;
}

void qed::Render (qed::RenderCommand & command)
{
	GLint param;
	glGetIntegerv (GL_CURRENT_PROGRAM, &param);
	if (param != command.program)
		glUseProgram (command.program);

	// uniforms
	{
		for (int i = 0; i < MAX_UNIFORMS; ++i)
		{
			Uniform * u = &(command.uniforms[i]);
			if (!u->is_valid)
				break;
			if (u->type == qed::Uniform::UniformType::MATRIX4FV)
			{
				glUniformMatrix4fv (u->location, u->count, u->transpose, u->value);
			}
			else
			{
			}
		}
	}

	// textures
	{
		for (int i = 0; i < MAX_TEXTURES; ++i)
		{
			Texture * t = &(command.textures[i]);
			if (!t->is_valid)
				break;
			glUniform1i (t->location, i);
			glActiveTexture (GL_TEXTURE0+i);
			glBindTexture (t->target, t->texture);
		}
	}

	// vertex array
	AttribArray * va = &command.vertex_array;
	glBindBuffer (va->target, va->buffer);
	glVertexAttribPointer (va->index, va->size, va->type, va->normalized, va->stride, va->pointer);
	glEnableVertexAttribArray (va->index);

	// attrib arrays
	{
		for (int i = 0; i < MAX_TEXTURES; ++i)
		{
			AttribArray * aa = &(command.attrib_array[i]);
			if (!aa->is_valid)
				break;
			glBindBuffer (aa->target, aa->buffer);
			//glVertexAttribPointer (aa->index, aa->size, aa->type, aa->normalized, aa->stride, aa->pointer);
			//glEnableClientState (aa->cap);
			//glEnableVertexAttribArray (aa->location);
			glVertexAttribPointer (aa->index, aa->size, aa->type, aa->normalized, aa->stride, aa->pointer);
			glEnableVertexAttribArray (aa->index);
		}
	}

	// indices
	IndexArray * ia = &command.index_array;
	glBindBuffer (ia->target, ia->buffer);
	
	glDrawElements (command.mode, command.count, command.type, command.indices);

	// TODO: disable client states and unbind buffers
}