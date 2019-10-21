#pragma once
#include "stdafx.h"

#include "INPLRuntime.h"
#include "INPLRuntimeState.h"
#include "IParaEngineCore.h"
#include "IParaEngineApp.h"

using namespace ParaEngine;

class ExeLoader {
public:
	ExeLoader();
	~ExeLoader();

		/**
		* run a external application.
		* the app plays a role of filter which read data from stdin and output the filtered result in stdout and exit.
		* this function run the app, pass in the data and return the filtered result
		* @return
		* if lpExeName executed and exit, return { "exit_code": exit_code, "output": app_stdout_content }
		* else something wrong happen in ExecuteFilter, return nil
		*/
    const std::string ExecuteFilter(const std::string& lpExeName, const std::string& data, int * runtime_error, int * exit_code);

private:
};