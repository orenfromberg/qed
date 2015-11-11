#include "qed2.h"

#if 0
extern const struct aiScene* scene;
#endif
char * filename;

int main(int argc, char ** argv)
{
	// assimp stuff
#if 0
	if (argc != 2)
	{
		cout << "usage: " << argv[0] << " <file>" <<endl;
		return 1;
	}

	scene = aiImportFile(argv[1], aiProcessPreset_TargetRealtime_Quality);

	if (!scene)
	{
		cout << "error:  could not read file " << argv[1] << endl;
		return 1;
	}

	//traverse(scene, scene->mRootNode);
	
#endif

	filename = argv[1];
	
	if (init() == false)
	{
		clean_up();
		return 1;
	}

	run();
	
	clean_up();

	return 0;
}