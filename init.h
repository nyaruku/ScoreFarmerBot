#pragma once
#include "config.h"
#include <cpr/cpr.h>
#include <json.hpp>
#include <filesystem>
#include <iostream>
#include "logger.h"
//#include "extension.h"

nlohmann::json AuthResult;
namespace fs = std::filesystem;

static void Auth() {
	std::string body = R"({"client_id":)" + client_id + R"(, "client_secret":")" + client_secret + R"(", "grant_type":"client_credentials", "scope":"public"})";
	auto r = cpr::Post(cpr::Url{ "https://osu.ppy.sh/oauth/token" },
		cpr::Body{
		body
	},
		cpr::Header{ { "Content-Type", "application/json" } });
	AuthResult = nlohmann::json::parse(r.text);
	auth_token = "Bearer " + AuthResult.value("access_token", "");
	AuthResult.clear();
}

static void pre() {
	if (fs::exists("data/") && fs::is_directory("data/")) {
	}
	else {
		if (fs::create_directory("data")) {
		}
		else {
		}
	}
	//Creating Log File
	if (!std::filesystem::exists("data/log.ansi")) {
		std::ofstream outputFile("data/log.ansi");
		if (outputFile.is_open()) {
			outputFile.close();
			console_log(0, "Log File created.");
		}
	}
}