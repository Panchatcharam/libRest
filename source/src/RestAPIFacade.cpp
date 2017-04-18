/*
 * RestAPIFacade.cpp
 *
 *  Created on: Jan 12, 2017
 *      Author: Panchatcharam
 */
#include "RestAPIFacade.h"
#include "CurlHttpWrapper.h"

using namespace rest;

// PIMPL for Curl Http wrapper
class RestAPIFacade::CurlpImpl
{
private:
	std::unique_ptr<CurlHttpWrapper> curl;

public:
	CurlpImpl();
	~CurlpImpl();
	std::unique_ptr<CurlHttpWrapper> & Get();
};

RestAPIFacade::CurlpImpl::CurlpImpl() try : curl(new CurlHttpWrapper())
{

}

catch(std::exception & e)
{
	//#### TODO: Log the curl http wrapper creation error
	throw e;
}

RestAPIFacade::CurlpImpl::~CurlpImpl()
{
	curl.reset();
}

std::unique_ptr<CurlHttpWrapper> & RestAPIFacade::CurlpImpl::Get()
{
	return curl;
}

RestAPIFacade::RestAPIFacade() try : curlpImpl(new CurlpImpl())
{
	curlpImpl->Get()->Start();
}

catch(std::exception & e)
{
	//#### TODO: Log the error
	throw e;
}

// Destructor
RestAPIFacade::~RestAPIFacade()
{
	curlpImpl->Get()->Shutdown();
}

// Create an Instance of REST API
std::shared_ptr<RestAPIFacade> RestAPIFacade::GetInstance()
{
	// Initialization of function-local statics is guaranteed to occur only
	// once even when called from multiple threads, and may be more efficient
	// than the equivalent code using std::call_once.
	static std::shared_ptr<RestAPIFacade> rest(std::shared_ptr<RestAPIFacade>(new RestAPIFacade()));

	return rest;
}

// Http POST call
int RestAPIFacade::CreateDeviceData(const std::string deviceUri, const rest::RestData & data)
{
	int status = rest::SUCCESS;
	curlpImpl->Get()->CreateDeviceData(deviceUri, data);
	return status;
}

// Http GET call
int RestAPIFacade::GetDeviceData(const std::string deviceUri, std::string & data)
{
	int status = rest::SUCCESS;
	curlpImpl->Get()->GetDeviceData(deviceUri, data);
	return status;
}

// Http PUT call
int RestAPIFacade::UpdateDeviceData(const std::string deviceUri, const rest::RestData & data)
{
	int status = rest::SUCCESS;
	return status;
}

// Http DELETE call
int RestAPIFacade::RemoveDeviceData(const std::string deviceUri)
{
	int status = rest::SUCCESS;
	return status;
}

int RestAPIFacade::SetCertificateAttributes(const rest::Certificate & cert)
{
	return (curlpImpl->Get()->SetCertificateAttributes(cert));
}

void RestAPIFacade::PostPollingData(const std::string deviceUri, const rest::RestData data)
{
	curlpImpl->Get()->PostPollingData(deviceUri, data);
}
