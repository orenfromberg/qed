#include "qed2.h"
#include "animeval.h"
#include "sceneanim.h"


#define BUFFER_OFFSET(bytes) ((GLubyte*) NULL + (bytes))
#define TRAVERSE_PRINT 0

const struct aiScene* scene = NULL;
int SCREEN_WIDTH = 1024;
int SCREEN_HEIGHT = 768;
int SCREEN_BPP = 32;
int FRAMES_PER_SECOND = 60;
const int MS_PER_FRAME = 1000 / FRAMES_PER_SECOND;
bool quit = false;
Timer fps;
bool fullscreen = false;
Timer delta;
Timer timestamp;

extern char * filename;

class StupidEffect
{
public:
	StupidEffect()
		: numNodes (0)
	{
	}
	void init()
	{
		scene = aiImportFile(filename, aiProcessPreset_TargetRealtime_Quality);

		if (!scene)
		{
			cout << "error:  could not read file " << filename << endl;
			return;
		}	

		sceneanim = new SceneAnim(scene);
		//animationEvaluator.init(scene->mAnimations[0]);

		//countNumNodes (scene, scene->mRootNode);

		//traverse(scene, scene->mRootNode);
				
		numVerts = 0;
		// get num vertices in model
		for (int i = 0; i < scene->mNumMeshes; ++i)
		{
			numVerts += scene->mMeshes[i]->mNumVertices;
		}

		// allocate space for GLfloat * model_vertices
#if 1
		model_vertices = new GLfloat[numVerts*3];
		model_colors = new GLfloat[numVerts*4];
		for (int i = 0; i < numVerts*4; ++i)
			model_colors[i] = 1.0;
		// now get all verts in the model and
		// put it in GLfloat * model_vertices
		int idx = 0;
#endif

#if 0
		traverse (scene, scene->mRootNode);
#else
		for (int i = 0; i < scene->mNumMeshes; ++i)
		{
			for (int j = 0; j < scene->mMeshes[i]->mNumVertices; ++j)
			{
				model_vertices[idx++] = scene->mMeshes[i]->mVertices[j].x;
				model_vertices[idx++] = scene->mMeshes[i]->mVertices[j].y;
				model_vertices[idx++] = scene->mMeshes[i]->mVertices[j].z;
			}
		}
#endif
	}

	void printMat(struct aiMatrix4x4 m)
	{
		cout << 
			m.a1 << " " << m.a2 << " " << m.a3 << " " << m.a4 << endl <<
			m.b1 << " " << m.b2 << " " << m.b3 << " " << m.b4 << endl <<
			m.c1 << " " << m.c2 << " " << m.c3 << " " << m.c4 << endl <<
			m.d1 << " " << m.d2 << " " << m.d3 << " " << m.d4 << endl;
	}

	double ticksToSecs(int ticks)
	{
		return (double)ticks * .001;
	}

	void update ()
	{
		// function must be given time in seconds
		sceneanim->Calculate(ticksToSecs(timestamp.get_ticks()));
	}

	void draw ()
	{
		glShadeModel(GL_FLAT);
		glViewport(0, 0, (GLsizei) SCREEN_WIDTH, (GLsizei) SCREEN_HEIGHT);

		// projection matrix
		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
		//glFrustum(-1.0, 1.0, -1.0, 1.0, 1.5, 20.0);
		gluPerspective (45.0, (GLdouble)SCREEN_WIDTH/(GLdouble)SCREEN_HEIGHT, 1.5, 10000.0);
		
		// now for the modelview matrix
		glMatrixMode(GL_MODELVIEW);

		// view matrix
		glLoadIdentity(); 

		gluLookAt(0.0, 100.0, 400.0, 0.0, 100.0, 0.0, 0.0, 1.0, 0.0);
#if 0
		// model matrix
		glScalef(1.0, 1.0, 1.0); /* modeling transformation */
		// color
		glColor3f (1.0, 1.0, 1.0);
		// draw thing
		glutSolidTeapot(1.0);
#else
		glScalef (1.50, 1.50, 1.50);
		//glRotatef (270.0, 1.0, 0.0, 0.0);
		static float rot = 270.0;
		rot += 0.1;
		if (rot >= 360.0)
			rot = 0.0;
		glRotatef (rot, 0.0, 1.0, 0.0);
// model matrix
# if 0
		glBegin(GL_POINTS);
		glColor3f (1.0, 1.0, 1.0);
		for (int i = 0; i < numVerts; ++i)
		{
			glVertex3f(model_vertices[i*3], model_vertices[i*3+1], model_vertices[i*3+2]);
		}
		glEnd();
# else
#  if 0
		glVertexPointer(3, GL_FLOAT, 0, model_vertices);
		glEnableClientState(GL_VERTEX_ARRAY);
		glColorPointer (4, GL_FLOAT, 0, model_colors);
		glEnableClientState(GL_COLOR_ARRAY);
		glDrawArrays (GL_POINTS, 0, numVerts);
		glDisableClientState(GL_VERTEX_ARRAY);
		glDisableClientState(GL_COLOR_ARRAY);
#  else
		struct aiMatrix4x4 m;
		RenderSkeleton(scene->mRootNode, m, m);
#  endif
# endif
#endif

		// reset the modelview so we can draw the origin thing
		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();
		gluLookAt(0.0, 100.0, 400.0, 0.0, 100.0, 0.0, 0.0, 1.0, 0.0);

		// draw origin system
		glScalef(10.0, 10.0, 10.0);
		glBegin(GL_LINES);
			glColor3f(1.0, 0.0, 0.0);
			glVertex3f(0.0, 0.0, 0.0);
			glVertex3f(1.0, 0.0, 0.0);
			glColor3f(0.0, 1.0, 0.0);
			glVertex3f(0.0, 0.0, 0.0);
			glVertex3f(0.0, 1.0, 0.0);
			glColor3f(0.0, 0.0, 1.0);
			glVertex3f(0.0, 0.0, 0.0);
			glVertex3f(0.0, 0.0, 1.0);
		glEnd();
	}
protected:
	struct aiNode * findnode (const struct aiScene *sc, struct aiNode * nd, const struct aiString & string)
	{
		if (nd->mName == string)
			return nd;

		bool done = false;
		struct aiNode * res = NULL;
		for (int n = 0; !done && n < (int)nd->mNumChildren; ++n)
		{
			res = findnode (sc, nd->mChildren[n], string);
			if (res)
				done = true;
		}
		return res;

	}

	int RenderSkeleton (aiNode* piNode,const aiMatrix4x4& piMatrix, const aiMatrix4x4& parent)
	{
		aiMatrix4x4 me = sceneanim->GetGlobalTransform( piNode);

		me.Transpose();

		glBegin(GL_LINES);
		glColor3f(1.0, 1.0, 1.0);
		glVertex3f(parent.d1, parent.d2, parent.d3);
		glVertex3f(me.d1, me.d2, me.d3);
		//for (; it != end; ++it)
		//{
		//	glColor3f(1.0, 1.0, 1.0);
		//	glVertex3f((*it).a4, (*it).b4, (*it).c4);
		//}
		glEnd();

		// render all child nodes
		for (unsigned int i = 0; i < piNode->mNumChildren;++i)
			RenderSkeleton(piNode->mChildren[i],piMatrix, me );

		return 1;
	}

	void recursive_render (const struct aiScene *sc, const struct aiNode* nd)
	{
		int i;
		unsigned int n = 0, t;
		struct aiMatrix4x4 m = nd->mTransformation;

		// update transform
		aiTransposeMatrix4(&m);
		glPushMatrix();
		glMultMatrixf((float*)&m);

		// draw all meshes assigned to this node
		for (; n < nd->mNumMeshes; ++n) {
			const struct aiMesh* mesh = scene->mMeshes[nd->mMeshes[n]];

			if(mesh->mNormals == NULL) {
				glDisable(GL_LIGHTING);
			} else {
				glEnable(GL_LIGHTING);
			}

			for (t = 0; t < mesh->mNumFaces; ++t) {
				const struct aiFace* face = &mesh->mFaces[t];
				GLenum face_mode;

				switch(face->mNumIndices) {
				case 1: face_mode = GL_POINTS; break;
				case 2: face_mode = GL_LINES; break;
				case 3: face_mode = GL_TRIANGLES; break;
				default: face_mode = GL_POLYGON; break;
				}

				glBegin(face_mode);

				for(i = 0; i < face->mNumIndices; i++) {
					int index = face->mIndices[i];
					if(mesh->mColors[0] != NULL)
						glColor4fv((GLfloat*)&mesh->mColors[0][index]);
					if(mesh->mNormals != NULL) 
						glNormal3fv(&mesh->mNormals[index].x);
					glVertex3fv(&mesh->mVertices[index].x);
				}

				glEnd();
			}

		}

		// draw all children
		for (n = 0; n < nd->mNumChildren; ++n) {
			recursive_render(sc, nd->mChildren[n]);
		}

		glPopMatrix();
	}

	void countNumNodes (const struct aiScene * sc, const struct aiNode * nd)
	{
		numNodes++;

		// traverse children
		for (int n = 0; n < (int)nd->mNumChildren; ++n) 
		{
			countNumNodes(sc, nd->mChildren[n]);
		}
	}

	void traverse (const struct aiScene *sc, const struct aiNode* nd)
	{
#if TRAVERSE_PRINT
		numNodes++;
		static int depth = 0;
		depth++;
		cout << numNodes << " ";
		for (int i = 0; i < depth; ++i)
			cout << " ";
		cout << (char *)nd->mName.data << endl;
# if 0
		cout << "current modelViewMatrix: " << endl;
		printMat(modelViewMatrix);
		cout << "current modelViewStack: " << endl;
		std::list <struct aiMatrix4x4>::iterator it = modelViewStack.begin();
		std::list <struct aiMatrix4x4>::iterator end = modelViewStack.end();
		for (; it != end; ++it)
		{
			printMat(*it);
			cout << endl;
		}
# endif
#endif
		//// 1. transpose the node transform ??
		//struct aiMatrix4x4 * m = new struct aiMatrix4x4(nd->mTransformation);
		//aiTransposeMatrix4(m);
		struct aiMatrix4x4 m(nd->mTransformation);
		//aiTransposeMatrix4(&m);
		//// 2. push current MV onto stack
		////glPushMatrix();
		//modelViewStack.push_back(&ModelViewMatrix);
		modelViewStack.push_back(modelViewMatrix);
		//// 3. multiply current MV by transposed transform
		////glMultMatrixf((float*)&m);
		//ModelViewMatrix *= *m;
		struct aiMatrix4x4 prevMVM(modelViewMatrix);
		aiTransposeMatrix4(&prevMVM);
		modelViewMatrix = modelViewMatrix * m;
		//modelViewMatrix = m * modelViewMatrix;
		struct aiMatrix4x4 MVM(modelViewMatrix);
	
		// take previous MVM and draw origin to current MVM*origin
		aiTransposeMatrix4(&MVM);
		glLoadIdentity();
		//glMultMatrixf((float*)&m);
		//glPointSize(3.0);
		glBegin(GL_LINES);
			glColor4f(1.0, 1.0, 1.0, 1.0);	
			//glVertex4f(0.0, 0.0, 0.0, 1.0);
			glVertex3f(prevMVM.d1, prevMVM.d2, prevMVM.d2);
			glVertex3f(MVM.d1, MVM.d2, MVM.d3);
		glEnd();

		//// mesh stuff
		//{
		//	for (int n = 0; n <nd->mNumMeshes; ++n)	// for each mesh
		//	{
		//		const struct aiMesh * mesh = scene->mMeshes[nd->mMeshes[n]];
		//		for (int t = 0; t < mesh->mNumFaces; ++t) // for each face
		//		{
		//			const struct aiFace * face = &mesh->mFaces[t];
		//			for (int i = 0; i < face->mNumIndices; ++i) // for each index
		//			{
		//				int index = face->mIndices[i];
		//			}
		//		}
		//	}
		//}


		// traverse children
		for (int n = 0; n < (int)nd->mNumChildren; ++n) 
		{
			traverse(sc, nd->mChildren[n]);
		}

		//// 4. pop matrix
		//ModelViewMatrix = *(modelViewStack.back());
		//modelViewStack.pop_back();
		modelViewMatrix = modelViewStack.back();
		modelViewStack.pop_back();
		
#if TRAVERSE_PRINT
		depth--;
#endif
	}

	SceneAnim * sceneanim;
	//AnimEval animationEvaluator;

	std::list<GLfloat> verts;
	int numVerts;
	int numNodes;
	struct aiMatrix4x4 modelViewMatrix;
	std::list <struct aiMatrix4x4> modelViewStack;
	std::list <struct aiMatrix4x4>::iterator it;
	GLuint vertexBuffer;
	GLuint colorBuffer;
	GLuint uvBuffer;
	GLuint indexBuffer;
	
	GLfloat * model_vertices;
	GLfloat * model_colors;
	GLfloat * model_uvs;
	GLuint * model_indices;
	
	GLuint fragShader;
	GLuint vertShader;
	GLuint program;
};

StupidEffect effect;



bool init()
{
	if( SDL_Init( SDL_INIT_EVERYTHING ) < 0 )
	{
		cout << "error: SDL_Init() failed" << endl;
		return false;
	}

	if (fullscreen)
	{
		if( SDL_SetVideoMode( SCREEN_WIDTH, SCREEN_HEIGHT, SCREEN_BPP, SDL_FULLSCREEN | SDL_OPENGL | SDL_HWSURFACE) == NULL )
		{
			cout << "error: SDL_SetVideoMode() fullscreen failed" << endl;
			return false;
		}
	}
	else
	{
		if( SDL_SetVideoMode( SCREEN_WIDTH, SCREEN_HEIGHT, SCREEN_BPP, SDL_OPENGL ) == NULL )
		{
			cout << "error: SDL_SetVideoMode() failed" << endl;
			return false;
		}
	}

	//Initialize OpenGL
	if( init_GL() == false )
	{
		return false;
	}

	//Set caption
	ostringstream oss;
	oss << "QED2 using OpenGL version " << glGetString(GL_VERSION) << endl;
	SDL_WM_SetCaption(oss.str().c_str(), NULL );

#if 0
	glGetIntegerv(GL_MAX_ELEMENTS_VERTICES, &maxVerts);
	glGetIntegerv(GL_MAX_ELEMENTS_INDICES, &maxIndxs);
	model_vertices = new GLfloat[3*maxVerts];
	model_indices = new GLuint[maxIndxs];

	// get mesh and draw to screen
	if (scene->HasMeshes())
	{
		// just get first mesh
		aiMesh * mesh = scene->mMeshes[0];
		
		// first make vertex buffer
		model_vertices = new GLfloat[3*mesh->mNumVertices];
		model_colors = new GLfloat[4*mesh->mNumVertices];
		{
			for (int i = 0, j = 0, k = 0; i < mesh->mNumVertices; ++i)
			{
				model_vertices[j++] = mesh->mVertices[i].x;
				model_vertices[j++] = mesh->mVertices[i].y;
				model_vertices[j++] = mesh->mVertices[i].z;
				model_colors[k++] = 1.0;
				model_colors[k++] = 1.0;
				model_colors[k++] = 1.0;
				model_colors[k++] = 1.0;
			}
		}

		// gl stuff
		glGenBuffers (1, &vertexBuffer);
		glBindBuffer (GL_ARRAY_BUFFER, vertexBuffer);
		glBufferData (GL_ARRAY_BUFFER, mesh->mNumVertices * 3, model_vertices, GL_STATIC_DRAW);
		
		glGenBuffers (1, &colorBuffer);
		glBindBuffer (GL_ARRAY_BUFFER, colorBuffer);
		glBufferData (GL_ARRAY_BUFFER, mesh->mNumVertices * 4, model_vertices, GL_STATIC_DRAW);
				
		// next make index buffer
		numIndices = 0;
		model_indices = new GLuint[3*mesh->mNumFaces];
		{
			// for each face
			for (int i = 0; i < mesh->mNumFaces; ++i)
			{
				for (int j = 0; j < mesh->mFaces[i].mNumIndices; ++j)
				{
					model_indices[numIndices++] = mesh->mFaces[i].mIndices[j];
				}
			}
		}

		// gl stuff
		glGenBuffers (1, &indexBuffer);
		glBindBuffer (GL_ELEMENT_ARRAY_BUFFER, indexBuffer);
		glBufferData (GL_ELEMENT_ARRAY_BUFFER, numIndices, model_indices, GL_STATIC_DRAW);

		// done
		//delete model_vertices;
		//delete model_indices;
	}
#endif

	effect.init ();

	return true;
}

bool init_GL ()
{
	// Initialize OpenGL entry points.
	if (glewInit()!=GLEW_OK || !GLEW_VERSION_1_1) 
	{
		fprintf(stderr, "failed to initialize GLEW, OpenGL 1.1 required.\n");
		return false;
	}

	string renderer((const char *)glGetString (GL_RENDERER));

	//If there was any errors
	if(glGetError() != GL_NO_ERROR)
	{
		return false;
	}

	//If everything initialized
	return true;
}

void handle_input ()
{
	SDL_Event event;

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

void run()
{
	int ticks = 0;
	delta.start();
	timestamp.start();

	while (quit == false)
	{
		fps.start();

		handle_input ();

#if 0
		ticks = delta.get_ticks();
		update (ticks);
		delta.start();
#else
		update ();
#endif


		draw ();

		ticks = fps.get_ticks();
		if(ticks < MS_PER_FRAME)
		{
			SDL_Delay(MS_PER_FRAME - ticks);
		}
	}
}

// the output of the update function should be a queue of draw 
// commands that will be executed in the draw function.
void update (int dt)
{
	if (dt == 0)
		return;
}

void update ()
{
	effect.update();
}

// the draw function should just iterate over the queue of draw
// commands and then swap buffers.
void draw ()
{
	glClearColor(0.42f, 0.42f, 0.42f, 1.0f);
	glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

#if 0
	///////////////////////////////////
	glEnable(GL_DEPTH_TEST);
	glDisable(GL_LIGHTING);

	// projection
	glm::mat4 proj =  glm::perspective<float>(45.0, (float)SCREEN_WIDTH/(float)SCREEN_HEIGHT, 0.1, 1000.0);

	// view matrix
	glm::vec3 eye = glm::vec3(0, 75, 250);
	glm::vec3 center = glm::vec3(0, 75, 0);
	glm::vec3 up = glm::vec3(0,1,0);
	glm::mat4 view = glm::lookAt<float>(eye, center, up);

	float xrot = 270.0f;
	float yrot = 0.0f;
	// model matrix
	//glm::vec3 translation = glm::vec3(0.0, -75.0, -10.0);
	glm::vec3 translation = glm::vec3(0.0, 0.0, 0.0);
	glm::mat4 model = glm::mat4 (1.0);
	model = glm::rotate<float>(glm::mat4(1.0f), yrot, glm::vec3(0, 1, 0));
	model = glm::rotate<float>(model, xrot, glm::vec3(1, 0, 0));
	model = glm::rotate<float>(model, 270, glm::vec3(1, 0, 0));
	model = glm::translate<float>(model, translation);

	glm::mat4 mv = view * model;

	glMatrixMode (GL_PROJECTION);
	glLoadIdentity();
	glLoadMatrixf (glm::value_ptr(proj));
	glMatrixMode (GL_MODELVIEW);
	glLoadIdentity();
	glLoadMatrixf(glm::value_ptr(mv));

	glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
	glEnableClientState(GL_VERTEX_ARRAY);
	glVertexPointer (3, GL_FLOAT, 0, BUFFER_OFFSET(0));
	
	glBindBuffer (GL_ARRAY_BUFFER, colorBuffer);
	glEnableClientState(GL_COLOR_ARRAY);
	glColorPointer (4, GL_FLOAT, 0, BUFFER_OFFSET(0));
	
	glBindBuffer (GL_ELEMENT_ARRAY_BUFFER, indexBuffer);
	glDrawElements (GL_TRIANGLES, numIndices, GL_UNSIGNED_INT, BUFFER_OFFSET(0));
	glDisableClientState (GL_VERTEX_ARRAY);
	glDisableClientState (GL_COLOR_ARRAY);
	glBindBuffer (GL_ARRAY_BUFFER, 0);
	glBindBuffer (GL_ELEMENT_ARRAY_BUFFER, 0);
	////////////////////////////////
#endif

	effect.draw ();
	
	SDL_GL_SwapBuffers();
}

void clean_up()
{
	aiReleaseImport(scene);
	SDL_Quit();
}