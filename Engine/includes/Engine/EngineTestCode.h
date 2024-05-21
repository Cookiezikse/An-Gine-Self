#pragma once
#ifdef ENGINECORE_EXPORTS
#define ENGINEDLL_API __declspec(dllexport)
#else
#define ENGINEDLL_API __declspec(dllimport)
#endif

//class ENGINEDLL_API EngineTestCode
class  EngineTestCode
{
	public:
		void TestCode();
};
