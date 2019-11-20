#include "EatingRequest.h"



EatingRequest::EatingRequest()
{
}


EatingRequest::~EatingRequest()
{
}

bool EatingRequest::ValidateHeaders(http_request request)
{
	bool success = true;

	if (!(request.headers().has(L"X-Eats") & request.headers().has(L"X-Food")))
	{
		success = false;
	}

	return success;
}

bool EatingRequest::ProcessRequest(http_request request, EatingModel &eatingModel)
{
	//RequestBody = json::value::object();
	bool success = true;

	// grab all the headers [UNUSED]
	http_headers requestHeaders = request.headers();

	//parse the body of the request
	json::value requestJSONData = json::value::object();
	request.extract_json().then([&requestJSONData](pplx::task<json::value> task)
	{
		requestJSONData = task.get();
	}).wait();

	if (requestJSONData.has_string_field(U("PlayerName")))
	{
		utility::string_t Name = requestJSONData.at(U("PlayerName")).as_string();
		wstring initials = Name;
		eatingModel.Name = Name;
	}
	else
	{
		success = false;
	}

	return success;
}
