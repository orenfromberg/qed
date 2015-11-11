#ifndef _DEMO_H_
#define _DEMO_H_

#include <GL/glew.h>
#include <vector>
#include <list>
#include <map>
#include <string>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "SDL.h"
#include "SDL_image.h"

#define MAX_UNIFORMS 10
#define MAX_TEXTURES 10
#define MAX_ATTRIBS 10

// this is my demo engine, called q.e.d:
// "quod erat demonstrandum"
// "that which was to be demonstrated"
// it is built on SDL and OpenGL


namespace exp_qed
{

	// uniforms are basically components of effects. they have a set of input parameters
	// and their output is a value that is directly uploadable to OpenGL through the
	// set of glUniform functions.
	class Uniform
	{
	public:
		Uniform ()
		{
		}
		Uniform (std::string name)
		{
			_name = name;
		}
		void getName (std::string & name)
		{
			name = _name;
		}
		virtual void Update () = 0;
	protected:
		std::string _name;
	};

	class UniformMat4x4: public Uniform
	{
	public:
		UniformMat4x4(std::string name)
			: Uniform (name)
		{
			_value = new GLfloat[16];
			memset ((void *)_value, 0, sizeof(GLfloat)*16);
		}
		~UniformMat4x4()
		{
			delete [] _value;
		}
	private:
		UniformMat4x4() 
		{
		}
	protected:
		const GLfloat * _value;
	};

	// examples of uniforms are: ModelViewProjMatrix, InvModelViewProjMatrix, etc..

	class ModelViewProjMatrix: public UniformMat4x4
	{
	public:
		ModelViewProjMatrix (
			std::string name,
			float fovy,
			float aspect,
			float zNear,
			float zFar,
			float eyeX,
			float eyeY,
			float eyeZ,
			float centerX, 
			float centerY,
			float centerZ, 
			float upX, 
			float upY, 
			float upZ,
			float posX,
			float posY,
			float posZ,
			float rotX,
			float rotY,
			float rotZ,
			float scaleX,
			float scaleY,
			float scaleZ)
			: UniformMat4x4 (name)
			, _fovy (fovy)
			, _aspect (aspect)
			, _zNear (zNear)
			, _zFar (zFar)
			, _eyeX (eyeX)
			, _eyeY (eyeY)
			, _eyeZ (eyeZ)
			, _centerX (centerX)
			, _centerY (centerY)
			, _centerZ (centerZ)
			, _upX (upX)
			, _upY (upY)
			, _upZ (upZ)
			, _posX (posX)
			, _posY (posY)
			, _posZ (posZ)
			, _rotX (rotX)
			, _rotY (rotY)
			, _rotZ (rotZ)
			, _scaleX (scaleX)
			, _scaleY (scaleY)
			, _scaleZ (scaleZ)
		{
		}
		~ModelViewProjMatrix ()
		{
		}
		void Update ()
		{
			// here we could run a lua script that keeps the state of the uniform
			// but for now we will hard code in C++


		// how does this uniform access the parameters? will it link to the camera to get the
			// screen parameters so it can compute the proj and view matrix? and link to the
			// gameobject for the model so it can compute its position, scale, and rotation for
			// the model matrix?

			// projection matrix 
			glm::mat4 proj =  glm::perspective<float>(_fovy, _aspect, _zNear, _zFar);

			// view matrix
			glm::vec3 eye = glm::vec3(_eyeX, _eyeY, _eyeZ);
			glm::vec3 center = glm::vec3(_centerX, _centerY, _centerZ);
			glm::vec3 up = glm::vec3(_upX, _upY, _upZ);
			glm::mat4 view = glm::lookAt<float>(eye, center, up);
			
			// not sure what the order is for the model matrix computation...
			glm::mat4 model = glm::mat4 (1.0);
			glm::vec3 translation = glm::vec3(_posX, _posY, _posZ);
			glm::vec3 scale = glm::vec3(_scaleX, _scaleY, _scaleZ);
			model = glm::rotate<float>(model, _rotZ, glm::vec3(0, 0, 1));
			model = glm::rotate<float>(model, _rotY, glm::vec3(0, 1, 0));
			model = glm::rotate<float>(model, _rotX, glm::vec3(1, 0, 0));
			model = glm::translate<float>(model, translation);
			model = glm::scale<float>(model, scale);

			// now we compute the model view projection matrix
			glm::mat4 mvp = proj * view * model; // did it work!?
			// here we should not access _value directly. use a setter method that is inherited from Matrix4x4.
			memcpy ((void *)_value, glm::value_ptr(mvp), sizeof(GLfloat) * 16);
		}
	private:
		// for projection matrix
		float _fovy;
		float _aspect;
		float _zNear;
		float _zFar;
		// for view matrix
		float _eyeX;
		float _eyeY;
		float _eyeZ;
		float _centerX;
		float _centerY;
		float _centerZ;
		float _upX;
		float _upY; 
		float _upZ;
		// for model matrix
		float _posX;
		float _posY;
		float _posZ;
		float _rotX;
		float _rotY;
		float _rotZ;
		float _scaleX;
		float _scaleY;
		float _scaleZ;
	};
};

namespace qed
{
	struct Texture
	{
		bool is_valid;
		GLenum target;
		GLuint texture;
		GLint location;
	};

	struct Uniform
	{
		bool is_valid;
		enum UniformType
		{
			MATRIX4FV
		};
		UniformType type;
		GLint location;
		GLsizei count;
		GLfloat v0, v1, v2, v3;
		GLboolean transpose;
		const GLfloat * value;
	};

	struct VertexArray
	{
		GLenum target;
		GLuint buffer;
		GLint location;
		GLint size;
		GLenum type;
		GLsizei stride;
		const GLvoid * pointer;
	};

	struct AttribArray
	{
		bool is_valid;
		GLuint index;
		GLenum target;
		GLuint buffer;
		GLint location;
		GLint size;
		GLenum type;
		GLboolean normalized;
		GLsizei stride;
		GLenum cap;
		const GLvoid * pointer;
	};

	struct IndexArray
	{
		GLenum target;
		GLuint buffer;
		//GLenum type;
		//GLsizei stride;
		//const GLvoid * pointer;
	};

	struct RenderCommand
	{
		bool is_valid;
		GLuint program;
		Texture textures[MAX_TEXTURES];
		Uniform uniforms[MAX_UNIFORMS];
		//VertexArray vertex_array;
		AttribArray vertex_array;
		AttribArray attrib_array[MAX_ATTRIBS];
		IndexArray index_array;
		GLenum mode;
		GLsizei count;
		GLenum type;
		const GLvoid * indices;
	};

	void Render (RenderCommand & command);

	class GL_State
	{
	public:
		GL_State()
			: _clearColor (0.0, 0.0, 0.0, 0.0)
			, _depthTestEnabled (false)
		{
		}
		glm::vec4 getClearColor () const
		{
			return _clearColor;
		}
		void setClearColor (glm::vec4 cc)
		{
			_clearColor = cc;
		}
		bool getDepthTestEnabled () const
		{
			return _depthTestEnabled;
		}
		void setDepthTestEnabled (bool val)
		{
			_depthTestEnabled = val;
		}
	protected:
		glm::vec4 _clearColor;
		bool _depthTestEnabled;
	};

	class DrawState
	{
	protected:
		GLuint _program;
		GLuint _vertexBuffer;
		//std::list <GLuint>
		GLuint _indexBuffer;

	};

	namespace util
	{
		class Shader
		{
		public:
			Shader ()
			: _code (NULL)
			{}
			~Shader ()
			{
				delete [] _code;
			}
			bool loadFile (const char * file);
			char * getCode () const
			{
				return _code;
			}
		protected:
			char * _code;
		};

		class Texture2D
		{
		public:
			Texture2D ()
			: _surface (NULL)
			, _texture_format(0)
			, _nOfColors (0)
			{}
			bool loadTGAFile (const char * file);
			~Texture2D () 
			{
				SDL_FreeSurface (_surface);
			}
			GLint getInternalFormat ()
			{
				return _nOfColors;
			}
			GLsizei getWidth ()
			{
				return _surface->w;
			}
			GLsizei getHeight ()
			{
				return _surface->h;
			}
			const GLvoid * getPixels ()
			{
				return _surface->pixels;
			}
			GLenum getFormat ()
			{
				return _texture_format;
			}
		protected:
			SDL_Surface * _surface;
			GLenum _texture_format;
			GLint _nOfColors;
		};
	};

	namespace manager
	{
		class VertexShader
		{
		protected:
			GLuint load (const char * file)
			{
				GLuint ret = 0;
				util::Shader shader;
				if (!shader.loadFile (file))
				{
					// shader file could not be loaded
					return ret;
				}
				ret = glCreateShader (GL_VERTEX_SHADER);
				if (ret == 0)
				{
					// glCreateShader failed
					return ret;
				}
				const char * vs = shader.getCode ();
				glShaderSource (ret, 1, &vs, NULL);
				glCompileShader (ret);
				GLint compiled;
				glGetShaderiv (ret, GL_COMPILE_STATUS, &compiled);
				if(!compiled)
				{
					GLint length;
					GLchar * log;
					glGetShaderiv (ret, GL_INFO_LOG_LENGTH, &length);
					log = (GLchar *)malloc (length);
					glGetShaderInfoLog (ret, length, &length, log);
					//std::cout << "compile log: " << log << std::endl;
					free(log);
					glDeleteShader (ret);
					return 0;
				}
				std::string name(file);
				vertexShaders[name] = ret;
				return ret;
			}
		public:
			GLuint getHandle (const char * file)
			{
				std::string name(file);
				GLuint ret = vertexShaders[name];
				if(ret)
					return ret;
				else
					return load (file);
			}
		protected:
			std::map <std::string, GLuint> vertexShaders;
		};
	};
// this is bad
	//class VertexShader
	//{
	//public:
	//	static GLuint getShader (const char * file)
	//	{
	//		//check to see if in map
	//		if(vertexShaders[file])
	//			return vertexShaders[file];
	//		return 0;
	//	}
	//protected:
	//	static std::map<std::string, GLuint> vertexShaders;
	//};

	// these classes are bad, they need to disappear
	// instead we need to manage all the handles
	// to resources that opengl give us.
	class Shader
	{
	public:
		Shader () {}
		~Shader ()
		{
			glDeleteShader (_shader);
		}
		bool loadFile (const char * file, GLenum type);
		GLuint getShader ()
		{
			return _shader;
		}
	protected:
		GLuint _shader;
		GLenum _type;
		//static std::map<std::string, GLuint> shaders;
	};

	class Program
	{
	public:
		Program () {}
		~Program ()
		{
			glDeleteProgram (_program);
		}
		bool init ()
		{
			_program = glCreateProgram ();
			return true;
		}
		bool setVertexShader (GLuint shader)
		{
			glAttachShader (_program, shader);
			glLinkProgram (_program);
			return true;
		}
		bool setVertexShader (Shader * vertexShader)
		{
			glAttachShader (_program, vertexShader->getShader ());
			glLinkProgram (_program);
			return true;
		}
		bool setFragmentShader (Shader * fragmentShader)
		{
			glAttachShader (_program, fragmentShader->getShader ());
			glLinkProgram (_program);
			return true;
		}
		void bind ()
		{
			glUseProgram (_program);
		}
		void unbind ()
		{
			glUseProgram (0);
		}
		GLint getAttribLocation (const char * name)
		{
			return glGetAttribLocation (_program, name);
		}
		GLint getUniformLocation (const char * name)
		{
			return glGetUniformLocation (_program, name);
		}
	protected:
		GLuint _program;
	};

	class Texture2D
	{
	public:
		Texture2D ()
			: _surface (NULL)
			, _texture (0)
			, _index (0)
			, _texture_format (0)
			, _nOfColors (0)
		{}
		~Texture2D () {}
#if 1
		bool init (const char * file);
#else
		bool init (Program * program, const char * name, const char * file);
		//bool init (Program * program, const char * name, const GLvoid * data);
#endif
		void bind ()
		{
			glBindTexture (GL_TEXTURE_2D, _texture);
		}
		void unbind ()
		{
			glBindTexture (GL_TEXTURE_2D, 0);
		}
		void TexParameteri (GLenum pname, GLint param)
		{
			bind ();
			glTexParameteri (GL_TEXTURE_2D, pname, param);
			unbind ();
		}
		GLuint getTexture ()
		{
			return _texture;
		}
	protected:
		SDL_Surface * _surface;
		GLuint _texture;
		GLuint _index;
		GLenum _texture_format;
		GLint _nOfColors;
	};

	class StaticArrayBuffer
	{
	public:
		StaticArrayBuffer () {}
		~StaticArrayBuffer () {}
		bool init (Program * program, 
			const char * name, 
			const GLvoid * data)
		{
			glGenBuffers (1, &_buffer);
			bind ();
			glBufferData (GL_ARRAY_BUFFER, sizeof(data), data, GL_STATIC_DRAW);
			unbind ();

			_index = program->getAttribLocation (name);
			if (_index == -1)
				return false;
			return true;
		}
		void bind ()
		{
			glBindBuffer (GL_ARRAY_BUFFER, _buffer);
		}
		void unbind ()
		{
			glBindBuffer (GL_ARRAY_BUFFER, 0);
		}
	protected:
		GLuint _index;
		GLuint _buffer;
	};

	class StaticElementArrayBuffer
	{
	};
};

// old stuff

// Projection as well as Modelview matrices are just uniforms ;)
class Projection
{
public:
	Projection () {}
	Projection (GLdouble left, GLdouble right,
		GLdouble bottom, GLdouble top,
		GLdouble near, GLdouble far)
		: _left (left)
		, _right (right)
		, _bottom (bottom)
		, _top (top)
		, _near (near)
		, _far (far)
	{
	}
	virtual void apply () = 0;
protected:
	GLdouble _left, _right, _bottom, _top, _near, _far;
};

class Vec4
{
public:
	Vec4 ()
	{}
	Vec4 (float x, float y, float z, float w)
	{
		data[0] = x;
		data[1] = y;
		data[2] = z;
		data[3] = w;
	}
	float x() { return data[0]; }
	float y() { return data[1]; }
	float z() { return data[2]; }
	float w() { return data[3]; }
public:
	float data[4];
};

// a vertex is the set of data which includes a position in space and any number of attributes.
class Vertex2
{
public:
	// data[0][0-2] = x, y, z position
	// data[0][3] = bone index
	// data[1][0,1] = x, y normal, z is sqrt(1-(x^2+y^2))
	// data[1][2,3] = u, v tex coord
	Vec4 data[2];
};

class ClientMesh
{
public:
	std::vector<Vertex2> verts;
	std::vector<int> idxs;
};

class Drawable
{
public:
	Drawable ();
	virtual void draw ()
	{
		glUseProgram (_program);
	}
protected:
	GLuint _program; // shader program to use
	GLuint vertices; // array buffer
	GLuint colors; // colors
	GLuint normals; // normals
	GLuint texs; // texture  
	GLuint indices; // index buffer 

	// all the uniforms
	//std::vector<Uniform> uniforms;
};

class QuadsOrthographicGeometry: public Drawable
{
public:
	QuadsOrthographicGeometry ()
	{}
protected:
};

class TriStripPerspectiveGeometry: public Drawable
{
public:
	TriStripPerspectiveGeometry ()
	{}
protected:
};

class Renderer // wrapper for OpenGL
{
public:
	Renderer ():
	  mClearColor (.5, .5, .5, 1)
	  {
	  }

	  bool init ()
	  {
		  glewInit ();

		  //set clear color
		  //glClearColor( .5, .5, .5, 1);
		  glClearColor (mClearColor.x(),
			  mClearColor.y(),
			  mClearColor.z(),
			  mClearColor.w());

		  //init projection matrix
		  glMatrixMode (GL_PROJECTION);
		  glLoadIdentity();

#if 0 // I don't want this here, this is per effect
		  glOrtho(0, SCREEN_WIDTH, SCREEN_HEIGHT, 0, -1, 1);
#endif

		  //init modelview matrix
		  glMatrixMode (GL_MODELVIEW);
		  glLoadIdentity();


		  //If there was any errors
		  if(glGetError() != GL_NO_ERROR)
		  {
			  return false;
		  }

		  //If everything initialized
		  return true;
	  }

	  void draw () // this will draw a frame
	  {
		  //Clear the screen
		  glClear (GL_COLOR_BUFFER_BIT);

		  // loop through the objects to be rendered
		  // do we do draw(object) or object.draw()?

		  //Update screen
		  SDL_GL_SwapBuffers();
	  }
protected:
	// is this per effect or per rendering context?
	Vec4 mClearColor;

	// list of objects to be rendered?
	std::vector <Drawable> objects;
};
#endif // _DEMO_H_
