#include "application.h"

int main()
{
	application app{};
	if (app.Construct(160, 144, 4, 4, false, true))
	{
		app.Start();
	}

	return 0;
}