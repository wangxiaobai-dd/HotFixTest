#pragma once 

#include <string>

struct FIXTABLE
{
				void* oldFunc = nullptr;
				void* newFunc = nullptr;
};

class HotFix
{
				public:
								static void doFix(const std::string& fixPatchTable, const std::string& dynLibPath);
								static void fixFunc(void* oldFunc, void* newFunc);
};
