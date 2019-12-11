#include "ExeLoader.h"


#define BUFSIZE 1024


ExeLoader::ExeLoader()
{

}

ExeLoader::~ExeLoader()
{

}

const std::string ExeLoader::Execute(const std::string& exe_path, const std::string& input, int* runtime_error, int* exit_code)
{
	std::string nil = "";
	*runtime_error = 0;
	*exit_code = 0;

	HANDLE g_hChildStd_IN_Rd = NULL;
	HANDLE g_hChildStd_IN_Wr = NULL;
	HANDLE g_hChildStd_OUT_Rd = NULL;
	HANDLE g_hChildStd_OUT_Wr = NULL;

	SECURITY_ATTRIBUTES saAttr;

	// Set the bInheritHandle flag so pipe handles are inherited.
	saAttr.nLength = sizeof(SECURITY_ATTRIBUTES);
	saAttr.bInheritHandle = TRUE;
	saAttr.lpSecurityDescriptor = NULL;

	// Create a pipe for the child process's STDOUT.
	if (!CreatePipe(&g_hChildStd_OUT_Rd, &g_hChildStd_OUT_Wr, &saAttr, 0)) {
		OUTPUT_LOG("error: create pipe stdout rd failed\n");
		*runtime_error = 1;
		return nil;
	}

	// Ensure the read handle to the pipe for STDOUT is not inherited.
	if (!SetHandleInformation(g_hChildStd_OUT_Rd, HANDLE_FLAG_INHERIT, 0)) {
		OUTPUT_LOG("error: set handle information stdout rd failed\n");
		*runtime_error = 1;
		return nil;
	}

	// Create a pipe for the child process's STDIN.
	if (!CreatePipe(&g_hChildStd_IN_Rd, &g_hChildStd_IN_Wr, &saAttr, 0)) {
		OUTPUT_LOG("error: create pipe stdin rd failed\n");
		*runtime_error = 1;
		return nil;
	}

	// Ensure the write handle to the pipe for STDIN is not inherited.
	if (!SetHandleInformation(g_hChildStd_IN_Wr, HANDLE_FLAG_INHERIT, 0)) {
		OUTPUT_LOG("error: set handle information stdin rd failed\n");
		*runtime_error = 1;
		return nil;
	}

	/* Create the child process */
	STARTUPINFO siStartInfo;
	PROCESS_INFORMATION piProcInfo;

	BOOL bSuccess = FALSE;
	DWORD dwErrorCode;

	// Set up members of the PROCESS_INFORMATION structure.
	ZeroMemory(&piProcInfo, sizeof(PROCESS_INFORMATION));

	// This structure specifies the STDIN and STDOUT handles for redirection.
	ZeroMemory(&siStartInfo, sizeof(STARTUPINFO));
	siStartInfo.cb = sizeof(STARTUPINFO);
	siStartInfo.hStdError = g_hChildStd_OUT_Wr;
	siStartInfo.hStdOutput = g_hChildStd_OUT_Wr;
	siStartInfo.hStdInput = g_hChildStd_IN_Rd;
	siStartInfo.dwFlags |= STARTF_USESTDHANDLES;

	// Create the child process.
	bSuccess = ::CreateProcess(NULL,
		(LPSTR)exe_path.c_str(),    // command line
		NULL,         // process security attributes
		NULL,         // primary thread security attributes
		TRUE,         // handles are inherited
//		0,            // creation flags
		CREATE_NO_WINDOW, // no window flag
		NULL,         // use parent's environment
		NULL,         // use parent's current directory
		&siStartInfo, // STARTUPINFO pointer
		&piProcInfo); // receives PROCESS_INFORMATION

	// If an error occurs, exit the application.
	if (!bSuccess) {
		dwErrorCode = GetLastError();
		OUTPUT_LOG("error: failed running cmdline: %s, error code(%d).\n", exe_path.c_str(), dwErrorCode);
		*runtime_error = 1;
		return nil;
	}

	/* Write data to child process */
	DWORD dwRead = input.length();
	DWORD dwWritten;

	bSuccess = WriteFile(g_hChildStd_IN_Wr, input.c_str(), dwRead, &dwWritten, NULL);
	if (!bSuccess) {
		OUTPUT_LOG("error: write to child process stdin failed\n");
		*runtime_error = 1;
		return nil;
	}
		
	// Close the pipe handle so the child process stops reading.
	if (!CloseHandle(g_hChildStd_IN_Wr)) {
		OUTPUT_LOG("error: close handle stdin wr failed\n");
		*runtime_error = 1;
		return nil;
	}


	/* Read result from child process pipe */
	DWORD dwExitCode;
	WaitForSingleObject(piProcInfo.hProcess, INFINITE);
	GetExitCodeProcess(piProcInfo.hProcess, &dwExitCode);
	CloseHandle(g_hChildStd_OUT_Wr);

	char chBuf[BUFSIZE];
	std::string output;

	for (;;)
	{
		bSuccess = ReadFile(g_hChildStd_OUT_Rd, chBuf, BUFSIZE, &dwRead, NULL);
		if (!bSuccess) {
			dwErrorCode = GetLastError();

			// child process has terminated the pipe
			if (dwErrorCode == ERROR_BROKEN_PIPE)
				break;

			OUTPUT_LOG("error: read output from child process stdout failed, error code %d\n", dwErrorCode);
			*runtime_error = 1;
			return nil;
		}

		if (dwRead == 0)
			break;

		output.append(chBuf, dwRead);
	}

	CloseHandle(piProcInfo.hProcess);
	CloseHandle(piProcInfo.hThread);

	*exit_code = (int)dwExitCode;
	return output;
}

const std::string ExeLoader::ExecuteNotWin(const std::string& exe_path, const std::string& input, int* runtime_error, int* exit_code)
{
	namespace bp = boost::process;

	bp::ipstream out;
	bp::opstream in;

	bp::child c(exe_path.c_str(), bp::std_in < in, bp::std_out > out);
	in.write(input.c_str(), input.length());
	in.flush();
	in.pipe().close();

	std::string line;
	std::string output;
	while (std::getline(out, line))
		output = output + line;

	c.wait();

	*runtime_error = 0;
	*exit_code = c.exit_code();

	return output;
}
