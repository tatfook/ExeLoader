#pragma once
#include "stdafx.h"

#include "INPLRuntime.h"
#include "INPLRuntimeState.h"
#include "IParaEngineCore.h"
#include "IParaEngineApp.h"

#include <boost/process.hpp>
#include <boost/process/windows.hpp>

using namespace ParaEngine;

class ExeLoader {
public:
	ExeLoader();
	~ExeLoader();

	/**
	* run a external exe and return stdout result (and runtime_error flag and exit_code)
	* @param
	* - exe_path: executable file path
	* - input: stdin input for exe file
	* - runtime_error: a flag indicate if system call error happened
	* - exit_code: exe file exit code
	# @return
	* - exe file stdout content
	*/
	const std::string Execute(const std::string& exe_path, const std::string& input, int* runtime_error, int* exit_code);

private:
};