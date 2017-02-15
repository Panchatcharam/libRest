/*
 * JsonParser.h
 *
 *  Created on: Jan 13, 2017
 *      Author: developer
 */

#ifndef JSONPARSER_H_
#define JSONPARSER_H_

#include "json/json-forwards.h"
#include "json/json.h"
#include "CommonDefs.h"

class JsonWrapper
{
public:
	int FormatDevicePollingData(const RestApiData & payload, Json::Value & postData);
	int FormatDeviceEntryData();
	int FormatGatewayData();
};

#endif /* JSONPARSER_H_ */
