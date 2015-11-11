#ifndef _UTIL_H_
#define _UTIL_H_

#include <list>
#include <assimp.h>

class Vertex3
{
public:
	Vertex3 ()
		: x(0.f)
		, y(0.f)
		, z(0.f)
	{}
	float x, y, z;
};

enum MatrixMode
{
	MODELVIEW,
	PROJECTION
};

enum DrawMode
{
	POINTS,
	LINES,
	TRIS,
	TRISTRIP,
	QUADS,
	QUADSTRIP
};

class Drawable
{
public:
	Drawable () {}
	void setMatrixMode(MatrixMode);
	void loadIdentity();
	void pushMatrix();
	void popMatrix();
	void vertex3(float, float, float);
	void begin(DrawMode);
	void end();
private:
	struct aiMatrix4x4 modelView;
	struct aiMatrix4x4 proj;
	std::list<Vertex3> verts;
	MatrixMode mode;
};

#endif