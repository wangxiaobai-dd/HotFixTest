#include "HotFix.h"
#include <stdio.h>
#include <dlfcn.h> 
#include <sys/mman.h>
#include <iostream>
#include <unistd.h>
#include <cstring>

void HotFix::doFix(const std::string& fixTableSymbol, const std::string& dynLibPath)
{
				void* lib = dlopen(dynLibPath.c_str(), RTLD_NOW);
				if(lib == nullptr)
				{
								std::cerr << "dlopen failed, fixTableSymbol:" << fixTableSymbol << ", error:" << dlerror() << std::endl;
								return;
				}
				FIXTABLE* table = (FIXTABLE*)dlsym(lib, fixTableSymbol.c_str());
				if(table == nullptr)
				{
								std::cerr << "dlsym failed " << ", error " << dlerror() << std::endl;
								dlclose(lib);
								return;
				}
				for( ; table->oldFunc && table->newFunc; ++table)
				{
								printf("for fixFunc, oldFunc:%p->newFunc:%p\n", table->oldFunc, table->newFunc);
								HotFix::fixFunc(table->oldFunc, table->newFunc);
				}
				std::cout << "doFix,symbol:" << fixTableSymbol << " dynlib:" << dynLibPath << std::endl;
}

void HotFix::fixFunc(void* oldFunc, void* newFunc)
{
				// 跳转指令
				char prefix[] = {'\x48', '\xb8'};   //MOV newFunc %rax
				char postfix[] = {'\xff', '\xe0'};  //JMP %rax

				size_t page_size= getpagesize();
				const int inst_len = sizeof(prefix) + sizeof(void *) + sizeof(postfix);
				char *align_point = (char *)oldFunc - ((uint64_t)(char *)oldFunc % page_size);
				if (0 != mprotect(align_point, (char *)oldFunc - align_point + inst_len, PROT_READ | PROT_WRITE | PROT_EXEC)) 
				{
								std::cerr << "mprotect failed begin" << std::endl;
								printf("begin errno: %d (%s)\n", errno, strerror(errno));
								return;
				}

				// 将跳转指令写入原函数开头
				memcpy(oldFunc, prefix, sizeof(prefix));
				memcpy((char *)oldFunc + sizeof(prefix), &newFunc, sizeof(void *));
				memcpy((char *)oldFunc + sizeof(prefix) + sizeof(void *), postfix, sizeof(postfix));

				// 关闭代码可写权限
				if(0 != mprotect(align_point, (char *)oldFunc - align_point + inst_len, PROT_READ | PROT_EXEC)) 
				{
								std::cerr << "mprotect failed end" << std::endl;
								printf("end errno: %d (%s)\n", errno, strerror(errno));
								return;
				}
				printf("fixFunc, oldFunc:%p->newFunc:%p\n", oldFunc, newFunc);
}
