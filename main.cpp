
#include "SceneUser.h"
#include "HotFix.h"
#include <signal.h>
#include <unistd.h>
#include <iostream>

static void sigDoFix(int signum)
{
				HotFix::doFix("patchFixTable", "hotfix.so");
				std::cout << "signal do fix" << std::endl;
}

int initHotFixSignal()
{
				if(signal(SIGUSR1, sigDoFix) == SIG_ERR)
				{
								return -1;
				}
				return 0;
}

int main()
{
				initHotFixSignal();
				SceneUser user;
				while(1)
				{
								sleep(2);
								user.test(1, 2);
				}
				return 0;
}
