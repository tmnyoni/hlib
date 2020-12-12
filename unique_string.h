#pragma once

#include <rpc.h>
#include <string>
#include <iostream>
#include "hlib.h"

#pragma comment(lib, "rpcrt4.lib")

std::string unique_string() {
	std::string string_uuid;

	UUID uuid;
	UuidCreate(&uuid);

	RPC_CSTR uuid_string;
	RPC_STATUS status = UuidToStringA(&uuid, &uuid_string);
	string_uuid += reinterpret_cast<char*>(uuid_string);
	RpcStringFreeA(&uuid_string);

	return string_uuid;
}

std::string unique_short_string() {
	std::string uuid = unique_string();

	const auto idx = uuid.find('-');
	if (idx != std::string::npos)
		return uuid.substr(0, idx);
	else
		return uuid;
}

std::string custom_uid(std::string prefix) {
	return prefix.append(unique_short_string());
}

	