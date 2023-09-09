#pragma once
#include <string>
#include "secret.h" // DONT PUSH TO GITHUB
static std::string bot_token = secret_bot_token;
static std::string prefix = ">>";
static std::string client_id = secret_client_id;
static std::string client_secret = secret_client_secret;
static std::string auth_token = "";
static std::string default_mode = "osu";
static std::string owner_profile_id = "13817114"; // osu! user id of owner
static std::string owner_discord_id = "302807502247165953"; // discord user id of owner 
static std::string logging_channel_id = secret_logging_channel_id;
static std::string log_file = "data/log.ansi"; // path to log file
static std::string bot_version = "v.1.5";
static bool production = true; // only true in Production