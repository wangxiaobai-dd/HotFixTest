
#include "SceneUser.h"
#include <stdio.h>

int SceneUser::d = 100;

void SceneUser::test(int p1, int p2)
{
					printf("test,p1:%d,p2:%d,a:%d,b:%d,c:%d\n", p1, p2, this->a, this->b, this->c);
}
