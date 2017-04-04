/*
 * CommonDefs.h
 *
 *  Created on: Jan 13, 2017
 *      Author: developer
 */

#ifndef COMMONDEFS_H_
#define COMMONDEFS_H_

#include <iostream>
#include <memory>
#include <assert.h>

namespace rest
{
	enum class DEVICE_STATUS
	{
		GOOD = 0,
		BAD,
		UNCERTAIN
	};

	enum HTTP_STATUS
	{
		SUCCESS = 0,
		FAILURE
	};

	// enum to define content type
	enum REST_CONTENT_TYPE
	{
		TYPE_JSON = 0,
		TYPE_WWW_FORM
	};

	class Certificate
	{
	public:
		Certificate() : strPassword("")
					  , strCertificate("")
					  , strCAcertificate("")
					  , strKeyName("")
					  , strKeyType("")
		{

		}
		~Certificate() = default;
		Certificate(const Certificate & rhs) = default;
		Certificate & operator=(const Certificate & rhs) = default;
		Certificate(Certificate && rhs) = default;
		Certificate & operator=(Certificate && rhs) = default;
		std::string strPassword;
		std::string strCertificate;
		std::string strCAcertificate;
		std::string strKeyName;
		std::string strKeyType;
	};

	class NewDeviceData
	{
	private:
		std::string strDeviceTag;
		std::string strDeviceAddr;
		std::string strGatewayTag;
		std::string strGatewayAddr;
		int updateRate;
		int outOfService;
		std::string strNetworkId;
		int deviceType;
		std::string strHeaderFormat;

	public:
		NewDeviceData() : strDeviceTag("")
						, strDeviceAddr("")
						, strGatewayTag("")
						, strGatewayAddr("")
						, updateRate(0)
						, outOfService(0)
						, strNetworkId("")
						, deviceType(0)
						, strHeaderFormat("Content-Type: application/x-www-form-urlencoded")
		{

		}
		void SetDeviceTag(const std::string & tag) {strDeviceTag = tag;}
		void SetDeviceAddr(const std::string & addr) {strDeviceAddr = addr;}
		void SetGatewayTag(const std::string & tag) {strGatewayTag = tag;}
		void SetGatewayAddr(const std::string & addr) {strGatewayAddr = addr;}
		void SetUpdateRate(const int & rate) { updateRate = rate;}
		void SetOutOfService(const int & status) { outOfService = status;}
		void SetNetworkId(const std::string & id) {strNetworkId = id;}
		void SetDeviceType(const int & type ) { deviceType = type; }
		void SetHeaderFormat(const std::string & format) {strHeaderFormat = format;}

		std::string GetDeviceTag() const {return strDeviceTag;}
		std::string GetDeviceAddr() const {return strDeviceAddr;}
		std::string GetGatewayTag() const {return strGatewayTag;}
		std::string GetGatewayAddr() const {return strGatewayAddr;}
		int GetUpdateRate() const {return updateRate;}
		int GetOutOfService() const {return outOfService;}
		std::string GetNetworkId() const {return strNetworkId;}
		int GetDeviceType() const {return deviceType;}
		std::string GetHeaderFormat() const {return strHeaderFormat;}
	};

	class GateWayData
	{
	private:
		std::string strHostName;
		unsigned int port;
		std::string strGatewayTag;
		std::string strGatewayAddr;
		std::string strDescription;
		std::string strNetworkId;
		int outOfService;
		int secure;
		std::string strHeaderFormat;

	public:
		GateWayData() : strHostName("")
						, port(0)
						, strGatewayTag("")
						, strGatewayAddr("")
						, strDescription("")
						, strNetworkId("")
						, outOfService(0)
						, secure(0)
						, strHeaderFormat("Content-Type: application/x-www-form-urlencoded")
		{

		}

		void SetHostName(std::string & name) { strHostName = name; }
		void SetPort(unsigned int & port) { this->port = port; }
		void SetGateWayTag(std::string & tag) { strGatewayTag = tag; }
		void SetGateWayAddr(std::string & addr) { strGatewayAddr = addr; }
		void SetDescription(std::string & desc) { strDescription = desc; }
		void SetNetworkId(std::string & id) { strNetworkId = id; }
		void SetOutOfService(const int & status) { outOfService = status; }
		void SetSecue(int & secure) { this->secure = secure; }
		void SetHeaderFormat(const std::string & format) {strHeaderFormat = format;}

		std::string GetHostName() const { return strHostName; }
		unsigned int GetPort() const { return port; }
		std::string GetGateWayTag() const { return strGatewayTag; }
		std::string GetGateWayAddr() const { return strGatewayAddr; }
		std::string GetDescription() const { return strDescription; }
		std::string GetNetworkId() const { return strNetworkId; }
		int  GetOutOfService() const { return outOfService; }
		int GetSecue() const { return secure; }
		std::string GetHeaderFormat() const {return strHeaderFormat;}
	};


	class RestApiData
	{
	public:
		RestApiData (std::string tag = "") : m_tag(tag)
										   , m_status(0)
										   , m_value(0)
										   , m_server_time(0)
										   , m_source_time(0)
										   , strHeaderFormat("Content-Type: application/json")
		{

		}

		RestApiData(const RestApiData & rhs)
		{
			this->m_tag = rhs.m_tag;
			this->m_status = rhs.m_status;
			this->m_value = rhs.m_value;
			this->m_server_time = rhs.m_server_time;
			this->m_source_time = rhs.m_source_time;
		}

		RestApiData & operator=(const RestApiData & rhs)
		{
			if (this != &rhs)
			{
				this->m_tag = rhs.m_tag;
				this->m_status = rhs.m_status;
				this->m_value = rhs.m_value;
				this->m_server_time = rhs.m_server_time;
				this->m_source_time = rhs.m_source_time;
			}

			return *this;
		}

		RestApiData(RestApiData && rhs)
		{
			this->m_tag = rhs.m_tag;
			this->m_status = rhs.m_status;
			this->m_value = rhs.m_value;
			this->m_server_time = rhs.m_server_time;
			this->m_source_time = rhs.m_source_time;

			// Reset the source
			rhs.m_tag.clear();
			rhs.m_status = 0;
			rhs.m_value = 0;
			rhs.m_server_time = 0;
			rhs.m_source_time = 0;
		}

		RestApiData & operator=(RestApiData && rhs)
		{
			assert(this != &rhs);

			this->m_tag = rhs.m_tag;
			this->m_status = rhs.m_status;
			this->m_value = rhs.m_value;
			this->m_server_time = rhs.m_server_time;
			this->m_source_time = rhs.m_source_time;

			// Reset the source
			rhs.m_tag.clear();
			rhs.m_status = 0;
			rhs.m_value = 0;
			rhs.m_server_time = 0;
			rhs.m_source_time = 0;

			return *this;
		}

		~RestApiData()
		{

		}

		std::string GetTag() const { return m_tag; }

		void SetStatus(int status) { m_status = status; }

		int GetStatus() const { return m_status; }

		void SetValue(double value) { m_value = value; }

		double GetValue() const { return m_value; }

		void SetServerTimeUTC(unsigned long long time) { m_server_time = time; }

		unsigned long long GetServerTimeUTC() const { return m_server_time; }

		void SetSourceTimeUTC(unsigned long long time) { m_source_time = time; }

		long long GetSourceTimeUTC() const { return m_source_time; }

		void dump() {}; // for debug purpose

		void Clear() {m_tag.clear(); m_status = 0; m_value = 0; m_server_time = 0; m_source_time = 0;}

		void SetHeaderFormat(const std::string & format) {strHeaderFormat = format;}
		std::string GetHeaderFormat() const {return strHeaderFormat;}

	private:
		std::string m_tag; // tag name
		int m_status; // status
		double m_value; // value of pressure
		unsigned long long m_server_time; // server time
		unsigned long long m_source_time; // source time
		std::string strHeaderFormat;
	};

	// Class to manage rest data
	class RestData
	{
	public:
		RestData() : RestContent(""), type(TYPE_JSON) {}
		RestData(std::string content, REST_CONTENT_TYPE contentType) : RestContent(content), type(contentType) {}
		~RestData(){};
		void SetContent(const std::string & content) { RestContent = content; }
		void SetType(const REST_CONTENT_TYPE & type) { this->type = type; }
		std::string GetContent() const { return RestContent; }
		REST_CONTENT_TYPE GetType() const { return type; };

	private:
		std::string RestContent;
		REST_CONTENT_TYPE type;
	};
}

#endif /* COMMONDEFS_H_ */
