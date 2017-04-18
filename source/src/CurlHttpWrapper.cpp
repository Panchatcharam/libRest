/*
 * CurlHttpWrapper.cpp
 *
 *  Created on: Jan 13, 2017
 *      Author: Panchatcharam
 */

#include <ctime>
#include "CurlHttpWrapper.h"
#include "Base64.h"
#include "sha256.h"

std::atomic<long> Req_Sent(0);
std::atomic<long> Req_Recvd(0);

// Constructor, Initialize the curl
CurlHttpWrapper::CurlHttpWrapper() try : mCurl(InitCurl(), curl_easy_cleanup)
                                                         , mPostThread(std::thread(&CurlHttpWrapper::HandlePostData, this))
                                                         , mPostQueue()
														 , mPostQueueMutex()
														 , mActive{false}
//                                   	   	   	   	   	 , serverUrl("")
														 , cert()
//														 , mJson(new JsonWrapper)
														 , strPostStatus("")
														 , mPollingMutex()
														 , cvPostQueue()
{
	LogError = [this](CURLcode errCode, std::string errStr)
	{
		if(errStr.empty())
		{
			errStr = std::string(curl_easy_strerror(errCode));
		}

		LOG4CPLUS_ERROR(mLogger, LOG4CPLUS_TEXT("Error: ") << errStr);
	};

	// Lambda to free headers.
	FreeHeader = [this](struct curl_slist * headers)
	{
		// free headers
		if (headers != nullptr)
		{
			curl_slist_free_all(headers);
			headers = nullptr;
		}
	};

//	mPostThread = std::thread(&CurlHttpWrapper::HandlePostData, this);

	LOG4CPLUS_DEBUG (mLogger, "Created CurlHttpWrapper");
}

// Catch the construction error
catch(...)
{
	LOG4CPLUS_FATAL (mLogger, "Creating CurlHttpWrapper failed");
}

// Destructor
CurlHttpWrapper::~CurlHttpWrapper()
{
	LOG4CPLUS_DEBUG (mLogger, "Destructed CurlHttpWrapper");

	if ( mPostThread.joinable() )
	{
		mPostThread.join();
	}

	curl_global_cleanup();
	log4cplus::Logger::shutdown();
}

CURL * CurlHttpWrapper::InitCurl()
{
	CURL * ptr = nullptr;

	try
	{
		// Initialize log4cplus
		log4cplus::initialize();
		auto logger = log4cplus::Logger::getInstance(LOG4CPLUS_TEXT("RestAPI"));
		mLogger = logger;
		log4cplus::PropertyConfigurator::doConfigure(LOG4CPLUS_TEXT("RestAPILog.properties"));
	}

	catch(...)
	{
		std::cout << "Failed creating logger in CurlHttpWrapper" << std::endl;
	}

	try
	{
		curl_global_init(CURL_GLOBAL_DEFAULT);
		ptr = curl_easy_init();

		LOG4CPLUS_DEBUG (mLogger, "Initialized libcurl");
	}

	catch(...)
	{
		LOG4CPLUS_FATAL (mLogger, "Unable to connect to curl library");
		throw std::runtime_error("Fatal Error: unable to connect to curl");
	}
	return ptr;
}

void CurlHttpWrapper::Start()
{
	if (mActive.load(std::memory_order_relaxed) == false)
	{
		std::lock_guard<std::mutex> lock(mPostQueueMutex);
		mActive.store(true,std::memory_order_relaxed);
		curl_easy_setopt(mCurl.get(), CURLOPT_WRITEFUNCTION, CurlHttpWrapper::WriteCallback);
		curl_easy_setopt(mCurl.get(), CURLOPT_WRITEDATA, &strPostStatus);
		LOG4CPLUS_DEBUG (mLogger, "Activated the  CurlHttpWrapper");
		cvPostQueue.notify_one();
	}
}

void CurlHttpWrapper::Shutdown()
{
	if (mActive.load(std::memory_order_relaxed))
	{
		std::lock_guard<std::mutex> lock(mPostQueueMutex);
		LOG4CPLUS_DEBUG (mLogger, "Deactivated the  CurlHttpWrapper");
		mActive.store(false,std::memory_order_relaxed);
		cvPostQueue.notify_one();
	}
}

// Http POST call
int CurlHttpWrapper::CreateDeviceData(const std::string deviceUri, const rest::RestData & data)
{
//	LOG4CPLUS_DEBUG (mLogger, "Request to create device data");
	return (PostData(deviceUri,data));
}

// Http GET call
int CurlHttpWrapper::GetDeviceData(const std::string deviceUri, std::string & data)
{
	int status = rest::SUCCESS;
	status = GetData(deviceUri, data);
	LOG4CPLUS_DEBUG (mLogger, "Request to get device data");
	return status;
}

// Http PUT call
int CurlHttpWrapper::UpdateDeviceData(const std::string deviceUri, const rest::RestData & data)
{
	int status = rest::SUCCESS;

	LOG4CPLUS_DEBUG (mLogger, "Update device data");
	return status;
}

// Http DELETE call
int CurlHttpWrapper::RemoveDeviceData(const std::string deviceUri)
{
	int status = rest::SUCCESS;

	LOG4CPLUS_DEBUG (mLogger, "Update device data");
	return status;
}

int CurlHttpWrapper::PostData(const std::string deviceUri, const rest::RestData & data)
{
	int status = rest::SUCCESS;

	try
	{
		std::lock_guard<std::mutex> queuelock(mPostQueueMutex);
		LOG4CPLUS_ERROR (mLogger, "Try Locking Mutex @ CurlHttpWrapper::PostData");
		mPostQueue.push(std::make_pair(deviceUri, std::unique_ptr<rest::RestData>(new rest::RestData(data))));
		cvPostQueue.notify_one();
		LOG4CPLUS_ERROR (mLogger, "Notified @ CurlHttpWrapper::PostData");
		++Req_Recvd;
	}

	catch(...)
	{
		LOG4CPLUS_ERROR (mLogger, "Failed Pushing data in to queue");
	}

	return status;
}

int CurlHttpWrapper::Post(const std::pair<std::string, std::unique_ptr<rest::RestData>> & data)
{
	LOG4CPLUS_TRACE (mLogger, "Processing Post request");

	CURLcode res={};
	char errbuf[CURL_ERROR_SIZE]={0};
	int status = rest::FAILURE;
	struct curl_slist *headers = nullptr;

	std::string url(data.first);

	// Clear the POST status info
	strPostStatus.clear();

	try
	{
		// Provide a buffer to store errors in
		curl_easy_setopt(mCurl.get(), CURLOPT_ERRORBUFFER, errbuf);

		// Format json data to std string
		std::string payLoad(data.second->GetContent());

		// First set the URL that is about to receive our POST.
		res = curl_easy_setopt(mCurl.get(), CURLOPT_URL, url.c_str());
		if ( res != CURLE_OK )
		{
			throw MyException();
		}

		// Format Headers
		headers = addSecurityHeaders(data.second->GetType());

		// Pass custom made headers
		res = curl_easy_setopt(mCurl.get(), CURLOPT_HTTPHEADER, headers);
		if ( res != CURLE_OK )
		{
			throw MyException();
		}

		res = curl_easy_setopt(mCurl.get(), CURLOPT_CUSTOMREQUEST, "POST");
		if ( res != CURLE_OK )
		{
			throw MyException();
		}

		// Set the size of the postfields data
		res = curl_easy_setopt(mCurl.get(), CURLOPT_POSTFIELDSIZE, payLoad.length());
		if ( res != CURLE_OK )
		{
			throw MyException();
		}

		// Now specify the POST data
		res = curl_easy_setopt(mCurl.get(), CURLOPT_POSTFIELDS, payLoad.c_str());
		if (  res != CURLE_OK )
		{
			throw MyException();
		}

		res = curl_easy_perform(mCurl.get());

		if (res != CURLE_OK)
		{
			throw MyException();
		}
		else
		{
			status = rest::SUCCESS;
		}

		// free headers
		FreeHeader(headers);
	}

	catch(MyException & e)
	{
		FreeHeader(headers);
		LogError(res, std::string(errbuf));
	}

	catch(...)
	{
		FreeHeader(headers);
		LOG4CPLUS_DEBUG (mLogger, LOG4CPLUS_TEXT("Exception occurred"));
	}

    LOG4CPLUS_DEBUG (mLogger, LOG4CPLUS_TEXT("POST Status : ") << strPostStatus);

	return status;
}

int CurlHttpWrapper::GetData(const std::string deviceUri, std::string & data)
{
	CURLcode res{};
	int status = rest::FAILURE;
	char errbuf[CURL_ERROR_SIZE]={0};
	std::string url(deviceUri);

	// Clear the POST status info
	strPostStatus.clear();

	try
	{
		// Provide a buffer to store errors in
		curl_easy_setopt(mCurl.get(), CURLOPT_ERRORBUFFER, errbuf);

		// Set SSL verifier
		res = curl_easy_setopt(mCurl.get(), CURLOPT_SSL_VERIFYPEER, 1L);

		if ( res != CURLE_OK)
		{
			throw MyException();
		}

		res = curl_easy_setopt(mCurl.get(), CURLOPT_URL, url.c_str());
		if ( res != CURLE_OK)
		{
			throw MyException();
		}

		// Require use of SSL for this, or fail
		res = curl_easy_setopt(mCurl.get(), CURLOPT_USE_SSL, CURLUSESSL_ALL);
		if ( res != CURLE_OK )
		{
			throw MyException();
		}

		// use a GET to fetch this
		res = curl_easy_setopt(mCurl.get(), CURLOPT_HTTPGET, 1L);
		if ( res != CURLE_OK)
		{
			throw MyException();
		}

		res = curl_easy_perform(mCurl.get());

		if ( res != CURLE_OK)
		{
			throw MyException();
		}
		else
		{
			status = rest::SUCCESS;
		}
	}

	catch(MyException & e)
	{
		LogError(res, std::string(errbuf));
	}

	catch(...)
	{
		LOG4CPLUS_DEBUG (mLogger, LOG4CPLUS_TEXT("Exception occurred"));
	}

	data = strPostStatus;

	return status;
}

int CurlHttpWrapper::PutData(const std::string deviceUri, const rest::RestData & data)
{
	return rest::SUCCESS;
}

int CurlHttpWrapper::DeleteData(const std::string deviceUri)
{
	return rest::SUCCESS;
}

void CurlHttpWrapper::HandlePostData()
{
	bool queueEmpty(false);

	{
		LOG4CPLUS_DEBUG(mLogger, "Entering Wait untill the ready signal is received.");
		// Wait untill the ready signal is received.
		std::unique_lock<std::mutex> lock(mPostQueueMutex);
		cvPostQueue.wait(lock, [this]{return mActive.load(std::memory_order_relaxed);});
		LOG4CPLUS_DEBUG(mLogger, "Ready signal is received.");
	}

	while(mActive.load(std::memory_order_relaxed))
	{
		{
			std::pair<std::string, std::unique_ptr<rest::RestData>> payLoad{};
			{
				std::lock_guard<std::mutex> queuelock(mPostQueueMutex);
				queueEmpty = mPostQueue.empty();

				if (!queueEmpty)
				{
					payLoad = std::move(mPostQueue.front());
					mPostQueue.pop();
					LOG4CPLUS_DEBUG(mLogger, LOG4CPLUS_TEXT("processing uri: ") << payLoad.first);
				}
			}

			if (!queueEmpty)
			{
				auto status = Post(payLoad);

				// Log the error if the post was not successful.
				if (status != rest::SUCCESS)
				{
					LOG4CPLUS_ERROR(mLogger, LOG4CPLUS_TEXT("POST Failed for ") << payLoad.first);
				}

				std::lock_guard<std::mutex> queuelock(mPostQueueMutex);
				queueEmpty = mPostQueue.empty();
				std::cout << "\rRequest Sent Count : " << ++Req_Sent << " Recvd Count : " << Req_Recvd;
			}
		}

		if (queueEmpty)
		{
            std::unique_lock<std::mutex> lock(mPostQueueMutex);
            cvPostQueue.wait(lock, [this]{return (!mPostQueue.empty() || !mActive.load(std::memory_order_relaxed));});
		}
	}
}

int CurlHttpWrapper::SetCertificateAttributes(const rest::Certificate & cert)
{
	this->cert = cert;

	auto status = UpdateTLSInfo();

	if (status != rest::SUCCESS)
	{
		LOG4CPLUS_ERROR(mLogger, LOG4CPLUS_TEXT("Setting Certificate attribute failed "));
	}

	return status;
}

// Update curl about the TLS credentials
int CurlHttpWrapper::UpdateTLSInfo()
{
	int status = rest::FAILURE;
	CURLcode res{};
	char errbuf[CURL_ERROR_SIZE]={0};

	LOG4CPLUS_DEBUG (mLogger, "Setting certificate attributes");

	try
	{
		// Provide a buffer to store errors in
		curl_easy_setopt(mCurl.get(), CURLOPT_ERRORBUFFER, errbuf);

		// Set key
		res = curl_easy_setopt(mCurl.get(), CURLOPT_SSLCERTTYPE, cert.strKeyType.c_str());
		if ( res != CURLE_OK)
		{
			throw MyException();
		}

		// Set the cert for client authentication
		res = curl_easy_setopt(mCurl.get(), CURLOPT_SSLCERT, cert.strCertificate.c_str());
		if ( res != CURLE_OK)
		{
			throw MyException();
		}

		// Set passphrase
		res = curl_easy_setopt(mCurl.get(), CURLOPT_KEYPASSWD, cert.strPassword.c_str());
		if ( res != CURLE_OK)
		{
			throw MyException();
		}

		// If we use a key stored in a crypto engine, we must set the key type to "ENG"
		res = curl_easy_setopt(mCurl.get(), CURLOPT_SSLKEYTYPE, cert.strKeyType.c_str());
		if ( res!=  CURLE_OK)
		{
			throw MyException();
		}

		// Set the private key (file or ID in engine)
		res = curl_easy_setopt(mCurl.get(), CURLOPT_SSLKEY, cert.strKeyName.c_str());
		if ( res !=  CURLE_OK)
		{
			throw MyException();
		}

		// Set the file with the certs vaildating the server
		res = curl_easy_setopt(mCurl.get(), CURLOPT_CAINFO, cert.strCAcertificate.c_str());
		if ( res !=  CURLE_OK)
		{
			throw MyException();
		}

		// Disconnect if we can't validate server's cert
		res = curl_easy_setopt(mCurl.get(), CURLOPT_SSL_VERIFYPEER, 1L);
		if ( res !=  CURLE_OK)
		{
			throw MyException();
		}
		else
		{
			return rest::SUCCESS;
		}
	}

	catch(MyException & e)
	{
		LogError(res, std::string(errbuf));
	}

	catch(...)
	{
		LOG4CPLUS_DEBUG (mLogger, LOG4CPLUS_TEXT("Exception occurred"));
	}

	return status;
}

int CurlHttpWrapper::PostPollingData(const std::string deviceUri, const rest::RestData & data)
{
	std::lock_guard<std::mutex> guard(mPollingMutex);

	CURLcode res{};
	char errbuf[CURL_ERROR_SIZE]={0};
	int status = rest::FAILURE;
	std::string url(deviceUri);
	std::string payLoad(data.GetContent());
	struct curl_slist *headers = nullptr;

	// Clear the POST status info
	strPostStatus.clear();

	try
	{
		// Provide a buffer to store errors in
		curl_easy_setopt(mCurl.get(), CURLOPT_ERRORBUFFER, errbuf);

	    // First set the URL that is about to receive our POST.
		res = curl_easy_setopt(mCurl.get(), CURLOPT_URL, url.c_str());
	    if ( res != CURLE_OK )
	    {
	    	throw MyException();
	    }

	    // Format Headers
	    headers = addSecurityHeaders(data.GetType());

	    // Pass custom made headers
	    res = curl_easy_setopt(mCurl.get(), CURLOPT_HTTPHEADER, headers);
	    if ( res != CURLE_OK )
	    {
	    	throw MyException();
	    }

	    res = curl_easy_setopt(mCurl.get(), CURLOPT_CUSTOMREQUEST, "POST");
	    if ( res != CURLE_OK )
	    {
	    	throw MyException();
	    }

		// Set the size of the postfields data
	    res = curl_easy_setopt(mCurl.get(), CURLOPT_POSTFIELDSIZE, payLoad.length());
		if ( res != CURLE_OK )
		{
			throw MyException();
		}

		// Now specify the POST data
		res = curl_easy_setopt(mCurl.get(), CURLOPT_POSTFIELDS, payLoad.c_str());
		if ( res != CURLE_OK )
		{
			throw MyException();
		}

		res = curl_easy_perform(mCurl.get());
		if ( res != CURLE_OK)
		{
			throw MyException();
		}
		else
		{
			status = rest::SUCCESS;
		}

	    // Free headers
		FreeHeader(headers);
	}

	catch(MyException & e)
	{
		FreeHeader(headers);
		LogError(res, std::string(errbuf));
	}

	catch(...)
	{
		FreeHeader(headers);
		LOG4CPLUS_DEBUG (mLogger, LOG4CPLUS_TEXT("Exception occurred"));
	}

    LOG4CPLUS_DEBUG (mLogger, LOG4CPLUS_TEXT("POST Status : ") << strPostStatus);

	return status;
}

size_t CurlHttpWrapper::WriteCallback(void *contents, size_t size, size_t nmemb, void *userp)
{
	size_t totalSize(size * nmemb);
    ((std::string*)userp)->append((char*)contents, totalSize);
    return totalSize;
}

struct curl_slist* CurlHttpWrapper::addSecurityHeaders(int data_type)
{
	const std::string sstm("");
	const std::string key("");
	const std::string token("");
	std::string Data1("");
	std::string Data2("");
	std::string Data3("");

	try
	{
		Data1 = "sstm: " + base64_encode(reinterpret_cast<const unsigned char*>(sstm.c_str()),sstm.length());;

		Data2 = "public_key: " + base64_encode(reinterpret_cast<const unsigned char*>(key.c_str()), key.length());
	}

	catch(std::exception & e)
	{
		LOG4CPLUS_ERROR(mLogger, LOG4CPLUS_TEXT("Unable to generate base64 code, error: ") << e.what());
	}

	try
	{
		Data3 = "token: " + sha256(token);
	}

	catch(std::exception & e)
	{
		LOG4CPLUS_ERROR(mLogger, LOG4CPLUS_TEXT("Unable to generate sha256, error: ") << e.what());
	}

	struct curl_slist *headers = NULL;

	try
	{
		headers = curl_slist_append(headers, "cache-control: no-cache");
		headers = curl_slist_append(headers, Data2.c_str());
		headers = curl_slist_append(headers, Data1.c_str());
		headers = curl_slist_append(headers, Data3.c_str());

		switch(data_type)
		{
		case rest::TYPE_WWW_FORM:
			headers = curl_slist_append(headers, "content-type: application/x-www-form-urlencoded");
			break;
		case rest::TYPE_JSON:
		default:
			headers = curl_slist_append(headers, "content-type: application/json");
			break;
		}
	}

	catch(std::exception & e)
	{
		FreeHeader(headers);
		LOG4CPLUS_ERROR(mLogger, LOG4CPLUS_TEXT("Failed to append headers, error: ") << e.what());
	}

	return headers;
}
