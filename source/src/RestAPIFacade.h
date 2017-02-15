/*
 * RestAPIFacade.h
 *
 *  Created on: Jan 12, 2017
 *      Author: developer
 */

#ifndef RESTAPIFACADE_H_
#define RESTAPIFACADE_H_

#include "CommonDefs.h"

class RestAPIFacade
{
private:
	static std::unique_ptr<RestAPIFacade> FacadeRef;

	// Private constructor for singleton design pattern
	RestAPIFacade(const std::string & url = "");

	// Forward declaration for curl Private implementation
	class CurlpImpl;

	// Unique pointer for the curl implementation
	std::unique_ptr<CurlpImpl> curlpImpl;

public:

	// Destructor
	~RestAPIFacade();

	// Create an Instance of REST API
	static RestAPIFacade * GetInstance(const std::string & url="");

	// Http POST call
	int CreateDeviceData(const std::string deviceUri, const RestApiData & data);

	// Http GET call
	int GetDeviceData(const std::string deviceUri);

	// Http PUT call
	int UpdateDeviceData(const std::string deviceUri, const RestApiData & data);

	// Http DELETE call
	int RemoveDeviceData(const std::string deviceUri);

	// Set certificate attributes
	int SetCertificateAttributes(const Certificate & cert);

	void PostPollingData(const std::string deviceUri, const std::string jsonData);
};

#endif /* RESTAPIFACADE_H_ */
