#include <cstdlib> // system
#include <cstdio>
#include <string>

#define MAIN_ARGS
#define DEBUG 0
#define LOG(fmt)  do { if (DEBUG) printf(fmt); } while (0)
#define LOG_ARGS(fmt, ...)  do { if (DEBUG) printf(fmt, __VA_ARGS__); } while (0)

int main(int argc, char* argv[])
{
#ifdef MAIN_ARGS
	LOG_ARGS("Print all args [%d]\n", argc);
	for(int i = 0; i < argc; ++i)
	{
		LOG_ARGS("~~%s\n", argv[i]);
	}
	LOG("\n");
#endif
	LOG("START~~!!!\n");

	//system("dir");
	std::string str;
	if(argc >= 4)
	{
		//argv[1] the path to pin
		//argv[2] the path to pintool
		//argv[3] the path to app
		//argv[4..] the args in app
		str = str + argv[1] + " -t " + argv[2] + " -- " + argv[3];
		// Additional args of the called app
		if(argc >= 5)
		{
			for(int i = 4; i < argc; ++i)
			{
				str = str + " " + argv[i];
			}
		}
	}
	else
	{
		LOG("~~ \nThere are not enough arguments to run\n");
	}

	LOG_ARGS("~~ \nSummarize:\n %s \n\n", str.c_str());

	// Call the app
	system(str.c_str());

	LOG("END~~!!!\n");

	return 0;
}
