/*
 * CurlWrapper.h
 *
 *  Created on: Jan 13, 2017
 *      Author: Panchatcharam
 */

#ifndef CURLHTTPWRAPPER_H_
#define CURLHTTPWRAPPER_H_

#include <memory>
#include <curl/curl.h>
#include <thread>
#include <queue>
#include <atomic>
#include <string>
#include <exception>
#include <condition_variable>
#include "CommonDefs.h"
//#include "JsonParser.h"
#include <log4cplus/logger.h>
#include <log4cplus/loggingmacros.h>
#include <log4cplus/configurator.h>
#include <log4cplus/fileappender.h>

class CurlHttpWrapper
{
private:
	enum class HTTP_METHOD
	{
        GET = 0,
		POST,
		PUT,
		DELETE
	};

	struct payload
	{
		HTTP_METHOD method;
		std::string url;
		std::unique_ptr<rest::RestData> restData;
	};

	// Post data up to server (create)
	int PostData(const std::string deviceUri, const rest::RestData & data);

	// Handle post request
	int Post(const payload & data);

	// Handle Put request
	int Put(const payload & data);

	//Get Data from server
	int GetData(const std::string deviceUri, std::string & data);

	// Put data up to server (update/replace)
	int PutData(const std::string deviceUri, const rest::RestData & data);

	//Delete data from server
	int DeleteData(const std::string deviceUri);

	//Initialize the CURL
	CURL * InitCurl();

	// Thread method to handle post data request queue
	void HandleHttpRequest();

	// Function to handle write callback
	static size_t WriteCallback(void *contents, size_t size, size_t nmemb, void *userp);

	// Function to handle GET write callback
	static size_t GetWriteCallback(void *contents, size_t size, size_t nmemb, void *userp);

	// Method to add security headers
	struct curl_slist* addSecurityHeaders(int data_type);

	// Object to hold logger objects.
	log4cplus::Logger mLogger;

	// To hold an instance to the CURL
	std::unique_ptr<CURL,decltype(&curl_easy_cleanup)> mCurl;

	// Function declaration for lambda
	std::function<void(CURLcode, std::string)> LogError;

	// Function to free headers
	std::function<void(struct curl_slist *)> FreeHeader;

	// Thread to handle the requests
	std::thread mHttpThread;

	// Mutex to protect post queue
	std::mutex mHttpQueueMutex;

	// To track whether the REST API is active
	std::atomic<bool> mActive;

	// Contains server url
	//const std::string serverUrl;

	// Certificate attributes
	rest::Certificate cert;

	// To hold an instance to json wrapper
    //std::unique_ptr<JsonWrapper> mJson;

	// To hold the return status
	std::string strHttpStatus;

	// Mutex to protect polling data method
	std::mutex mPollingMutex;

	// Condition variable for queue data
	std::condition_variable cvHttpDataQueue;

	// POST/PUT/DELTE Queue
	std::queue<payload> mHttpDataQueue;

	// To hold GET data
	std::string strHttpGetData;

public:

	// Constructor
	CurlHttpWrapper();

	// Destructor
	~CurlHttpWrapper();

	CurlHttpWrapper(const CurlHttpWrapper & rhs) = delete;

	CurlHttpWrapper & operator=(const CurlHttpWrapper & rhs) = delete;

	CurlHttpWrapper(CurlHttpWrapper && rhs) = delete;
	CurlHttpWrapper & operator=(CurlHttpWrapper && rhs) = delete;

	// Http POST call
	int CreateDeviceData(const std::string deviceUri, const rest::RestData & data);

	// Http GET call
	int GetDeviceData(const std::string deviceUri, std::string & data);

	// Http PUT call
	int UpdateDeviceData(const std::string deviceUri, const rest::RestData & data);

	// Http DELETE call
	int RemoveDeviceData(const std::string deviceUri);

	// Called to initialize the curlhttpwrapper
	void Start();

	// Called to un initialize the curlhttpwrapper
	void Shutdown();

	// Update curl about the TLS credentials
	int UpdateTLSInfo();

	// Update certificate attributes
	int SetCertificateAttributes(const rest::Certificate & cert);

	int PostPollingData(const std::string deviceUri, const rest::RestData & data);
};

class MyException : public std::exception
{
public:
	MyException(std::string str="CurlHttpWrapper Exception") : reason(str) {}
	~MyException() {}

	const char* what() const throw()
	{
		return reason.c_str();
	}

private:
	std::string reason;
};

#endif /* CURLHTTPWRAPPER_H_ */
