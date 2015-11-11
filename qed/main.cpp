#include <GL/glew.h>
#include <GL/wglew.h>
#include <GL/gl.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "SDL.h"
#include "SDL_image.h"
#include "demo.h"
#include "data.h"

#define BUFFER_OFFSET(bytes) ((GLubyte*) NULL + (bytes))

enum {VERTICES = 0, UVS, INDICES, NUM_BUFFERS};

//Screen attributes
const int SCREEN_WIDTH = 600;
const int SCREEN_HEIGHT = 800;
//const int SCREEN_WIDTH = 1920;
//const int SCREEN_HEIGHT = 1080;
const int SCREEN_BPP = 32;
//The frame rate
const int FRAMES_PER_SECOND = 60;
//The attributes of the player
const int PLAYER_WIDTH = 20;
const int PLAYER_HEIGHT = 20;

//The timer
class Timer
{
private:
	//The clock time when the timer started
	int startTicks;

	//The ticks stored when the timer was paused
	int pausedTicks;

	//The timer status
	bool paused;
	bool started;

public:
	//Initializes variables
	Timer()
	{
		//Initialize the variables
		startTicks = 0;
		pausedTicks = 0;
		paused = false;
		started = false;
	}

	//The various clock actions
	void start()
	{
		//Start the timer
		started = true;

		//Unpause the timer
		paused = false;

		//Get the current clock time
		startTicks = SDL_GetTicks();
	}
	void stop()
	{
		//Stop the timer
		started = false;

		//Unpause the timer
		paused = false;
	}
	void pause()
	{
		//If the timer is running and isn't already paused
		if( ( started == true ) && ( paused == false ) )
		{
			//Pause the timer
			paused = true;

			//Calculate the paused ticks
			pausedTicks = SDL_GetTicks() - startTicks;
		}
	}
	void unpause()
	{
		//If the timer is paused
		if( paused == true )
		{
			//Unpause the timer
			paused = false;

			//Reset the starting ticks
			startTicks = SDL_GetTicks() - pausedTicks;

			//Reset the paused ticks
			pausedTicks = 0;
		}
	}

	//Gets the timer's time
	int get_ticks()
	{
		//If the timer is running
		if( started == true )
		{
			//If the timer is paused
			if( paused == true )
			{
				//Return the number of ticks when the timer was paused
				return pausedTicks;
			}
			else
			{
				//Return the current time minus the start time
				return SDL_GetTicks() - startTicks;
			}
		}

		//If the timer isn't running
		return 0;
	}

	//Checks the status of the timer
	bool is_started()
	{
		return started;
	}
	bool is_paused()
	{
		return paused;
	}
};

// global declarations
GLuint vertexBuffer;
GLuint uvBuffer;
GLuint indexBuffer;
#if 0
qed::Shader * vertexShader;
#else
GLuint vertexShader;
# if 0
qed::manager::VertexShader vertexShaderManager;
# endif
#endif
GLuint fragmentShader;
GLuint program;
qed::Texture2D * colorsTexture;
qed::Texture2D * normalsTexture;
GLint attrib_tex_index;
GLint attrib_vertex_index;
GLint uniform_index_texture;
GLint uniform_index_texture_normals;
GLint uniform_index_frame_number;
GLuint uniform_index_mvp = -1;
int x;
int y;
int xVel;
int yVel;
float xrot = 270.0f;
float yrot = 0.0f;
float xrotvel = 0.0f;
float yrotvel = 0.0f;
bool quit;
SDL_Event event;
Timer fps; //The frame rate regulator
qed::GL_State gl_state; // opengl state

bool init_GL ()
{
	// Initialize OpenGL entry points.
	if (glewInit()!=GLEW_OK || !GLEW_VERSION_1_1) 
	{
		fprintf(stderr, "failed to initialize GLEW, OpenGL 1.1 required.\n");    
		exit(1);
	}

	std::string renderer((const char *)glGetString (GL_RENDERER));

	// load up the ASSETS:
	// shaders
	// textures
	// model data
	//  verts
	//  texcoords
	//  indices
	// uniforms? (scripts)

	// load all resources (shaders, textures, vertices, attribs)
	// to the GPU

	// vertex shader
#if 0
	vertexShader = vertexShaderManager.getHandle ("shader.vert");
#else
	{
		qed::util::Shader shader;
		if (!shader.loadFile ("C:\\Users\\Oren\\Documents\\Visual Studio 2010\\Projects\\qed\\qed\\shader.vert"))
			return false;
		vertexShader = glCreateShader (GL_VERTEX_SHADER);
		const char * vs = shader.getCode ();
		glShaderSource (vertexShader, 1, &vs, NULL);
		glCompileShader (vertexShader);
	}
#endif

	// fragment shader
	{
		qed::util::Shader shader;
		if (!shader.loadFile ("C:\\Users\\Oren\\Documents\\Visual Studio 2010\\Projects\\qed\\qed\\shader.frag"))
			return false;
		fragmentShader = glCreateShader (GL_FRAGMENT_SHADER);
		const char * vs = shader.getCode ();
		glShaderSource (fragmentShader, 1, &vs, NULL);
		glCompileShader (fragmentShader);
	}

	// opengl pipeline program
	program = glCreateProgram ();
	glAttachShader (program, vertexShader);
	glAttachShader (program, fragmentShader);
	glLinkProgram (program);

	// set buffer with vertex data
	glGenBuffers (1, &vertexBuffer);
	glBindBuffer (GL_ARRAY_BUFFER, vertexBuffer);
	glBufferData (GL_ARRAY_BUFFER, sizeof(model_vertices), model_vertices, 
		GL_STATIC_DRAW);

	attrib_vertex_index = glGetAttribLocation (program, "vertex");

	glGenBuffers (1, &uvBuffer);
	glBindBuffer (GL_ARRAY_BUFFER, uvBuffer);
	glBufferData (GL_ARRAY_BUFFER, sizeof(model_uvs), model_uvs, 
		GL_STATIC_DRAW);

	attrib_tex_index = glGetAttribLocation (program, "texCoord");

	// set buffer with index data
	glGenBuffers (1, &indexBuffer);
	glBindBuffer (GL_ELEMENT_ARRAY_BUFFER, indexBuffer);
	glBufferData (GL_ELEMENT_ARRAY_BUFFER, sizeof(model_indices), 
		model_indices, GL_STATIC_DRAW);

#if 1
	colorsTexture = new qed::Texture2D;
	if (!colorsTexture->init ("hellknight.tga"))
		delete colorsTexture;
	else
	{
		colorsTexture->TexParameteri (GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		colorsTexture->TexParameteri (GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		colorsTexture->TexParameteri (GL_TEXTURE_WRAP_S, GL_REPEAT);
		colorsTexture->TexParameteri (GL_TEXTURE_WRAP_T, GL_REPEAT);
	}
	uniform_index_texture = glGetUniformLocation (program, "tex");

	normalsTexture = new qed::Texture2D;
	normalsTexture->init ("hellknight_local.tga");
	normalsTexture->TexParameteri (GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	normalsTexture->TexParameteri (GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	normalsTexture->TexParameteri (GL_TEXTURE_WRAP_S, GL_REPEAT);
	normalsTexture->TexParameteri (GL_TEXTURE_WRAP_T, GL_REPEAT);
	uniform_index_texture_normals = glGetUniformLocation (program, "texNorms");
#else
	colorsTexture = new qed::Texture2D;
	if (!colorsTexture->init (program, "tex", "hellknight.tga"))
		return false;
	colorsTexture->TexParameteri (GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	colorsTexture->TexParameteri (GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	colorsTexture->TexParameteri (GL_TEXTURE_WRAP_S, GL_REPEAT);
	colorsTexture->TexParameteri (GL_TEXTURE_WRAP_T, GL_REPEAT);

	normalsTexture = new qed::Texture2D;
	if (!normalsTexture->init (program, "texNorms", "hellknight_local.tga"))
		return false;
	normalsTexture->TexParameteri (GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	normalsTexture->TexParameteri (GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	normalsTexture->TexParameteri (GL_TEXTURE_WRAP_S, GL_REPEAT);
	normalsTexture->TexParameteri (GL_TEXTURE_WRAP_T, GL_REPEAT);
#endif

	uniform_index_mvp = glGetUniformLocation (program, "mvp");


	//If there was any errors
	if(glGetError() != GL_NO_ERROR)
	{
		return false;
	}

	//If everything initialized
	return true;
}

bool init ()
{
	//Initialize SDL
	if( SDL_Init( SDL_INIT_EVERYTHING ) < 0 )
	{
		return false;
	}

	//Create Window
#if 1
	if( SDL_SetVideoMode( SCREEN_WIDTH, SCREEN_HEIGHT, SCREEN_BPP, SDL_OPENGL ) == NULL )
#else // do fullscreen
	if( SDL_SetVideoMode( SCREEN_WIDTH, SCREEN_HEIGHT, SCREEN_BPP, SDL_FULLSCREEN | SDL_OPENGL | SDL_HWSURFACE) == NULL )
#endif
	{
		return false;
	}

	//Initialize OpenGL
	if( init_GL() == false )
	{
		return false;
	}

	//Set caption
	//SDL_WM_SetCaption( "qed engine", NULL );
	SDL_WM_SetCaption((const char *)glGetString(GL_VERSION), NULL );

	return true;
}

void handle_input ()
{
	//While there are events to handle
	while (SDL_PollEvent(&event))
	{
		
		if (event.type == SDL_KEYDOWN)
		{
		}
		else if (event.type == SDL_KEYUP)
		{
			switch (event.key.keysym.sym)
			{
			case SDLK_ESCAPE:
				quit = true;
				break;
			}
		}
		else if (event.type == SDL_QUIT)
		{
			quit = true;
		}
		
		//If a key was pressed
		if( event.type == SDL_KEYDOWN )
		{
			//Adjust the velocity
			switch( event.key.keysym.sym )
			{
			case SDLK_UP: 
				yVel -= PLAYER_HEIGHT / 2; 
				xrotvel -= 2;
				break;
			case SDLK_DOWN: 
				yVel += PLAYER_HEIGHT / 2; 
				xrotvel += 2;
				break;
			case SDLK_LEFT: 
				xVel -= PLAYER_WIDTH / 2; 
				yrotvel -= 2;
				break;
			case SDLK_RIGHT: 
				xVel += PLAYER_WIDTH / 2; 
				yrotvel += 2;
				break;
			}
		}
		//If a key was released
		else if( event.type == SDL_KEYUP )
		{
			//Adjust the velocity
			switch( event.key.keysym.sym )
			{
			case SDLK_UP: 
				yVel += PLAYER_HEIGHT / 2; 
				xrotvel += 2;
				break;
			case SDLK_DOWN: 
				yVel -= PLAYER_HEIGHT / 2; 
				xrotvel -= 2;
				break;
			case SDLK_LEFT: 
				xVel += PLAYER_WIDTH / 2; 
				yrotvel += 2;
				break;
			case SDLK_RIGHT: 
				xVel -= PLAYER_WIDTH / 2; 
				yrotvel -= 2;
				break;
			}
		}
	}
}

void clean_up()
{
	//Quit SDL
	SDL_Quit();
}

void update ()
{
	//Move the player left or right
	x += xVel;

	//If the player went too far
	if( ( x < 0 ) || ( x + PLAYER_WIDTH > SCREEN_WIDTH ) )
	{
		//Move back
		x -= xVel;
	}

	//Move the player up or down
	y += yVel;

	//If the player went too far
	if( ( y < 0 ) || ( y + PLAYER_HEIGHT > SCREEN_HEIGHT ) )
	{
		//Move back
		y -= yVel;
	}

	xrot += xrotvel;
	yrot += yrotvel;
}

void draw ()
{
	// renderer commands
	// - clear screen
	// - enable depth testing
	// - swap buffers
	// - draw effect

// TODO: move some of the initGL stuff in here? lazy state changing (only if we need to)


	// first clear the screen
	//Set clear color
	{
		glm::vec4 clearColor = glm::vec4(.5,.5,.5,1);
		GLbitfield mask = GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT;
		float params[4];
		glGetFloatv (GL_COLOR_CLEAR_VALUE, params);
		glm::vec4 cc (params[0], params[1], params[2], params[3]);
		if (cc != clearColor)
		{
			glClearColor (clearColor[0], clearColor[1], clearColor[2], clearColor[3]);
		}
		glClear (mask);
	}

	// enable depth testing
	{
		if (!glIsEnabled (GL_DEPTH_TEST))
			glEnable(GL_DEPTH_TEST);
	}

	// enable 2d texturing
	{
		if (!glIsEnabled (GL_TEXTURE_2D))
			glEnable (GL_TEXTURE_2D);
	}

	qed::RenderCommand command, command2;
	// init to zero
	memset (&command, 0, sizeof(qed::RenderCommand));

	command.is_valid = true;
	command.program = program;

	command.textures[0].is_valid = true;
	command.textures[0].target = GL_TEXTURE_2D;
	command.textures[0].texture = colorsTexture->getTexture ();
	command.textures[0].location = uniform_index_texture;

	command.textures[1].is_valid = true;
	command.textures[1].target = GL_TEXTURE_2D;
	command.textures[1].texture = normalsTexture->getTexture ();
	command.textures[1].location = uniform_index_texture_normals;
	
	command.uniforms[0].is_valid = true;
	command.uniforms[0].location = uniform_index_mvp;
	command.uniforms[0].count = 1;
	command.uniforms[0].transpose = GL_FALSE;
	
	// projection
	glm::mat4 proj =  glm::perspective<float>(45.0, (float)SCREEN_WIDTH/(float)SCREEN_HEIGHT, 0.1, 1000.0);

	// view matrix
	glm::vec3 eye = glm::vec3(0, 75, 250);
	glm::vec3 center = glm::vec3(0, 75, 0);
	glm::vec3 up = glm::vec3(0,1,0);
	glm::mat4 view = glm::lookAt<float>(eye, center, up);

	// model matrix
	//glm::vec3 translation = glm::vec3(0.0, -75.0, -10.0);
	glm::vec3 translation = glm::vec3(0.0, 0.0, 0.0);
	glm::mat4 model = glm::mat4 (1.0);
	model = glm::rotate<float>(glm::mat4(1.0f), yrot, glm::vec3(0, 1, 0));
	model = glm::rotate<float>(model, xrot, glm::vec3(1, 0, 0));
	model = glm::rotate<float>(model, 270, glm::vec3(1, 0, 0));
	model = glm::translate<float>(model, translation);

	// mvp is a local variable so if you put this into a function,
	// it will be deleted upon leaving scope
	glm::mat4 mvp = proj * view * model;
	command.uniforms[0].value = glm::value_ptr (mvp);
	
	command.vertex_array.target = GL_ARRAY_BUFFER;
	command.vertex_array.buffer = vertexBuffer;
	command.vertex_array.index = attrib_vertex_index;
	command.vertex_array.size = 3;
	command.vertex_array.type = GL_FLOAT;
	command.vertex_array.normalized = GL_FALSE;
	command.vertex_array.stride = 0;
	command.vertex_array.pointer = BUFFER_OFFSET(0);

	command.attrib_array[0].is_valid = true;
	command.attrib_array[0].target = GL_ARRAY_BUFFER;
	command.attrib_array[0].buffer = uvBuffer;
	command.attrib_array[0].index = attrib_tex_index;
	command.attrib_array[0].size = 2;
	command.attrib_array[0].type = GL_FLOAT;
	command.attrib_array[0].normalized = GL_FALSE;
	command.attrib_array[0].stride = 0;
	command.attrib_array[0].pointer = BUFFER_OFFSET(0);

	command.index_array.target = GL_ELEMENT_ARRAY_BUFFER;
	command.index_array.buffer = indexBuffer;

	command.mode = GL_TRIANGLES;
	command.count = sizeof(model_indices)/sizeof (int);
	command.type = GL_UNSIGNED_INT;
	command.indices = BUFFER_OFFSET(0);

#if 0
	memcpy (&command2, &command, sizeof (qed::RenderCommand));
		// model matrix
	glm::vec3 translation = glm::vec3(0.0, 0.0, 0.0);
	glm::mat4 model = glm::mat4 (1.0);
	translation = glm::vec3(100.0, 0, -10.0);
	model = glm::rotate<float>(glm::mat4(1.0f), yrot, glm::vec3(0, 1, 0));
	model = glm::rotate<float>(model, xrot, glm::vec3(1, 0, 0));
	model = glm::translate<float>(model, translation);
	// projection
	glm::mat4 proj =  glm::perspective<float>(45.0, (float)SCREEN_WIDTH/(float)SCREEN_HEIGHT, 0.1, 1000.0);

	// view matrix
	glm::vec3 eye = glm::vec3(0, 75, 250);
	glm::vec3 center = glm::vec3(0, 75, 0);
	glm::vec3 up = glm::vec3(0,1,0);
	glm::mat4 view = glm::lookAt<float>(eye, center, up);

	glm::mat4 mvp2 = proj * view * model;
	command2.uniforms[0].value = glm::value_ptr(mvp2);
#endif
#if 0
	// setting all uniforms
	// BIND PROGRAM FIRST BEFORE SENDING UNIFORMS!
	{
		GLuint prog = program;
		GLint param;
		glGetIntegerv (GL_CURRENT_PROGRAM, &param);
		if (param != prog)
			glUseProgram (prog);
	}

	// projection
	glm::mat4 proj =  glm::perspective<float>(45.0, (float)SCREEN_WIDTH/(float)SCREEN_HEIGHT, 0.1, 1000.0);

	// view matrix
	glm::vec3 eye = glm::vec3(0, -25, 200);
	glm::vec3 center = glm::vec3(0, 0, 0);
	glm::vec3 up = glm::vec3(0,0,1);
	glm::mat4 view = glm::lookAt<float>(eye, center, up);

	// model matrix
	glm::vec3 translation = glm::vec3(0.0, -75.0, -10.0);
	glm::mat4 model = glm::translate<float>(glm::mat4(1.0f), translation);
	model = glm::rotate<float>(model, yrot, glm::vec3(0, 1, 0));
	model = glm::rotate<float>(model, xrot, glm::vec3(1, 0, 0));

	glm::mat4 mvp = proj * view * model;
	glUniformMatrix4fv (uniform_index_mvp, 1, GL_FALSE, glm::value_ptr (mvp));

	// textures
	
	// bind texture contexts
	glUniform1i (uniform_index_texture, 0);
	glActiveTexture (GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, colorsTexture->getTexture ());

	glUniform1i (uniform_index_texture_normals, 1);
	glActiveTexture (GL_TEXTURE1);
	glBindTexture (GL_TEXTURE_2D, normalsTexture->getTexture ());

	// bind buffers and set pointers to buffer data
	// & enable client states

	// verts
	glBindBuffer (GL_ARRAY_BUFFER, vertexBuffer);
	glVertexAttribPointer (attrib_vertex_index, 3, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));
	glEnableVertexAttribArray (attrib_vertex_index);

	// uvs
	glBindBuffer (GL_ARRAY_BUFFER, uvBuffer);
	glVertexAttribPointer (attrib_tex_index, 2, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));
	glEnableVertexAttribArray (attrib_tex_index);

	// indices
	glBindBuffer (GL_ELEMENT_ARRAY_BUFFER, indexBuffer);

	// draw
	glDrawElements (GL_TRIANGLES, sizeof(model_indices)/sizeof(int), GL_UNSIGNED_INT, BUFFER_OFFSET(0));

	//disable client states
	glDisableVertexAttribArray (attrib_vertex_index);
	glDisableVertexAttribArray (attrib_tex_index);

	// unbind buffers
	glBindBuffer (GL_ARRAY_BUFFER, 0);
	glBindBuffer (GL_ELEMENT_ARRAY_BUFFER, 0);
	
	// go back to fixed function
	// don't really need this
#if 1
	{
		GLuint prog = 0;
		GLint param;
		glGetIntegerv (GL_CURRENT_PROGRAM, &param);
		if (param != prog)
			glUseProgram (prog);
	}
#endif
#endif

	qed::Render (command);
#if 0
	qed::Render (command2);
#endif

	//Update screen
	SDL_GL_SwapBuffers();
}

int main(int argc, char ** argv)
{
	//Initialize
	if(init() == false)
	{
		clean_up ();
		return 1;
	}

	//Wait for user exit
	while (quit == false)
	{
		//Start the frame timer
		fps.start();

		handle_input ();

		//Move the player
		update ();

		//Show the player
		draw ();

		//Cap the frame rate
		if( fps.get_ticks() < 1000 / FRAMES_PER_SECOND )
		{
			SDL_Delay( ( 1000 / FRAMES_PER_SECOND ) - fps.get_ticks() );
		}
	}

	//Clean up
	clean_up();
	return 0;
}
