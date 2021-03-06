/*
 * RestAPIFacade.h
 *
 *  Created on: Jan 12, 2017
 *      Author: Panchatcharam
 */

#ifndef RESTAPIFACADE_H_
#define RESTAPIFACADE_H_

#include "CommonDefs.h"

namespace rest
{
	class RestAPIFacade
	{
	private:

		// Private constructor for singleton design pattern
		RestAPIFacade();

		// Forward declaration for curl Private implementation
		class CurlpImpl;

		// Unique pointer for the curl implementation
		std::unique_ptr<CurlpImpl> curlpImpl;

	public:

		// Destructor
		~RestAPIFacade();

		// Create an Instance of REST API
		static std::shared_ptr<RestAPIFacade> GetInstance();

		// Http POST call
		int CreateDeviceData(const std::string deviceUri, const rest::RestData & data);

		// Http GET call
		int GetDeviceData(const std::string deviceUri, std::string & data);

		// Http PUT call
		int UpdateDeviceData(const std::string deviceUri, const rest::RestData & data);

		// Http DELETE call
		int RemoveDeviceData(const std::string deviceUri);

		// Set certificate attributes
		int SetCertificateAttributes(const rest::Certificate & cert);

		void PostPollingData(const std::string deviceUri, const rest::RestData data);
	};
}

#endif /* RESTAPIFACADE_H_ */
