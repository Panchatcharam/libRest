/*
 * RestAPIFacade.cpp
 *
 *  Created on: Jan 12, 2017
 *      Author: developer
 */
#include "RestAPIFacade.h"
#include "CurlHttpWrapper.h"

// Initialize facade reference
std::unique_ptr<RestAPIFacade> RestAPIFacade::FacadeRef = nullptr;

// PIMPL for Curl Http wrapper
class RestAPIFacade::CurlpImpl
{
private:
	std::unique_ptr<CurlHttpWrapper> curl;

public:
	CurlpImpl(const std::string & url = "");
	~CurlpImpl();
	std::unique_ptr<CurlHttpWrapper> & Get();
};

RestAPIFacade::CurlpImpl::CurlpImpl(const std::string & url) try : curl(new CurlHttpWrapper(url))
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

RestAPIFacade::RestAPIFacade(const std::string & url) try : curlpImpl(new CurlpImpl(url))
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
	FacadeRef.release();
}

// Create an Instance of REST API
RestAPIFacade * RestAPIFacade::GetInstance(const std::string & url)
{
	if (FacadeRef == nullptr)
	{
		if (!url.empty())
		{
			FacadeRef = std::unique_ptr<RestAPIFacade>(new RestAPIFacade(url));
		}
		else
		{
			throw std::invalid_argument("Failed creating REST API object, Pass valid URL");
		}
	}

	return FacadeRef.get();
}

// Http POST call
int RestAPIFacade::CreateDeviceData(const std::string deviceUri, const rest::RestData & data)
{
	int status = rest::SUCCESS;
	curlpImpl->Get()->CreateDeviceData(deviceUri, data);
	return status;
}

// Http GET call
int RestAPIFacade::GetDeviceData(const std::string deviceUri)
{
	int status = rest::SUCCESS;
	curlpImpl->Get()->GetDeviceData(deviceUri);
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
