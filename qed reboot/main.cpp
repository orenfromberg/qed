#include <GL/glew.h>
#include <GL/wglew.h>
#include <GL/gl.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "SDL.h"
#include "SDL_image.h"

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


SDL_Event event;
Timer fps; //The frame rate regulator

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

				break;
			case SDLK_DOWN: 
				break;
			case SDLK_LEFT: 
				break;
			case SDLK_RIGHT: 
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
				break;
			case SDLK_DOWN: 
				break;
			case SDLK_LEFT: 
				break;
			case SDLK_RIGHT: 
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
