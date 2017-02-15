/*
 * CurlWrapper.h
 *
 *  Created on: Jan 13, 2017
 *      Author: developer
 */

#ifndef CURLHTTPWRAPPER_H_
#define CURLHTTPWRAPPER_H_

#include <memory>
#include <curl/curl.h>
#include <thread>
#include <queue>
#include <mutex>
#include <atomic>
#include <string>
#include "CommonDefs.h"
#include "JsonParser.h"

class CurlHttpWrapper
{
private:
	// Post data up to server (create)
	int PostData(const std::string deviceUri, const RestApiData & data);

	int Post(const std::pair<std::string, std::unique_ptr<RestApiData>> & data);

	//Get Data from server
	int GetData(const std::string deviceUri);

	// Put data up to server (update/replace)
	int PutData(const std::string deviceUri, const RestApiData & data);

	//Delete data from server
	int DeleteData(const std::string deviceUri);

	//Initialize the CURL
	CURL * InitCurl();

	// Thread method to handle post data request queue
	void HandlePostData();

	// Function to handle write callback
	static size_t WriteCallback(void *contents, size_t size, size_t nmemb, void *userp);

	struct curl_slist* addSecurityHeaders(int data_type);

	// To hold an instance to the CURL
	std::unique_ptr<CURL,decltype(&curl_easy_cleanup)> mCurl;

	// Thread to handle the requests
	std::thread mPostThread;

	// POST Queue
	std::queue<std::pair<std::string, std::unique_ptr<RestApiData>>> mPostQueue;

	// Mutex to protect post queue
	std::mutex mPostQueueMutex;

	// To track whether the REST API is active
	std::atomic<bool> mActive;

	// Contains server url
	const std::string serverUrl;

	// Certificate attributes
	Certificate cert;

	// To hold an instance to json wrapper
	std::unique_ptr<JsonWrapper> mJson;

	// To hold the return status
	std::string strPostStatus;

	// Mutex to protect polling data method
	std::mutex mPollingMutex;

public:

	enum DATA_TYPE
	{
		TYPE_JSON = 0,
		TYPE_WWW_FORM
	};

	// Constructor
	CurlHttpWrapper(const std::string & url);

	// Destructor
	~CurlHttpWrapper();

	CurlHttpWrapper(const CurlHttpWrapper & rhs) = delete;

	CurlHttpWrapper & operator=(const CurlHttpWrapper & rhs) = delete;

	CurlHttpWrapper(CurlHttpWrapper && rhs) = delete;
	CurlHttpWrapper & operator=(CurlHttpWrapper && rhs) = delete;

	// Http POST call
	int CreateDeviceData(const std::string deviceUri, const RestApiData & data);

	// Http GET call
	int GetDeviceData(const std::string deviceUri);

	// Http PUT call
	int UpdateDeviceData(const std::string deviceUri, const RestApiData & data);

	// Http DELETE call
	int RemoveDeviceData(const std::string deviceUri);

	// Called to initialize the curlhttpwrapper
	void Start();

	// Called to un initialize the curlhttpwrapper
	void Shutdown();

	// Update curl about the TLS credentials
	int UpdateTLSInfo();

	// Update certificate attributes
	int SetCertificateAttributes(const Certificate & cert);

	int PostPollingData(const std::string deviceUri, const std::string jsonData);
};



#endif /* CURLHTTPWRAPPER_H_ */
