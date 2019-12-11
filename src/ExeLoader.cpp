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
	namespace bp = boost::process;

	bp::ipstream out;
	bp::opstream in;

	bp::environment env = boost::this_process::environment();

	bp::child c(exe_path.c_str(), bp::std_in < in, bp::std_out > out, env, bp::windows::hide);

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
