#ifndef _RENDERER_H_
#define _RENDERER_H_

class DrawCommand
{
public:
	DrawCommand()
	{
	}
	virtual void execute() = 0;
protected:
};

class DrawElements: public DrawCommand
{
	DrawElements()
	{
	}
	virtual void execute ()
	{

	}
};

#endif