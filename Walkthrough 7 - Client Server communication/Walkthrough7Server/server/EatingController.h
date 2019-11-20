#pragma once

#include <cpprest/http_listener.h>
#include <cpprest/json.h>

#include "EatingResponse.h"
#include "EatingRequest.h"
#include "EatingModel.h"

using namespace web;
using namespace web::http;

class EatingController
{
public:
	EatingController();
	~EatingController();

	void Process(http_request request);
};

