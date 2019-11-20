#include "stdafx.h"

#include "INPLRuntime.h"
#include "INPLRuntimeState.h"
#include "IParaEngineCore.h"
#include "IParaEngineApp.h"

#include "ExeLoader.h"

using namespace ParaEngine;

#pragma region PE_DLL 

#ifdef WIN32
#define CORE_EXPORT_DECL    __declspec(dllexport)
#else
#define CORE_EXPORT_DECL
#endif

// forware declare of exported functions. 
#ifdef __cplusplus
extern "C" {
#endif
	CORE_EXPORT_DECL const char* LibDescription();
	CORE_EXPORT_DECL int LibNumberClasses();
	CORE_EXPORT_DECL unsigned long LibVersion();
	CORE_EXPORT_DECL ParaEngine::ClassDescriptor* LibClassDesc(int i);
	CORE_EXPORT_DECL void LibInit();
	CORE_EXPORT_DECL void LibActivate(int nType, void* pVoid);
	CORE_EXPORT_DECL void LibInitParaEngine(ParaEngine::IParaEngineCore* pCoreInterface);
#ifdef __cplusplus
}   /* extern "C" */
#endif

HINSTANCE Instance = NULL;



ClassDescriptor* ExeLoader_GetClassDesc();
typedef ClassDescriptor* (*GetClassDescMethod)();

GetClassDescMethod Plugins[] =
{
	ExeLoader_GetClassDesc,
};

#define ExeLoader_CLASS_ID Class_ID(0x3b305bd1, 0x47a7a9ce)

class ExeLoaderDesc : public ClassDescriptor
{
public:

	void* Create(bool loading)
	{
		return NULL;
	}
	const char* ClassName()
	{
		return "IExeLoader";
	}

	SClass_ID SuperClassID()
	{
		return OBJECT_MODIFIER_CLASS_ID;
	}

	Class_ID ClassID()
	{
		return ExeLoader_CLASS_ID;
	}

	const char* Category()
	{
		return "ExeLoader Category";
	}

	const char* InternalName()
	{
		return "ExeLoader InternalName";
	}

	HINSTANCE HInstance()
	{
		extern HINSTANCE Instance;
		return Instance;
	}
};

ClassDescriptor* ExeLoader_GetClassDesc()
{
	static ExeLoaderDesc s_desc;
	return &s_desc;
}

CORE_EXPORT_DECL const char* LibDescription()
{
	return "ParaEngine ExeLoader Ver 1.0.0";
}

CORE_EXPORT_DECL unsigned long LibVersion()
{
	return 1;
}

CORE_EXPORT_DECL int LibNumberClasses()
{
	return sizeof(Plugins) / sizeof(Plugins[0]);
}

CORE_EXPORT_DECL ClassDescriptor* LibClassDesc(int i)
{
	if (i < LibNumberClasses() && Plugins[i])
	{
		return Plugins[i]();
	}
	else
	{
		return NULL;
	}
}
ParaEngine::IParaEngineCore* g_pCoreInterface = NULL;
ParaEngine::IParaEngineCore* GetCoreInterface()
{
	return g_pCoreInterface;
}

CORE_EXPORT_DECL void LibInitParaEngine(IParaEngineCore* pCoreInterface)
{
	g_pCoreInterface = pCoreInterface;
}
CORE_EXPORT_DECL void LibInit()
{
}

#ifdef WIN32
BOOL WINAPI DllMain(HINSTANCE hinstDLL, ULONG fdwReason, LPVOID lpvReserved)
#else
void __attribute__((constructor)) DllMain()
#endif
{
	// TODO: dll start up code here
#ifdef WIN32
	Instance = hinstDLL;				// Hang on to this DLL's instance handle.
	return (TRUE);
#endif
}
extern "C" {
	/** this is an example of c function calling NPL core interface */
	void WriteLog(const char* str) {
		if (GetCoreInterface())
			GetCoreInterface()->GetAppInterface()->WriteToLog(str);
	}
}
#pragma endregion PE_DLL 
CORE_EXPORT_DECL void LibActivate(int nType, void* pVoid)
{
	if (nType == ParaEngine::PluginActType_STATE)
	{
		NPL::INPLRuntimeState* pState = (NPL::INPLRuntimeState*)pVoid;
		const char* sMsg = pState->GetCurrentMsg();

		NPLInterface::NPLObjectProxy tabMsg = NPLInterface::NPLHelper::MsgStringToNPLTable(sMsg);
		std::string exe_path = tabMsg["exe_path"];
		std::string input = tabMsg["input"];
		std::string callback = tabMsg["callback"];

		ExeLoader loader;

		int runtime_error;
		int exit_code;
		std::string output = loader.Execute(exe_path, input, &runtime_error, &exit_code);

		NPLInterface::NPLObjectProxy resMsg;
		resMsg["runtime_error"] = false;
		resMsg["exit_code"] = (double)0;
		resMsg["output"] = "";
		
		if (runtime_error != 0) {
			resMsg["runtime_error"] = true;
		}
		else {
			resMsg["exit_code"] = (double)exit_code;
			resMsg["output"] = output.c_str();
		}

		std::string resMsgStr;
		NPLInterface::NPLHelper::NPLTableToString(NULL, resMsg, resMsgStr);

		resMsgStr = std::string("msg = ") + resMsgStr;

		if (!callback.empty()) {
			// sync method `call` instead of async `activate`
			pState->call(callback.c_str(), resMsgStr.c_str(), resMsgStr.length());

		}
	}
}



