/*
 * CurlHttpWrapper.cpp
 *
 *  Created on: Jan 13, 2017
 *      Author: developer
 */

#include <ctime>
#include "CurlHttpWrapper.h"
#include "Base64.h"
#include "sha256.h"

// Constructor, Initialize the curl
CurlHttpWrapper::CurlHttpWrapper(const std::string & url) try : mCurl(InitCurl(), curl_easy_cleanup)
                                                          , mPostThread()
                                                          , mPostQueue()
														  , mPostQueueMutex()
														  , mActive{false}
                                   	   	   	   	   	   	  , serverUrl(url)
														  , cert()
														  , mJson(new JsonWrapper)
														  , strPostStatus("")
														  , mPollingMutex()
{

}

// Catch the construction error
catch(...)
{

}

// Destructor
CurlHttpWrapper::~CurlHttpWrapper()
{
	if ( mPostThread.joinable() )
	{
		mPostThread.join();
	}

	mCurl.release();
	curl_global_cleanup();
}

CURL * CurlHttpWrapper::InitCurl()
{
	CURL * ptr = nullptr;

	try
	{
		curl_global_init(CURL_GLOBAL_DEFAULT);
		ptr = curl_easy_init();
	}

	catch(...)
	{
		throw std::runtime_error("Fatal Error: unable to connect to curl");
	}
	return ptr;
}

void CurlHttpWrapper::Start()
{
	mActive.store(true,std::memory_order_relaxed);
	curl_easy_setopt(mCurl.get(), CURLOPT_WRITEFUNCTION, CurlHttpWrapper::WriteCallback);
	curl_easy_setopt(mCurl.get(), CURLOPT_WRITEDATA, &strPostStatus);
	mPostThread = std::thread(&CurlHttpWrapper::HandlePostData, this);
}

void CurlHttpWrapper::Shutdown()
{
	mActive.store(false,std::memory_order_relaxed);
}

// Http POST call
int CurlHttpWrapper::CreateDeviceData(const std::string deviceUri, const RestApiData & data)
{
	return (PostData(deviceUri,data));
}

// Http GET call
int CurlHttpWrapper::GetDeviceData(const std::string deviceUri)
{
	int status = SUCCESS;
	status = GetData(deviceUri);
	return status;
}

// Http PUT call
int CurlHttpWrapper::UpdateDeviceData(const std::string deviceUri, const RestApiData & data)
{
	int status = SUCCESS;
	return status;
}

// Http DELETE call
int CurlHttpWrapper::RemoveDeviceData(const std::string deviceUri)
{
	int status = SUCCESS;
	return status;
}

int CurlHttpWrapper::PostData(const std::string deviceUri, const RestApiData & data)
{
	int status = SUCCESS;
	{
		std::lock_guard<std::mutex> queuelock(mPostQueueMutex);
		mPostQueue.push(std::make_pair(deviceUri,std::unique_ptr<RestApiData>(new RestApiData(data))));
	}
	return status;
}

int CurlHttpWrapper::Post(const std::pair<std::string, std::unique_ptr<RestApiData>> & data)
{
	int status = FAILURE;
	std::string url = serverUrl + data.first;

	Json::Value postData;

	// Clear the POST status info
	strPostStatus.clear();

	// Format data to be sent
	mJson->FormatDevicePollingData(*(data.second), postData);

	// Format json data to std string
	std::string payLoad(postData.toStyledString());

    // First set the URL that is about to receive our POST.
    if ( curl_easy_setopt(mCurl.get(), CURLOPT_URL, url.c_str()) != CURLE_OK )
    {
    	return status;
    }

    // Format Headers
    struct curl_slist *headers = addSecurityHeaders(static_cast<int>(TYPE_JSON));
//    headers = curl_slist_append(headers, data.second.GetHeaderFormat().c_str());

    // Pass custom made headers
    if ( curl_easy_setopt(mCurl.get(), CURLOPT_HTTPHEADER, headers) != CURLE_OK )
    {
    	return status;
    }

    if ( curl_easy_setopt(mCurl.get(), CURLOPT_CUSTOMREQUEST, "POST") != CURLE_OK )
    {
    	return status;
    }

	// Set the size of the postfields data
	if ( curl_easy_setopt(mCurl.get(), CURLOPT_POSTFIELDSIZE, payLoad.length()) != CURLE_OK )
	{
		return status;
	}

	/* Now specify the POST data */
	if ( curl_easy_setopt(mCurl.get(), CURLOPT_POSTFIELDS, payLoad.c_str()) != CURLE_OK )
	{
		return status;
	}

	if (curl_easy_perform(mCurl.get()) == CURLE_OK)
	{
		status = SUCCESS;
	}

    /* free headers */
    curl_slist_free_all(headers);

    std::cout<< "POST Status : " << strPostStatus << std::endl;

	return status;
}

int CurlHttpWrapper::GetData(const std::string deviceUri)
{
	int status = FAILURE;
	std::string url = serverUrl + deviceUri;

	if ( curl_easy_setopt(mCurl.get(), CURLOPT_SSL_VERIFYPEER, 1L) != CURLE_OK)
	{
		return status;
	}

	if ( curl_easy_setopt(mCurl.get(), CURLOPT_URL, url.c_str()) != CURLE_OK)
	{
		return status;
	}

	/* require use of SSL for this, or fail */
	if ( curl_easy_setopt(mCurl.get(), CURLOPT_USE_SSL, CURLUSESSL_ALL) != CURLE_OK )
	{
		return status;
	}

	  /* use a GET to fetch this */
	if ( curl_easy_setopt(mCurl.get(), CURLOPT_HTTPGET, 1L) != CURLE_OK)
	{
		return status;
	}

	if ( curl_easy_perform(mCurl.get()) == CURLE_OK)
	{
		status = SUCCESS;
	}

	return status;
}

int CurlHttpWrapper::PutData(const std::string deviceUri, const RestApiData & data)
{
	int status = SUCCESS;
	return status;
}

int CurlHttpWrapper::DeleteData(const std::string deviceUri)
{
	int status = SUCCESS;
	return status;
}

void CurlHttpWrapper::HandlePostData()
{
	std::pair<std::string,std::unique_ptr<RestApiData>> payLoad = {};
	bool empty = false;
	while(mActive.load(std::memory_order_relaxed))
	{
		{
			std::lock_guard<std::mutex> queuelock(mPostQueueMutex);
			empty = mPostQueue.empty();
			if (!empty)
			{
				payLoad = std::move(mPostQueue.front());
				mPostQueue.pop();
			}
		}

		if (!empty)
		{
			Post(payLoad);
			//####TODO: Log the return code if its failed.
		}
	}
}

int CurlHttpWrapper::SetCertificateAttributes(const Certificate & cert)
{
	this->cert = cert;
	return (UpdateTLSInfo());
}

// Update curl about the TLS credentials
int CurlHttpWrapper::UpdateTLSInfo()
{
	int status = FAILURE;

	// Set key
	if ( curl_easy_setopt(mCurl.get(), CURLOPT_SSLCERTTYPE, cert.strKeyType.c_str()) != CURLE_OK)
	{
		return status;
	}

	/* set the cert for client authentication */
	if ( curl_easy_setopt(mCurl.get(), CURLOPT_SSLCERT, cert.strCertificate.c_str()) != CURLE_OK)
	{
		return status;
	}

	/* sorry, for engine we must set the passphrase
	 (if the key has one...) */
	if ( curl_easy_setopt(mCurl.get(), CURLOPT_KEYPASSWD, cert.strPassword.c_str()) != CURLE_OK)
	{
		return status;
	}

	/* if we use a key stored in a crypto engine,
	 we must set the key type to "ENG" */
	if ( curl_easy_setopt(mCurl.get(), CURLOPT_SSLKEYTYPE, cert.strKeyType.c_str()) !=  CURLE_OK)
	{
		return status;
	}

	/* set the private key (file or ID in engine) */
	if ( curl_easy_setopt(mCurl.get(), CURLOPT_SSLKEY, cert.strKeyName.c_str()) !=  CURLE_OK)
	{
		return status;
	}

	/* set the file with the certs vaildating the server */
	if ( curl_easy_setopt(mCurl.get(), CURLOPT_CAINFO, cert.strCAcertificate.c_str()) !=  CURLE_OK)
	{
		return status;
	}

	/* disconnect if we can't validate server's cert */
	if ( curl_easy_setopt(mCurl.get(), CURLOPT_SSL_VERIFYPEER, 1L) !=  CURLE_OK)
	{
		return SUCCESS;
	}

	return status;
}

int CurlHttpWrapper::PostPollingData(const std::string deviceUri, const std::string jsonData)
{
	std::lock_guard<std::mutex> guard(mPollingMutex);

	int status = FAILURE;
	std::string url = serverUrl + deviceUri;

	// Clear the POST status info
	strPostStatus.clear();

    // First set the URL that is about to receive our POST.
    if ( curl_easy_setopt(mCurl.get(), CURLOPT_URL, url.c_str()) != CURLE_OK )
    {
    	return status;
    }

    // Format Headers
    struct curl_slist *headers= addSecurityHeaders(static_cast<int>(TYPE_JSON));
    headers = curl_slist_append(headers, "Content-Type: application/json");

    // Pass custom made headers
    if ( curl_easy_setopt(mCurl.get(), CURLOPT_HTTPHEADER, headers) != CURLE_OK )
    {
    	return status;
    }

    if ( curl_easy_setopt(mCurl.get(), CURLOPT_CUSTOMREQUEST, "POST") != CURLE_OK )
    {
    	return status;
    }

	// Set the size of the postfields data
	if ( curl_easy_setopt(mCurl.get(), CURLOPT_POSTFIELDSIZE, jsonData.length()) != CURLE_OK )
	{
		return status;
	}

	/* Now specify the POST data */
	if ( curl_easy_setopt(mCurl.get(), CURLOPT_POSTFIELDS, jsonData.c_str()) != CURLE_OK )
	{
		return status;
	}

	if (curl_easy_perform(mCurl.get()) == CURLE_OK)
	{
		status = SUCCESS;
	}

    /* free headers */
    curl_slist_free_all(headers);

    std::cout<< "POST Status : " << strPostStatus << std::endl;

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
	char ctime[12];
	snprintf(ctime, sizeof(ctime), "%ld", time(0));
	const std::string sstm(ctime);
	const std::string key("lop+2dzuioa/000mojijiaop");
	const std::string token(key + sstm);

	std::string sstm_encoded = base64_encode(
			reinterpret_cast<const unsigned char*>(sstm.c_str()),
			sstm.length());
	std::string Data1("sstm: " + sstm_encoded);

	std::string key_encoded = base64_encode(
			reinterpret_cast<const unsigned char*>(key.c_str()), key.length());
	std::string Data2("public_key: " + key_encoded);

	std::string token_sha = sha256(token);
	std::string Data3("token: " + token_sha);

	struct curl_slist *headers = NULL;
	headers = curl_slist_append(headers, "cache-control: no-cache");
	headers = curl_slist_append(headers, Data2.c_str());
	headers = curl_slist_append(headers, Data1.c_str());
	headers = curl_slist_append(headers, Data3.c_str());

	switch(data_type)
	{
	case TYPE_WWW_FORM:
		headers = curl_slist_append(headers, "content-type: application/x-www-form-urlencoded");
		break;
	case TYPE_JSON:
	default:
		headers = curl_slist_append(headers, "content-type: application/json");
		break;
	}

	return headers;
}
