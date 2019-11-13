// the following code is an adaptation of the code presented on: 
// https://mariusbancila.ro/blog/2017/11/19/revisited-full-fledged-client-server-example-with-c-rest-sdk-2-10/

#include <cpprest/http_listener.h>
#include <cpprest/json.h>
#include <map>

#include <iostream>
#include <string>
#include <time.h>
#include <stdlib.h>

#pragma comment(lib, "cpprest_2_10")

using namespace web;
using namespace web::http;
using namespace web::http::experimental::listener;
using namespace std;

map<wstring, int> Sessions;
int g_SessionID = 0;

int currentServerRandSeed = -1;															//copy of the seed and last generated number 
int latestServerRand = -1;																//in case it needs to be distributed

void handle_get(http_request request)
{
	wstring APIPath = request.absolute_uri().to_string();
	http_headers requestHeaders = request.headers();

	cout << "\nhandle GET";
	wcout << " API PATH " << APIPath;

	if (wcscmp(APIPath.c_str(), L"/SLCGame311/playerDied") == 0)
	{
		if (request.headers().has(L"UserID"))
		{
			json::value JSONObj = json::value::object();
			JSONObj[L"UserID"] = json::value::string(requestHeaders[L"UserID"]);
			request.reply(status_codes::OK, JSONObj);
			cout << " OK \n";
		}
		else
		{
			request.reply(status_codes::FailedDependency, L"Error, Who Died? (UserID required)");
			cout << " no user ID\n";
		}
	}
	else if (wcscmp(APIPath.c_str(), L"/SLCGame311/gameOver") == 0)
	{
		json::value JSONObj = json::value::object();
		JSONObj[L"GameOver"] = json::value::boolean(true);
		request.reply(status_codes::OK, JSONObj);
		cout << " Part 1a) OK\n";
	}
	else if (wcscmp(APIPath.c_str(), L"/SLCGame311/getServerRand") == 0)
	{
		json::value JSONObj = json::value::object();

//		JSONObj[L"RandSeed"] = json::value::number(currentServerRandSeed);
		JSONObj[L"RandVal"] = json::value::number(latestServerRand);
		request.reply(status_codes::OK, JSONObj);
		cout << " Part 1b) OK\n";
	}
	else
	{
		request.reply(status_codes::NotImplemented, "Unrecognized Get endpoint");
		cout << " Unrecognized endpoint\n";
	}	
}

void handle_post(http_request request)
{
	wstring APIPath = request.absolute_uri().to_string();
	http_headers requestHeaders = request.headers();

	cout << "\nhandle POST";
	wcout << " API PATH " << APIPath;

	//json::value::string(requestHeaders[L"Content-type"]);
	wcout << " " << requestHeaders.content_type();

//	if (requestHeaders[L"Content-type"] == L"application/json")
	if (requestHeaders.content_type() == L"application/json")
	{
		if (wcscmp(APIPath.c_str(), L"/SLCGame311/Login") == 0)
		{
			json::value requestJSONData = json::value::object();

			request.extract_json()
				.then([&requestJSONData](pplx::task<json::value> task)
			{
				requestJSONData = task.get();
			})
				.wait();

			if (requestJSONData.has_string_field(U("Name")));
			{
				utility::string_t Name = requestJSONData.at(U("Name")).as_string();
				wstring initials = Name;

				if (Sessions[initials] != NULL)
				{
					Name.append(L" welcome back.");
				}
				else
				{
					g_SessionID++;
					Sessions[initials] = g_SessionID;
					Name.append(L" Welcome to the server. Since this is your first time, please see your authentication token attached.");
				}

				json::value JSONObj = json::value::object();
				JSONObj[L"ResponseStr"] = json::value::string(Name);
				JSONObj[L"SessionToken"] = json::value::number(g_SessionID);
				request.reply(status_codes::OK, JSONObj);
			}
			request.reply(status_codes::OK, "Post Success");
			cout << "\n    Logging in...";
		}
		else
		{
			request.reply(status_codes::NotAcceptable, "unsupported Post endpoint");
			cout << " unsupported endpoint\n";
		}
	}
	else
	{
		request.reply(status_codes::NotAcceptable, "unsupported Post content type");
		cout << " unsupported content type\n";
	}

}

int main()
{

	currentServerRandSeed = time(nullptr)%60;
	srand(currentServerRandSeed);
	latestServerRand = rand();

	http_listener listener(L"http://localhost:8777/SLCGame311");

	listener.support(methods::GET, handle_get);
	listener.support(methods::POST, handle_post);
	
	try
	{
		listener.open().then([&listener]() 
		{
			wcout << ("\nlistening::\n") << listener.uri().to_string().c_str(); 
		})
		.wait();
	}
	catch (exception const & e)
	{
		wcout << e.what() << endl;
	}

	// infinite while loop to ensure our application continues to run and doesn't reach the end

	int lastTime = time(nullptr) % 60;
	while (true)
	{
//		if (lastTime != time(nullptr) % 60)
//		{
//			lastTime = time(nullptr) % 60;
//			latestServerRand = rand();
//		}
	}
	

	return 0;
}
