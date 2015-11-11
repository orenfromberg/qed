#ifndef _ANIMEVAL_H_
#define _ANIMEVAL_H_

#include <vector>
#include <aiAnim.h>

class Triple
{
public:
	Triple()
	{
		for (int i = 0; i < 3; ++i)
			data[i] = 0;
	}
	unsigned int data[3];
};

class AnimEval
{
public:
	AnimEval ()
		: mLastTime(0.0)
		, mAnim (NULL)
	{
	}
	void init (const aiAnimation* pAnim);
	void Evaluate( double pTime);
	const std::vector<aiMatrix4x4>& GetTransformations() const { return mTransforms; }
private:
	const aiAnimation* mAnim;
	double mLastTime;
	std::vector<Triple> mLastPositions;	
	std::vector<aiMatrix4x4> mTransforms;
};


#endif //_ANIMEVAL_H_