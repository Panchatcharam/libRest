/*
 * JsonParser.cpp
 *
 *  Created on: Jan 13, 2017
 *      Author: developer
 */
#include "JsonParser.h"

int JsonWrapper::FormatDevicePollingData(const RestApiData & payload, Json::Value & postData)
{
	Json::Value deviceData;

	deviceData["date"] = Json::Value(static_cast<Json::Value::UInt64>(payload.GetServerTimeUTC()));
	deviceData["device_tag"] = Json::Value(payload.GetTag());
	deviceData["device_address"] = Json::Value("0x123456");

	Json::Value procdata(Json::arrayValue);

	Json::Value subProcData;

	///////
	subProcData["dv_label"] = Json::Value("PV");
	subProcData["dv_num"] 	= Json::Value(0);
	subProcData["dv_unit"] 	= Json::Value("HaiXiang");
	subProcData["dv_value"] = Json::Value(payload.GetValue());
	subProcData["dv_status"]= Json::Value(payload.GetStatus());
	subProcData["dv_TS"] 	= Json::Value(static_cast<Json::Value::UInt64>(payload.GetServerTimeUTC()));

	procdata.append(subProcData);
	subProcData.clear();

	subProcData["dv_label"] = Json::Value("SV");
	subProcData["dv_num"] 	= Json::Value(1);
	subProcData["dv_unit"] 	= Json::Value("Panch");
	subProcData["dv_value"] = Json::Value(18.354);
	subProcData["dv_status"]= Json::Value("0xd0");
	subProcData["dv_TS"] 	= Json::Value("47679635674");

	procdata.append(subProcData);
	subProcData.clear();

	subProcData["dv_label"] = Json::Value("TV");
	subProcData["dv_num"] 	= Json::Value(1);
	subProcData["dv_unit"] 	= Json::Value("true");
	subProcData["dv_value"] = Json::Value(26.6);
	subProcData["dv_status"]= Json::Value("0xc0");
	subProcData["dv_TS"] 	= Json::Value("1481060117000");

	procdata.append(subProcData);
	subProcData.clear();

	subProcData["dv_label"] = Json::Value("QV");
	subProcData["dv_num"] 	= Json::Value(0);
	subProcData["dv_unit"] 	= Json::Value("V");
	subProcData["dv_value"] = Json::Value(123.2);
	subProcData["dv_status"]= Json::Value("0xc0");
	subProcData["dv_TS"] 	= Json::Value("1481060117000");

	procdata.append(subProcData);
	subProcData.clear();

	subProcData["dv_label"] = Json::Value("Range");
	subProcData["dv_num"] 	= Json::Value(244);
	subProcData["dv_unit"] 	= Json::Value("%");
	subProcData["dv_value"] = Json::Value(0.2);
	subProcData["dv_status"]= Json::Value("0xc0");
	subProcData["dv_TS"] 	= Json::Value("1481060117000");

	procdata.append(subProcData);
	subProcData.clear();

	subProcData["dv_label"] = Json::Value("Loop");
	subProcData["dv_num"] 	= Json::Value(242);
	subProcData["dv_unit"] 	= Json::Value("mA");
	subProcData["dv_value"] = Json::Value(0);
	subProcData["dv_status"]= Json::Value("0x30");
	subProcData["dv_TS"] 	= Json::Value("1481060117000");

	procdata.append(subProcData);
	subProcData.clear();

	deviceData["data"]["process_data"] = procdata;
    ////////


	subProcData["basic_status"] = Json::Value("0x00");
	subProcData["extended_status"] = Json::Value("0x00");
	subProcData["standardized_status"] = Json::Value("0x00");
	subProcData["diag_data"] = Json::Value("0x00");
	subProcData["diag_TS"] = Json::Value("0x00");

	deviceData["data"]["diagnostics"] = subProcData;

	postData = deviceData;

	return 0;
}

int JsonWrapper::FormatDeviceEntryData()
{
	// ?hostname=123&
	return 0;
}

int JsonWrapper::FormatGatewayData()
{
	return 0;
}
