#ifndef _QED2_H_
#define _QED2_H_

#include <iostream>
#include <sstream>
#include <list>
#include <vector>
#include <SDL.h>
#include <SDL_image.h>
#include <GL/glew.h>
#include <assimp.h>
#include <aiPostProcess.h>
#include <aiScene.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "Timer.h"
#include "Renderer.h"
#include "util.h"

using namespace std;

void traverse (const struct aiScene *sc, const struct aiNode* nd);
bool init();
bool init_GL ();
void handle_input ();
void run();
void update (int);
void update (void);
void draw ();
void clean_up();

#endif