#include "ScenePatch.h"
#include "HotFix.h"
#include "SceneUser.h"
#include <iostream>

void fixSceneUserTest(SceneUser* user)
{
				if(!user)
								return;
				user->a = 5;
				std::cout << "test3:" << user->a << std::endl;
}

FIXTABLE patchFixTable[] = 
{
				{(void*)(0x0000000000401882), (void*)fixSceneUserTest},
				{nullptr, nullptr}
};
