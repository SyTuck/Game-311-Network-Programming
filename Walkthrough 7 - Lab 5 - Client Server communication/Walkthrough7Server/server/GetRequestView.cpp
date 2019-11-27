#include "GetRequestView.h"
#include "PlayerDiedController.h"

void handle_get(http_request request)
{
	wstring APIPath = request.absolute_uri().to_string();

	wcout << "\nAPI PATH:" << APIPath;
	cout << "\nhandle GET\n";

	if (wcscmp(APIPath.c_str(), L"/SLCGame311/PlayerDied") == 0)
	{
		PlayerDiedController Controller;
		Controller.Process(request);
	}

	request.reply(status_codes::BadRequest, "Requested API not found.");
}