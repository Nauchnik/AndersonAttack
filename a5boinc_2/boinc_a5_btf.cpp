#ifdef _WIN32
#include "boinc_win.h"
#else
#include <cstdio>
#include <cctype>
#include <ctime>
#include <cstring>
#include <cstdlib>
#include <csignal>
#include <unistd.h>
#endif

#include <boinc_api.h>
#include <fstream>
#include <iostream>

#define INPUT_FILENAME  "in"
#define OUTPUT_FILENAME "out"

unsigned long long RunKernel(unsigned long long c_stream, unsigned long long start, unsigned long long stop, int device);
int main(int argc, char** argv)
{
	int device_arg = 0;
	if (argc == 3)
		if (std::string(argv[1])=="--device")
			device_arg = atoi(argv[2]);
	int device;
	APP_INIT_DATA aid;
	boinc_get_init_data(aid);
	if (aid.gpu_device_num >= 0) {
		   device = aid.gpu_device_num;
	} else {
		   device = device_arg;
	}

	char buf[256];
	
	int retval = boinc_init();
	if (retval)
	{
		fprintf(stderr, "%s boinc_init returned %d\n",
			boinc_msg_prefix(buf, sizeof(buf)), retval);
		exit(retval);
	}
	

	std::ofstream outfile;

	unsigned long long a, b, c;
	{
		std::string input_path;
		boinc_resolve_filename_s (INPUT_FILENAME, input_path);
		std::ifstream infile (input_path.c_str(), std::ios_base::in);

		infile >> std::hex >> a >> b >> c;
	}
	//std::cout << std::endl << " " << a << " " << b  << " " << c << std::endl;
	
	unsigned long long out = RunKernel(a, b, c, device);
	
	{
		std::string output_path;
		boinc_resolve_filename_s (OUTPUT_FILENAME, output_path);
		std::ofstream outfile (output_path.c_str(), std::ios_base::out);
		outfile << std::hex << a << " " << out ;
	}

	boinc_finish(0);
	return 0;
}
