#include "application.h"

int main(int argc, char** argv)
{
	if (argc < 3)
	{
		return 0;
	}

	application app{ std::string{ argv[1] }, std::string{ argv[2] } };

	if (app.Construct(160, 144, 4, 4, false, true))
	{
		app.Start();
	}

	return 0;
}