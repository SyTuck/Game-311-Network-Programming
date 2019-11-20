#include "EatingController.h"



EatingController::EatingController()
{
}


EatingController::~EatingController()
{
}

void EatingController::Process(http_request request)
{
	EatingRequest eatingReq;
	EatingModel eatingModel;

	// validate headers
	bool success = eatingReq.ValidateHeaders(request);
	if (!success)
	{
		request.reply(status_codes::FailedDependency, "Error, Missing or Incorrect Header Information");
	}

	// parse request to populate Model Data
	success = eatingReq.ProcessRequest(request, eatingModel);
	if (!success)
	{
		request.reply(status_codes::BadRequest, "Error, Unable to Process the Request");
	}

	// Complete Any Logic Here
//	bool newUser = !SessionManager::GetInstance()->SessionExists(loginModel.Name);

//	loginModel.ResponseStr = loginModel.Name;
//	loginModel.SessionToken = -1;

//	if (newUser == false)
//	{
//		loginModel.ResponseStr.append(L" Welcome back");
//		loginModel.SessionToken = SessionManager::GetInstance()->GetSession(loginModel.Name);
//	}
//	else
//	{
//		loginModel.SessionToken = SessionManager::GetInstance()->CreateSession(loginModel.Name);
//		loginModel.ResponseStr.append(L" Welcome to the server. Since this is your first time, please see your authentication token attached.");
//	}

	// Process and send the response
	EatingResponse Response;
	success = Response.ProcessResponse(eatingModel);
	if (!success)
	{
		request.reply(status_codes::BadRequest, "Error, Unable to Process Response Data");
	}

	Response.SendResponse(request);
}
