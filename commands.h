#pragma once
#include <dpp/dpp.h>
#include <iostream>
#include <fstream>
#include <string>
#include <filesystem>
#include "extension.h"
#include <vector>
#include "logger.h";

std::string get_userid(std::string);

dpp::embed help() {
	/* create the embed */
	dpp::embed embed = dpp::embed().
		set_color(dpp::colors::sti_blue).
		set_title("Help").
		set_description("**All Available Commands**\nPrefix: '>>'").
		add_field(
			"**help**",
			"This thingy."
		).
		add_field(
			"**link (your_osu_user_id)**",
			"Link your osu! profile to your discord account.\nEx: >>link 13817114"
		).
		add_field(
			"**profile**",
			"Fetch osu! Profile stats (you must be linked).\nIf you want to fetch unlinked users: `>>profile.(osu_username)` (yes with the dot)\nIf you want to fetch by userID: `>>profile (osu_user_id)` both without the brackets."
		).
		add_field(
			"**scorefarming**",
			"*scorefarming*"
		).
		add_field(
			"**linked**",
			"Get all osu! Players who are linked to this bot."
		).
		add_field(
			"**lb_rs | lb_ts | lb_ss | lb_s | lb_a | lb_sr**",
			"Leaderboard of linked people:\nrs = Ranked Score | ts = Total Score | ss = SS Count | s = S Count | a = A Count | sr = Score Rank"
		).
		add_field(
			"**daily | weekly**", // not implemented
			"Get current daily/weekly progress."
		).
		add_field(
			"**daily_all | weekly_all**", // not implemented
			"Get current daily/weekly progress but all."
		).
		add_field(
			"**when_daily | when_weekly**", // not implemented
			"When Daily/Weekly ???????..."
		).
		add_field(
			"**about**",
			"Info about the Bot."
		).
		add_field(
			"**runtime**",
			"Runtime stats about the bot."
		).
		add_field(
			"**owner**",
			"Bot owner commands."
		).
		set_timestamp(time(0));
	return embed;
}

dpp::embed about(std::string bot_pfp) {
	/* create the embed */
	dpp::embed embed = dpp::embed().
		set_color(dpp::colors::sti_blue).
		set_title("About - " + bot_version).
		set_thumbnail(bot_pfp).
		add_field(
			"**Creator**",
			"Discord: <@302807502247165953>\nosu! Profile: <https://osu.ppy.sh/users/13817114>"
		).
		add_field(
			"**Launguage**",
			"Everything in C++"
		).
		add_field(
			"**Base**",
			"Bot was made with the C++ Library called D++ (dpp) a Discord API Library.\n<https://dpp.dev/>"
		).
		add_field(
			"**Additional Libraries**",
			"libcurl: <https://curl.se/libcurl/c/>\ncpr: <https://github.com/libcpr/cpr>\njson: <https://github.com/nlohmann/json>"
		).
		add_field(
			"**Score Rank API**",
			"By Respektive, source: <https://github.com/respektive/osu-score-rank-api>"
		).
		set_timestamp(time(0));
	return embed;
}

std::string register_player(std::string discord_id, std::string osu_id, std::string discord_username) {
	if (fetch_username(osu_id) == ".$error") {
		return "Error: osu! id is not valid, user doesnt exist";
	}
	//std::string log;
	if (!std::filesystem::exists("data/register.txt")) {
		// File doesn't exist, create it and write the new content
		std::ofstream outputFile("data/register.txt");
		if (outputFile.is_open()) {
			//outputFile << "" << std::endl;
			outputFile.close();
			console_log(0, "'register.txt' File created.");
		}
		else {
			console_log(0, "Failed to create 'register.txt' File.");
		}
	}

	std::ifstream inputFile("data/register.txt");
	std::string fileContent, line;
	bool searchStringFound = false;
	// Check if the file exists
	if (inputFile) {
		// File exists, read its content
		while (std::getline(inputFile, line)) {
			if (containsSubstring(line, discord_id)) {
				// Replace the line with the new string
				fileContent += discord_id + "," + osu_id + "," + split_discordUsername(line) + "\n";
				searchStringFound = true;
			}
			else {
				fileContent += line + "\n";
			}
		}
		inputFile.close();
	}
	if (!searchStringFound) {
		fileContent += discord_id + "," + osu_id + "," + discord_username + "\n";
	}
	// File doesn't exist or was successfully read, now open it for writing
	std::ofstream outputFile("data/register.txt");
	if (outputFile.is_open()) {
		outputFile << fileContent; // Write the modified content back to the file
		outputFile.close();
	}
	std::string osuid = get_userid(discord_id); //caching id to prevent multiple attempts of accessing file
	// Performing output and additional checks
	if (osuid == "") {
		return "Seems like something went wrong, couldnt save user.";
	}
	else {
		return "Seems like everything went well.\nYou are now linked under the username: " + fetch_username(osuid) + "\nYour Daily Tracking will start on the next day.";
	}
}

std::string get_userid(std::string discord_id) {
	std::ifstream inputFile("data/register.txt");
	std::string line;
	std::string searchStringFound = "";
	// Check if the file exists
	if (inputFile) {
		// File exists, read its content
		while (std::getline(inputFile, line)) {
			if (containsSubstring(line, discord_id)) {
				// filter osu!id
				//ReplaceStringInPlace(line, discord_id + ",", "");
				searchStringFound = split_osuid(line);
			}
		}
		inputFile.close();
	}

	return searchStringFound;
}

std::string get_all_users() {
	int limit = 0, index = 0;
	std::vector<std::string> userArray;
	userArray.push_back(""); // init
	std::ifstream inputFile("data/register.txt");
	std::string line = "", arrayContent = "";
	// Check if the file exists
	if (inputFile) {
		// File exists, read its content
		while (std::getline(inputFile, line)) {
			if (containsSubstring(line, ",")) {
				std::string osu_id = split_osuid(line);
				limit++;

				if (limit == 1)
				{
					arrayContent += "ids[]=" + osu_id;
				}
				else {
					if (limit > 50)
					{
						//	userArray[index] = arrayContent;
						limit = 1;
						index++;
						arrayContent = "";
						arrayContent += "ids[]=" + osu_id;
						userArray.push_back(arrayContent);
					}
					else {
						arrayContent += "&ids[]=" + osu_id;
					}
				}

				userArray[index] = arrayContent;
			}
		}
		inputFile.close();

		return multiple_users(userArray);
	}
	else {
		return "no file";
	}
}

std::string get_all_data(std::string input) {
	int limit = 0, index = 0;
	std::vector<std::string> userArray;
	userArray.push_back(""); // init
	std::ifstream inputFile("data/register.txt");
	std::string line = "", arrayContent = "";
	// Check if the file exists
	if (inputFile) {
		// File exists, read its content
		while (std::getline(inputFile, line)) {
			if (containsSubstring(line, ",")) {
				std::string osu_id = split_osuid(line);
				limit++;

				if (limit == 1)
				{
					if (input == "sr") {
						arrayContent += osu_id;
					}
					else {
						arrayContent += "ids[]=" + osu_id;
					}
				}
				else {
					if (limit > 50)
					{
						//	userArray[index] = arrayContent;
						limit = 1;
						index++;
						arrayContent = "";
						if (input == "sr") {
							arrayContent += osu_id;
						}
						else {
							arrayContent += "ids[]=" + osu_id;
						}
						userArray.push_back(arrayContent);
					}
					else {
						if (input == "sr") {
							arrayContent += "," + osu_id;
						}
						else {
							arrayContent += "&ids[]=" + osu_id;
						}
					}
				}

				userArray[index] = arrayContent;
			}
		}
		inputFile.close();

		return multiple_data(userArray, input);
	}
	else {
		return "no file";
	}
}

void dump_daily() {
	console_log(1, "Dump Daily function.");
	int limit = 0, index = 0;
	std::vector<std::string> userArray;
	userArray.push_back(""); // init
	std::ifstream inputFile("data/register.txt");
	std::string line = "", arrayContent = "";
	// Check if the file exists
	if (inputFile) {
		// File exists, read its content
		while (std::getline(inputFile, line)) {
			if (containsSubstring(line, ",")) {
				std::string osu_id = split_osuid(line);
				limit++;

				if (limit == 1)
				{
					arrayContent += "ids[]=" + osu_id;
				}
				else {
					if (limit > 50)
					{
						//	userArray[index] = arrayContent;
						limit = 1;
						index++;
						arrayContent = "";
						arrayContent += "ids[]=" + osu_id;
						userArray.push_back(arrayContent);
					}
					else {
						arrayContent += "&ids[]=" + osu_id;
					}
				}

				userArray[index] = arrayContent;
			}
		}
		inputFile.close();
		console_log(1, "'register.txt' opened.");
		console_log(1, "Array Size: " + std::to_string(userArray.size()) + ", latest array content: " + arrayContent);

		dump_all(userArray);
	}
	else {
		console_log(1, "'register.txt' couldnt open.");
	}
}

// IMPLEMENTED
void dump_weekly() {
	console_log(1, "Dump Weekly function.");
	int limit = 0, index = 0;
	std::vector<std::string> userArray;
	userArray.push_back(""); // init
	std::ifstream inputFile("data/register.txt");
	std::string line = "", arrayContent = "";
	// Check if the file exists
	if (inputFile) {
		// File exists, read its content
		while (std::getline(inputFile, line)) {
			if (containsSubstring(line, ",")) {
				std::string osu_id = split_osuid(line);
				limit++;

				if (limit == 1)
				{
					arrayContent += "ids[]=" + osu_id;
				}
				else {
					if (limit > 50)
					{
						//	userArray[index] = arrayContent;
						limit = 1;
						index++;
						arrayContent = "";
						arrayContent += "ids[]=" + osu_id;
						userArray.push_back(arrayContent);
					}
					else {
						arrayContent += "&ids[]=" + osu_id;
					}
				}

				userArray[index] = arrayContent;
			}
		}
		inputFile.close();
		console_log(1, "'register.txt' opened.");
		console_log(1, "Array Size: " + std::to_string(userArray.size()) + ", latest array content: " + arrayContent);

		dump_all_weekly(userArray);
	}
	else {
		console_log(1, "'register.txt' couldnt open.");
	}
}

dpp::embed dump_compare(std::string dayTitle) {
	console_log(1, "Top 3 Dumping started.");
	int limit = 0, index = 0;
	std::vector<std::string> userArray;
	userArray.push_back(""); // init
	//get from any daily dump as all should have the same users, or i did some shit wrong
	std::ifstream inputFile("data/dump_daily_rankedscore.txt");
	std::string line = "", arrayContent = "";
	// Check if the file exists
	if (inputFile) {
		// File exists, read its content
		while (std::getline(inputFile, line)) {
			if (containsSubstring(line, ",")) {
				std::string osu_id = split_discordid(line);
				limit++;

				if (limit == 1)
				{
					arrayContent += "ids[]=" + osu_id;
				}
				else {
					if (limit > 50)
					{
						//	userArray[index] = arrayContent;
						limit = 1;
						index++;
						arrayContent = "";
						arrayContent += "ids[]=" + osu_id;
						userArray.push_back(arrayContent);
					}
					else {
						arrayContent += "&ids[]=" + osu_id;
					}
				}

				userArray[index] = arrayContent;
			}
		}
		inputFile.close();
		console_log(1, "'dump_daily_rankedscore.txt' opened.");
		console_log(1, "Array Size: " + std::to_string(userArray.size()) + ", latest array content: " + arrayContent);
		console_log(1, "Getting current Top3 stats from osu! api...");
		return compare(userArray, dayTitle);
	}
	else {
		console_log(2, "Couldn't open 'dump_daily_rankedscore.txt' file.");
	}
}

std::string dump_compare_all(std::string dayTitle) {
	int limit = 0, index = 0;
	std::vector<std::string> userArray;
	userArray.push_back(""); // init
	//get from any daily dump as all should have the same users, or i did some shit wrong
	std::ifstream inputFile("data/dump_daily_rankedscore.txt");
	std::string line = "", arrayContent = "";
	// Check if the file exists
	if (inputFile) {
		// File exists, read its content
		while (std::getline(inputFile, line)) {
			if (containsSubstring(line, ",")) {
				std::string osu_id = split_discordid(line);
				limit++;

				if (limit == 1)
				{
					arrayContent += "ids[]=" + osu_id;
				}
				else {
					if (limit > 50)
					{
						//	userArray[index] = arrayContent;
						limit = 1;
						index++;
						arrayContent = "";
						arrayContent += "ids[]=" + osu_id;
						userArray.push_back(arrayContent);
					}
					else {
						arrayContent += "&ids[]=" + osu_id;
					}
				}

				userArray[index] = arrayContent;
			}
		}
		inputFile.close();
		return compare_all(userArray, dayTitle);
	}
}

// IMPLEMENTED
dpp::embed dump_compare_weekly(std::string dayTitle) {
	console_log(1, "Top 3 Dumping started.");
	int limit = 0, index = 0;
	std::vector<std::string> userArray;
	userArray.push_back(""); // init
	//get from any weekly dump as all should have the same users, or i did some shit wrong
	std::ifstream inputFile("data/dump_weekly_rankedscore.txt");
	std::string line = "", arrayContent = "";
	// Check if the file exists
	if (inputFile) {
		// File exists, read its content
		while (std::getline(inputFile, line)) {
			if (containsSubstring(line, ",")) {
				std::string osu_id = split_discordid(line);
				limit++;

				if (limit == 1)
				{
					arrayContent += "ids[]=" + osu_id;
				}
				else {
					if (limit > 50)
					{
						//	userArray[index] = arrayContent;
						limit = 1;
						index++;
						arrayContent = "";
						arrayContent += "ids[]=" + osu_id;
						userArray.push_back(arrayContent);
					}
					else {
						arrayContent += "&ids[]=" + osu_id;
					}
				}

				userArray[index] = arrayContent;
			}
		}
		inputFile.close();
		console_log(1, "'dump_weeklyy_rankedscore.txt' opened.");
		console_log(1, "Array Size: " + std::to_string(userArray.size()) + ", latest array content: " + arrayContent);
		console_log(1, "Getting current Top3 stats from osu! api...");
		return compare_weekly(userArray, dayTitle);
	}
	else {
		console_log(2, "Couldn't open 'dump_weekly_rankedscore.txt' file.");
	}
}

// IMPLEMENTED
std::string dump_compare_all_weekly(std::string dayTitle) {
	int limit = 0, index = 0;
	std::vector<std::string> userArray;
	userArray.push_back(""); // init
	//get from any weekly dump as all should have the same users, or i did some shit wrong
	std::ifstream inputFile("data/dump_weekly_rankedscore.txt");
	std::string line = "", arrayContent = "";
	// Check if the file exists
	if (inputFile) {
		// File exists, read its content
		while (std::getline(inputFile, line)) {
			if (containsSubstring(line, ",")) {
				std::string osu_id = split_discordid(line);
				limit++;

				if (limit == 1)
				{
					arrayContent += "ids[]=" + osu_id;
				}
				else {
					if (limit > 50)
					{
						//	userArray[index] = arrayContent;
						limit = 1;
						index++;
						arrayContent = "";
						arrayContent += "ids[]=" + osu_id;
						userArray.push_back(arrayContent);
					}
					else {
						arrayContent += "&ids[]=" + osu_id;
					}
				}

				userArray[index] = arrayContent;
			}
		}
		inputFile.close();
		return compare_all_weekly(userArray, dayTitle);
	}
}
