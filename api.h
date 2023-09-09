#pragma once
#include <iostream>
#include <sstream>
#include <curl/curl.h>
#include <json.hpp>
#include "extension.h"
#include <cpr/cpr.h>
#include <dpp/dpp.h> // for embeds
#include <vector>
#include <algorithm> // for sorting
#include "logger.h"


// TODO: COMBINE DAILY AND WEEKLY FUNCTION FOR SIMPLICITY


using json = nlohmann::json;

//userid
dpp::embed profile_stats(std::string userid, std::string mode) {
	std::stringstream buffer;
	json PostResult;
	json RespektiveResult;

	std::string
		username = "username",
		rankedScore = "0",
		totalScore = "0",
		scoreRank = "0",
		playTime = "0",
		playCount = "0",
		silverSS = "0",
		goldSS = "0",
		silverS = "0",
		goldS = "0",
		rankA = "0",
		avatar_url = "";

	int hour = 0,
		minute = 0,
		second = 0;

	auto g = cpr::Get(cpr::Url{ "https://osu.ppy.sh/api/v2/users/" + userid + "/" + mode + "?key=id" },
		cpr::Header{
			{ "Content-Type", "application/json" },
			{ "Accept", "application/json" },
			{ "Authorization", auth_token }
	});

	PostResult = json::parse(g.text);
	username = PostResult.value("username", "error");
	avatar_url = PostResult.value("avatar_url", "error");

	/*buffer << PostResult["statistics"]["play_time"];
	playTime = buffer.str();
	buffer.str("");*/

	buffer << PostResult["statistics"]["ranked_score"];
	rankedScore = buffer.str();
	rankedScore = FormatWithCommas(std::stod(rankedScore.c_str()));
	std::replace(rankedScore.begin(), rankedScore.end(), ',', '.');
	ReplaceStringInPlace(rankedScore, ".000000", "");
	buffer.str("");

	buffer << PostResult["statistics"]["total_score"];
	totalScore = buffer.str();
	totalScore = FormatWithCommas(std::stod(totalScore.c_str()));
	std::replace(totalScore.begin(), totalScore.end(), ',', '.');
	ReplaceStringInPlace(totalScore, ".000000", "");
	buffer.str("");

	buffer << PostResult["statistics"]["play_count"];
	playCount = buffer.str();
	playCount = FormatWithCommas(atoi(playCount.c_str()));
	std::replace(playCount.begin(), playCount.end(), ',', '.');
	ReplaceStringInPlace(playCount, ".000000", "");
	buffer.str("");

	buffer << PostResult["statistics"]["grade_counts"]["ssh"];
	silverSS = buffer.str();
	silverSS = FormatWithCommas(atoi(silverSS.c_str()));
	std::replace(silverSS.begin(), silverSS.end(), ',', '.');
	ReplaceStringInPlace(silverSS, ".000000", "");
	buffer.str("");

	buffer << PostResult["statistics"]["grade_counts"]["ss"];
	goldSS = buffer.str();
	goldSS = FormatWithCommas(atoi(goldSS.c_str()));
	std::replace(goldSS.begin(), goldSS.end(), ',', '.');
	ReplaceStringInPlace(goldSS, ".000000", "");
	buffer.str("");

	buffer << PostResult["statistics"]["grade_counts"]["sh"];
	silverS = buffer.str();
	silverS = FormatWithCommas(atoi(silverS.c_str()));
	std::replace(silverS.begin(), silverS.end(), ',', '.');
	ReplaceStringInPlace(silverS, ".000000", "");
	buffer.str("");

	buffer << PostResult["statistics"]["grade_counts"]["s"];
	goldS = buffer.str();
	goldS = FormatWithCommas(atoi(goldS.c_str()));
	std::replace(goldS.begin(), goldS.end(), ',', '.');
	ReplaceStringInPlace(goldS, ".000000", "");
	buffer.str("");

	auto respektive = cpr::Get(cpr::Url{ "https://score.respektive.pw/u/" + userid + "?mode=" + mode });
	RespektiveResult = json::parse(respektive.text);

	buffer << RespektiveResult[0].value("rank", 0);
	scoreRank = buffer.str();
	scoreRank = FormatWithCommas(atoi(scoreRank.c_str()));
	std::replace(scoreRank.begin(), scoreRank.end(), ',', '.');
	ReplaceStringInPlace(scoreRank, ".000000", "");
	buffer.str("");

	/*std::cout << playTime << std::endl;
	int timeplay = atoi(playTime.c_str());
	hour = (timeplay / 3600);
	minute = ((timeplay - hour * 3600) / 60);
	second = (timeplay - hour * 3600) - (minute * 60);
	*/

	buffer << PostResult["statistics"]["grade_counts"]["a"];
	rankA = buffer.str();
	rankA = FormatWithCommas(atoi(rankA.c_str()));
	std::replace(rankA.begin(), rankA.end(), ',', '.');
	ReplaceStringInPlace(rankA, ".000000", "");
	buffer.str("");

	/* create the embed */
	dpp::embed embed = dpp::embed().
		set_color(dpp::colors::sti_blue).
		set_title(username).
		set_url("https://osu.ppy.sh/users/" + userid).
		//   set_author("Stats", "https://osu.ppy.sh/users/" + userid, avatar_url).
		//   set_description("Some description here").
		set_thumbnail(avatar_url).
		add_field(
			"**Stats**",
			"**Ranked Score: **" + rankedScore + "\n" +
			"**Total Score: **" + totalScore + "\n" +
			"**Score Rank: **" + "#" + scoreRank + "\n" +
			"**Play Count: **" + playCount + "\n" +
			"**Silver SS: **" + silverSS + "\n" +
			"**Gold SS: **" + goldSS + "\n" +
			"**Silver S: **" + silverS + "\n" +
			"**Gold S: **" + goldS + "\n" +
			"**A: **" + rankA
		).

		//	set_image(PostResult.value("avatar_url", "https://a.ppy.sh/1?1501234567.jpeg")).
			//   set_footer(dpp::embed_footer().set_text("Some footer text here").set_icon("https://dpp.dev/DPP-Logo.png")).
		set_timestamp(time(0));
	PostResult.clear();
	RespektiveResult.clear();
	buffer.clear();

	return embed;
}

dpp::embed profile_stats_name(std::string osu_username, std::string mode) {
	json PostResult;
	ReplaceStringInPlace(osu_username, " ", "%20");
	int osu_id = 0;
	auto g = cpr::Get(cpr::Url{ "https://osu.ppy.sh/api/v2/users/" + osu_username + "/" + mode + "?key=username" },
		cpr::Header{
			{ "Content-Type", "application/json" },
			{ "Accept", "application/json" },
			{ "Authorization", auth_token }
	});
	PostResult = json::parse(g.text);
	osu_id = PostResult.value("id", 0);

	return profile_stats(std::to_string(osu_id), default_mode);
}

std::string fetch_username(std::string osu_userid) {
	json PostResult;
	auto g = cpr::Get(cpr::Url{ "https://osu.ppy.sh/api/v2/users/" + osu_userid + "/" + "osu" + "?key=id" },
		cpr::Header{
			{ "Content-Type", "application/json" },
			{ "Accept", "application/json" },
			{ "Authorization", auth_token }
	});

	PostResult = json::parse(g.text);
	return PostResult.value("username", ".$error");
}

std::string multiple_users(std::vector<std::string> id_url) {
	std::stringstream buffer;
	json PostResult;
	json RespektiveResult;

	std::string output = "All Linked Users:\n------------------------\n";
	int nr = 0;

	for (size_t i = 0; i < id_url.size(); ++i) {
		auto g = cpr::Get(cpr::Url{ "https://osu.ppy.sh/api/v2/users?" + id_url[i] },
			cpr::Header{
				{ "Content-Type", "application/json" },
				{ "Accept", "application/json" },
				{ "Authorization", auth_token }
		});

		PostResult = json::parse(g.text);
		// Loop through each user
		for (const auto& user : PostResult["users"]) {
			nr++;
			output += std::to_string(nr) + " >> " + user.value("username", "Error") + "\n";
		}
	}
	ReplaceStringInPlace(output, "All Linked Users:", "All Linked Users (" + std::to_string(nr) + "):");
	return removeEmptyLineAtEnd(output);
}

std::string multiple_data(std::vector<std::string> id_url, std::string input) {
	std::stringstream buffer;
	json PostResult;
	if (input == "rs") {
		std::string rankedScore;
		std::string output = "Ranked Score Leaderboard:\n------------------------\n";

		for (size_t i = 0; i < id_url.size(); ++i) {
			auto g = cpr::Get(cpr::Url{"https://osu.ppy.sh/api/v2/users?" + id_url[i]},
				cpr::Header{
					{"Content-Type", "application/json"},
					{ "Accept", "application/json" },
				{ "Authorization", auth_token }});

			PostResult = json::parse(g.text);

			// Collect ranked scores into a vector of pairs (username, ranked_score)
			std::vector<std::pair<std::string, double>> rankedScores;

			// Loop through each user
			for (const auto& user : PostResult["users"]) {
				// Extract username
				std::string username = user.value("username", "Error");

				buffer << user["statistics_rulesets"]["osu"]["ranked_score"];
				rankedScore = buffer.str();
				double score = std::stod(rankedScore.c_str());
				buffer.str("");

				rankedScores.emplace_back(username, score);
			}

			// Sort the ranked scores in descending order
			std::sort(rankedScores.begin(), rankedScores.end(), [](const auto& a, const auto& b) {
				return a.second > b.second; // Compare by ranked score
				});

			// Generate the leaderboard using the sorted data
			int nr = 1;
			for (const auto& entry : rankedScores) {
				std::string value = std::to_string(entry.second);
				value = FormatWithCommas(entry.second);
				std::replace(value.begin(), value.end(), ',', '.');
				ReplaceStringInPlace(value, ".000000", "");
				output += std::to_string(nr) + " >> " + entry.first + " - " + value + "\n";
				nr++; // Increment the counter
			}
		}

		return removeEmptyLineAtEnd(output);
	}
	if (input == "ts") {
		std::string totalScore;
		std::string output = "Total Score Leaderboard:\n------------------------\n";

		for (size_t i = 0; i < id_url.size(); ++i) {
			auto g = cpr::Get(cpr::Url{"https://osu.ppy.sh/api/v2/users?" + id_url[i]},
				cpr::Header{
					{"Content-Type", "application/json"},
					{ "Accept", "application/json" },
				{ "Authorization", auth_token }});

			PostResult = json::parse(g.text);

			// Collect ranked scores into a vector of pairs (username, score)
			std::vector<std::pair<std::string, double>> totalScores;

			// Loop through each user
			for (const auto& user : PostResult["users"]) {
				// Extract username
				std::string username = user.value("username", "Error");

				buffer << user["statistics_rulesets"]["osu"]["total_score"];
				totalScore = buffer.str();
				double score = std::stod(totalScore.c_str());
				buffer.str("");

				totalScores.emplace_back(username, score);
			}

			// Sort the ranked scores in descending order
			std::sort(totalScores.begin(), totalScores.end(), [](const auto& a, const auto& b) {
				return a.second > b.second; // Compare by ranked score
				});

			// Generate the leaderboard using the sorted data
			int nr = 1;
			for (const auto& entry : totalScores) {
				std::string value = std::to_string(entry.second);
				value = FormatWithCommas(entry.second);
				std::replace(value.begin(), value.end(), ',', '.');
				ReplaceStringInPlace(value, ".000000", "");
				output += std::to_string(nr) + " >> " + entry.first + " - " + value + "\n";
				nr++; // Increment the counter
			}
		}

		return removeEmptyLineAtEnd(output);
	}
	if (input == "ss") {
		std::string temp;
		std::string output = "SS Rank Leaderboard:\n------------------------\n";

		for (size_t i = 0; i < id_url.size(); ++i) {
			auto g = cpr::Get(cpr::Url{"https://osu.ppy.sh/api/v2/users?" + id_url[i]},
				cpr::Header{
					{"Content-Type", "application/json"},
					{ "Accept", "application/json" },
				{ "Authorization", auth_token }});

			PostResult = json::parse(g.text);

			// Collect ranked scores into a vector of pairs (username, score)
			std::vector<std::pair<std::string, double>> totalScores;

			// Loop through each user
			for (const auto& user : PostResult["users"]) {
				// Extract username
				std::string username = user.value("username", "Error");

				buffer << user["statistics_rulesets"]["osu"]["grade_counts"]["ss"];
				temp = buffer.str();
				double ss = std::stod(temp.c_str());
				buffer.str("");
				buffer << user["statistics_rulesets"]["osu"]["grade_counts"]["ssh"];
				temp = buffer.str();
				double ssh = std::stod(temp.c_str());
				buffer.str("");
				double total_ss = ss + ssh;
				totalScores.emplace_back(username, total_ss);
			}

			// Sort the ranked scores in descending order
			std::sort(totalScores.begin(), totalScores.end(), [](const auto& a, const auto& b) {
				return a.second > b.second; // Compare by ranked score
				});

			// Generate the leaderboard using the sorted data
			int nr = 1;
			for (const auto& entry : totalScores) {
				std::string value = std::to_string(entry.second);
				value = FormatWithCommas(entry.second);
				std::replace(value.begin(), value.end(), ',', '.');
				ReplaceStringInPlace(value, ".000000", "");
				output += std::to_string(nr) + " >> " + entry.first + " - " + value + "\n";
				nr++; // Increment the counter
			}
		}

		return removeEmptyLineAtEnd(output);
	}
	if (input == "s") {
		std::string temp;
		std::string output = "S Rank Leaderboard:\n------------------------\n";

		for (size_t i = 0; i < id_url.size(); ++i) {
			auto g = cpr::Get(cpr::Url{"https://osu.ppy.sh/api/v2/users?" + id_url[i]},
				cpr::Header{
					{"Content-Type", "application/json"},
					{ "Accept", "application/json" },
				{ "Authorization", auth_token }});

			PostResult = json::parse(g.text);

			// Collect ranked scores into a vector of pairs (username, score)
			std::vector<std::pair<std::string, double>> totalScores;

			// Loop through each user
			for (const auto& user : PostResult["users"]) {
				// Extract username
				std::string username = user.value("username", "Error");

				buffer << user["statistics_rulesets"]["osu"]["grade_counts"]["s"];
				temp = buffer.str();
				double ss = std::stod(temp.c_str());
				buffer.str("");
				buffer << user["statistics_rulesets"]["osu"]["grade_counts"]["sh"];
				temp = buffer.str();
				double ssh = std::stod(temp.c_str());
				buffer.str("");
				double total_ss = ss + ssh;
				totalScores.emplace_back(username, total_ss);
			}

			// Sort the ranked scores in descending order
			std::sort(totalScores.begin(), totalScores.end(), [](const auto& a, const auto& b) {
				return a.second > b.second; // Compare by ranked score
				});

			// Generate the leaderboard using the sorted data
			int nr = 1;
			for (const auto& entry : totalScores) {
				std::string value = std::to_string(entry.second);
				value = FormatWithCommas(entry.second);
				std::replace(value.begin(), value.end(), ',', '.');
				ReplaceStringInPlace(value, ".000000", "");
				output += std::to_string(nr) + " >> " + entry.first + " - " + value + "\n";
				nr++; // Increment the counter
			}
		}

		return removeEmptyLineAtEnd(output);
	}
	if (input == "a") {
		std::string temp;
		std::string output = "A Rank Leaderboard:\n------------------------\n";

		for (size_t i = 0; i < id_url.size(); ++i) {
			auto g = cpr::Get(cpr::Url{"https://osu.ppy.sh/api/v2/users?" + id_url[i]},
				cpr::Header{
					{"Content-Type", "application/json"},
					{ "Accept", "application/json" },
				{ "Authorization", auth_token }});

			PostResult = json::parse(g.text);

			// Collect ranked scores into a vector of pairs (username, score)
			std::vector<std::pair<std::string, double>> totalScores;

			// Loop through each user
			for (const auto& user : PostResult["users"]) {
				// Extract username
				std::string username = user.value("username", "Error");

				buffer << user["statistics_rulesets"]["osu"]["grade_counts"]["a"];
				temp = buffer.str();
				double a = std::stod(temp.c_str());
				buffer.str("");
				totalScores.emplace_back(username, a);
			}

			// Sort the ranked scores in descending order
			std::sort(totalScores.begin(), totalScores.end(), [](const auto& a, const auto& b) {
				return a.second > b.second; // Compare by ranked score
				});

			// Generate the leaderboard using the sorted data
			int nr = 1;
			for (const auto& entry : totalScores) {
				std::string value = std::to_string(entry.second);
				value = FormatWithCommas(entry.second);
				std::replace(value.begin(), value.end(), ',', '.');
				ReplaceStringInPlace(value, ".000000", "");
				output += std::to_string(nr) + " >> " + entry.first + " - " + value + "\n";
				nr++; // Increment the counter
			}
		}

		return removeEmptyLineAtEnd(output);
	}
	if (input == "sr") {
		return "Command still in development.";

		std::string output = "Score Rank Leaderboard:\n------------------------\n";

		for (size_t i = 0; i < id_url.size(); ++i) {
			auto respektive = cpr::Get(cpr::Url{"https://score.respektive.pw/u/" + id_url[i]});
			PostResult = json::parse(respektive.text);

			// Collect ranked scores into a vector of pairs (username, ranked_score)
			std::vector<std::pair<std::string, int>> ScoreRank;

			// Loop through each user
			for (const auto& user : PostResult[0]) {
				console_log(1, user);
				buffer << user.value("rank", "Error");
				int score_rank = std::stoi(buffer.str());
				buffer.str("");

				buffer << user.value("username", "Error");
				std::string username = buffer.str();
				buffer.str("");

				ScoreRank.emplace_back(username, score_rank);
			}

			// Sort the ranked scores in descending order
			std::sort(ScoreRank.begin(), ScoreRank.end(), [](const auto& a, const auto& b) {
				return a.second < b.second; // Compare by ranked score
				});

			// Generate the leaderboard using the sorted data
			int nr = 1;
			for (const auto& entry : ScoreRank) {
				output += std::to_string(nr) + " >> " + entry.first + " - " + std::to_string(entry.second) + "\n";
				nr++; // Increment the counter
			}
		}

		return removeEmptyLineAtEnd(output);
	}
}

void dump_all(std::vector<std::string> id_url) {
	
	console_log(1, "Dumping All Linked Player Stats.");

	std::stringstream buffer;
	json PostResult;

	std::string rankedScore, totalScore, ss_Count, s_Count, a_Count;
	for (size_t i = 0; i < id_url.size(); ++i) {
		auto g = cpr::Get(cpr::Url{"https://osu.ppy.sh/api/v2/users?" + id_url[i]},
			cpr::Header{
				{"Content-Type", "application/json"},
				{ "Accept", "application/json" },
			{ "Authorization", auth_token }});

		console_log(1, "RESULT: " + std::to_string(g.status_code) + " " + g.status_line);

		PostResult = json::parse(g.text);
		console_log(1, "Looping....");
		// Loop through each user
		for (const auto& user : PostResult["users"]) {
			std::string userID = std::to_string(user.value("id", 0));
			console_log(1, userID);
			std::string temp;
			buffer << user["statistics_rulesets"]["osu"]["ranked_score"];
			temp = buffer.str();
			rankedScore += userID + "," + temp + "\n";
			buffer.str("");

			buffer << user["statistics_rulesets"]["osu"]["total_score"];
			temp = buffer.str();
			totalScore += userID + "," + temp + "\n";
			buffer.str("");

			buffer << user["statistics_rulesets"]["osu"]["grade_counts"]["ssh"];
			temp = buffer.str();
			double SS = std::stod(temp.c_str());
			buffer.str("");
			buffer << user["statistics_rulesets"]["osu"]["grade_counts"]["ss"];
			temp = buffer.str();
			SS = SS + std::stod(temp.c_str());
			buffer.str("");

			ss_Count += userID + "," + std::to_string(SS) + "\n";

			buffer << user["statistics_rulesets"]["osu"]["grade_counts"]["sh"];
			temp = buffer.str();
			double S = std::stod(temp.c_str());
			buffer.str("");
			buffer << user["statistics_rulesets"]["osu"]["grade_counts"]["s"];
			temp = buffer.str();
			S = S + std::stod(temp.c_str());
			buffer.str("");

			s_Count += userID + "," + std::to_string(S) + "\n";

			buffer << user["statistics_rulesets"]["osu"]["grade_counts"]["a"];
			temp = buffer.str();
			a_Count += userID + "," + temp + "\n";
			buffer.str("");
		}
	}

	std::ofstream outputFile("data/dump_daily_rankedscore.txt");
	if (outputFile.is_open()) {
		outputFile << rankedScore;
		outputFile.close();
		console_log(1, "Ranked Score Data has been dumped.");
	}
	else {
		console_log(2, "Couldnt open 'dump_daily_rankedscore.txt' for writing.");
	}

	std::ofstream outputFile2("data/dump_daily_totalscore.txt");
	if (outputFile2.is_open()) {
		outputFile2 << totalScore;
		outputFile2.close();
		console_log(1, "Total Score Data has been dumped.");
	}
	else {
		console_log(2, "Couldnt open 'dump_daily_totalscore.txt' for writing.");
	}

	std::ofstream outputFile3("data/dump_daily_ss_count.txt");
	if (outputFile3.is_open()) {
		outputFile3 << ss_Count;
		outputFile3.close();
		console_log(1, "SS Count Data has been dumped.");
	}
	else {
		console_log(2, "Couldnt open 'dump_daily_ss_count.txt' for writing.");
	}

	std::ofstream outputFile4("data/dump_daily_s_count.txt");
	if (outputFile4.is_open()) {
		outputFile4 << s_Count;
		outputFile4.close();
		console_log(1, "S Count Data has been dumped.");
	}
	else {
		console_log(2, "Couldnt open 'dump_daily_s_count.txt' for writing.");
	}

	std::ofstream outputFile5("data/dump_daily_a_count.txt");
	if (outputFile5.is_open()) {
		outputFile5 << a_Count;
		outputFile5.close();
		console_log(1, "A Count Data has been dumped.");
	}
	else {
		console_log(2, "Couldnt open 'dump_daily_a_count.txt' for writing.");
	}
}


// IMPLEMENTED
void dump_all_weekly(std::vector<std::string> id_url) { 

	console_log(1, "Dumping All Linked Player Stats.");

	std::stringstream buffer;
	json PostResult;

	std::string rankedScore, totalScore, ss_Count, s_Count, a_Count;
	for (size_t i = 0; i < id_url.size(); ++i) {
		auto g = cpr::Get(cpr::Url{ "https://osu.ppy.sh/api/v2/users?" + id_url[i] },
			cpr::Header{
				{"Content-Type", "application/json"},
				{ "Accept", "application/json" },
			{ "Authorization", auth_token } });

		console_log(1, "RESULT: " + std::to_string(g.status_code) + " " + g.status_line);

		PostResult = json::parse(g.text);
		console_log(1, "Looping....");
		// Loop through each user
		for (const auto& user : PostResult["users"]) {
			std::string userID = std::to_string(user.value("id", 0));
			console_log(1, userID);
			std::string temp;
			buffer << user["statistics_rulesets"]["osu"]["ranked_score"];
			temp = buffer.str();
			rankedScore += userID + "," + temp + "\n";
			buffer.str("");

			buffer << user["statistics_rulesets"]["osu"]["total_score"];
			temp = buffer.str();
			totalScore += userID + "," + temp + "\n";
			buffer.str("");

			buffer << user["statistics_rulesets"]["osu"]["grade_counts"]["ssh"];
			temp = buffer.str();
			double SS = std::stod(temp.c_str());
			buffer.str("");
			buffer << user["statistics_rulesets"]["osu"]["grade_counts"]["ss"];
			temp = buffer.str();
			SS = SS + std::stod(temp.c_str());
			buffer.str("");

			ss_Count += userID + "," + std::to_string(SS) + "\n";

			buffer << user["statistics_rulesets"]["osu"]["grade_counts"]["sh"];
			temp = buffer.str();
			double S = std::stod(temp.c_str());
			buffer.str("");
			buffer << user["statistics_rulesets"]["osu"]["grade_counts"]["s"];
			temp = buffer.str();
			S = S + std::stod(temp.c_str());
			buffer.str("");

			s_Count += userID + "," + std::to_string(S) + "\n";

			buffer << user["statistics_rulesets"]["osu"]["grade_counts"]["a"];
			temp = buffer.str();
			a_Count += userID + "," + temp + "\n";
			buffer.str("");
		}
	}

	std::ofstream outputFile("data/dump_weekly_rankedscore.txt");
	if (outputFile.is_open()) {
		outputFile << rankedScore;
		outputFile.close();
		console_log(1, "Ranked Score Data has been dumped.");
	}
	else {
		console_log(2, "Couldnt open 'dump_weekly_rankedscore.txt' for writing.");
	}

	std::ofstream outputFile2("data/dump_weekly_totalscore.txt");
	if (outputFile2.is_open()) {
		outputFile2 << totalScore;
		outputFile2.close();
		console_log(1, "Total Score Data has been dumped.");
	}
	else {
		console_log(2, "Couldnt open 'dump_weekly_totalscore.txt' for writing.");
	}

	std::ofstream outputFile3("data/dump_weekly_ss_count.txt");
	if (outputFile3.is_open()) {
		outputFile3 << ss_Count;
		outputFile3.close();
		console_log(1, "SS Count Data has been dumped.");
	}
	else {
		console_log(2, "Couldnt open 'dump_weekly_ss_count.txt' for writing.");
	}

	std::ofstream outputFile4("data/dump_weekly_s_count.txt");
	if (outputFile4.is_open()) {
		outputFile4 << s_Count;
		outputFile4.close();
		console_log(1, "S Count Data has been dumped.");
	}
	else {
		console_log(2, "Couldnt open 'dump_weekly_s_count.txt' for writing.");
	}

	std::ofstream outputFile5("data/dump_weekly_a_count.txt");
	if (outputFile5.is_open()) {
		outputFile5 << a_Count;
		outputFile5.close();
		console_log(1, "A Count Data has been dumped.");
	}
	else {
		console_log(2, "Couldnt open 'dump_weekly_a_count.txt' for writing.");
	}
} 


dpp::embed compare(std::vector<std::string> id_url, std::string dayTitle) {
	console_log(1, "# COMPARE FUNCTION #");
	std::vector<std::string> filenames = { "data/dump_daily_rankedscore.txt","data/dump_daily_totalscore.txt", "data/dump_daily_ss_count.txt", "data/dump_daily_s_count.txt", "data/dump_daily_a_count.txt" };
	// Collect ranked scores into a vector of pairs (userID, ranked_score)
	std::vector<std::pair<std::string, double>> rankedScores;
	std::vector<std::pair<std::string, double>> totalScores;
	std::vector<std::pair<std::string, double>> ss_Counts;
	std::vector<std::pair<std::string, double>> s_Counts;
	std::vector<std::pair<std::string, double>> a_Counts;

	//increasement
	std::vector<std::pair<std::string, double>> inc_rankedScores;
	std::vector<std::pair<std::string, double>> inc_totalScores;
	std::vector<std::pair<std::string, double>> inc_ss_Counts;
	std::vector<std::pair<std::string, double>> inc_s_Counts;
	std::vector<std::pair<std::string, double>> inc_a_Counts;

	std::stringstream buffer;
	json PostResult;

	std::vector<std::pair<std::string, double>> rankedScore_old, totalScore_old, ss_Count_old, s_Count_old, a_Count_old;

	double rankedScore_new, totalScore_new, ss_Count_new, s_Count_new, a_Count_new;

	// reading dumps
	for (const std::string& filename : filenames) {
		std::ifstream file(filename);
		if (!file.is_open()) {
			std::cerr << "Failed to open file: " << filename << std::endl;
			continue;
		}
		// init already dumped data
		std::string line;
		while (std::getline(file, line)) {
			if (containsSubstring(line, ",")) {
				if (filename == "data/dump_daily_rankedscore.txt") {
					rankedScore_old.emplace_back(keepBeforeComma(line), std::stod(cutAfterComma(line)));
				}
				if (filename == "data/dump_daily_totalscore.txt") {
					totalScore_old.emplace_back(keepBeforeComma(line), std::stod(cutAfterComma(line)));
				}
				if (filename == "data/dump_daily_ss_count.txt") {
					ss_Count_old.emplace_back(keepBeforeComma(line), std::stod(cutAfterComma(line)));
				}
				if (filename == "data/dump_daily_s_count.txt") {
					s_Count_old.emplace_back(keepBeforeComma(line), std::stod(cutAfterComma(line)));
				}
				if (filename == "data/dump_daily_a_count.txt") {
					a_Count_old.emplace_back(keepBeforeComma(line), std::stod(cutAfterComma(line)));
				}
			}
		}
		file.close();
	}

	for (size_t i = 0; i < id_url.size(); ++i) {
		auto g = cpr::Get(cpr::Url{"https://osu.ppy.sh/api/v2/users?" + id_url[i]},
			cpr::Header{
				{"Content-Type", "application/json"},
				{ "Accept", "application/json" },
			{ "Authorization", auth_token }});
		console_log(1, "RESULT: " + std::to_string(g.status_code) + " " + g.status_line);
		PostResult = json::parse(g.text);

		// Loop through each user and get their current data
		for (const auto& user : PostResult["users"]) {
			std::string userID = std::to_string(user.value("id", 0));
			std::string username = user.value("username", "Error");
			console_log(0, "LOOP: (" + userID + ") " + username);
			buffer << user["statistics_rulesets"]["osu"]["ranked_score"];
			rankedScore_new = std::stod(buffer.str());
			buffer.str("");
			rankedScores.emplace_back(username + "," + userID, rankedScore_new);

			buffer << user["statistics_rulesets"]["osu"]["total_score"];
			totalScore_new = std::stod(buffer.str());
			buffer.str("");
			totalScores.emplace_back(username + "," + userID, totalScore_new);

			buffer << user["statistics_rulesets"]["osu"]["grade_counts"]["ssh"];
			ss_Count_new = std::stod(buffer.str());
			buffer.str("");
			buffer << user["statistics_rulesets"]["osu"]["grade_counts"]["ss"];
			ss_Count_new = ss_Count_new + std::stod(buffer.str());
			buffer.str("");
			ss_Counts.emplace_back(username + "," + userID, ss_Count_new);

			buffer << user["statistics_rulesets"]["osu"]["grade_counts"]["sh"];
			s_Count_new = std::stod(buffer.str());
			buffer.str("");
			buffer << user["statistics_rulesets"]["osu"]["grade_counts"]["s"];
			s_Count_new = s_Count_new + std::stod(buffer.str());
			buffer.str("");
			s_Counts.emplace_back(username + "," + userID, s_Count_new);

			buffer << user["statistics_rulesets"]["osu"]["grade_counts"]["a"];
			a_Count_new = std::stod(buffer.str());
			buffer.str("");
			a_Counts.emplace_back(username + "," + userID, a_Count_new);
		}
	}
	console_log(1, "Sorting arrays...");
	// SORT THESE SHIT ARRAYS BECAUSE PEPPY SO STUPID RETURNING A SORTED RESPONSE BY USER_ID AND NOT BY MY ORDER HOW I SENT IT TO THE OSU! API

	std::sort(rankedScore_old.begin(), rankedScore_old.end(), [](const auto& a, const auto& b) {
		return std::stod(a.first) < std::stod(b.first);
		});

	std::sort(totalScore_old.begin(), totalScore_old.end(), [](const auto& a, const auto& b) {
		return std::stod(a.first) < std::stod(b.first);
		});

	std::sort(ss_Count_old.begin(), ss_Count_old.end(), [](const auto& a, const auto& b) {
		return std::stod(a.first) < std::stod(b.first);
		});

	std::sort(s_Count_old.begin(), s_Count_old.end(), [](const auto& a, const auto& b) {
		return std::stod(a.first) < std::stod(b.first);
		});

	std::sort(a_Count_old.begin(), a_Count_old.end(), [](const auto& a, const auto& b) {
		return std::stod(a.first) < std::stod(b.first);
		});
	console_log(1, "Sorting Done...");
	console_log(1, "Calculating difference.");
	// substract from past data to get the change value
	int loopIndex = 0;
	for (const auto& pair : rankedScores) {
		const std::string& key = pair.first;
		double value = pair.second;
		//
		std::pair<std::string, double> item = getDataAtIndex(rankedScore_old, loopIndex);
		if (containsSubstring(key, item.first)) {
			inc_rankedScores.emplace_back(ReplaceString(key, "," + item.first, ""), (value - item.second));
		}
		loopIndex++;
	}

	loopIndex = 0;
	for (const auto& pair : totalScores) {
		const std::string& key = pair.first;
		double value = pair.second;
		//
		std::pair<std::string, double> item = getDataAtIndex(totalScore_old, loopIndex);
		if (containsSubstring(key, item.first)) {
			inc_totalScores.emplace_back(ReplaceString(key, "," + item.first, ""), (value - item.second));
		}
		loopIndex++;
	}

	loopIndex = 0;
	for (const auto& pair : ss_Counts) {
		const std::string& key = pair.first;
		double value = pair.second;
		//
		std::pair<std::string, double> item = getDataAtIndex(ss_Count_old, loopIndex);
		if (containsSubstring(key, item.first)) {
			inc_ss_Counts.emplace_back(ReplaceString(key, "," + item.first, ""), (value - item.second));
		}
		loopIndex++;
	}

	loopIndex = 0;
	for (const auto& pair : s_Counts) {
		const std::string& key = pair.first;
		double value = pair.second;
		//
		std::pair<std::string, double> item = getDataAtIndex(s_Count_old, loopIndex);
		if (containsSubstring(key, item.first)) {
			inc_s_Counts.emplace_back(ReplaceString(key, "," + item.first, ""), (value - item.second));
		}
		loopIndex++;
	}

	loopIndex = 0;
	for (const auto& pair : a_Counts) {
		const std::string& key = pair.first;
		double value = pair.second;
		//
		std::pair<std::string, double> item = getDataAtIndex(a_Count_old, loopIndex);
		if (containsSubstring(key, item.first)) {
			inc_a_Counts.emplace_back(ReplaceString(key, "," + item.first, ""), (value - item.second));
		}
		loopIndex++;
	}
	console_log(1, "Sorting everything in descending order.");
	// Sort the ranked scores in descending order
	std::sort(inc_rankedScores.begin(), inc_rankedScores.end(), [](const auto& a, const auto& b) {
		return a.second > b.second; // Compare by ranked score
		});

	// Sort the total scores in descending order
	std::sort(inc_totalScores.begin(), inc_totalScores.end(), [](const auto& a, const auto& b) {
		return a.second > b.second; // Compare by total score
		});

	// Sort the SS scores in descending order
	std::sort(inc_ss_Counts.begin(), inc_ss_Counts.end(), [](const auto& a, const auto& b) {
		return a.second > b.second; // Compare by SS Ranks
		});

	// Sort the S scores in descending order
	std::sort(inc_s_Counts.begin(), inc_s_Counts.end(), [](const auto& a, const auto& b) {
		return a.second > b.second; // Compare by S Ranks
		});

	// Sort the A scores in descending order
	std::sort(inc_a_Counts.begin(), inc_a_Counts.end(), [](const auto& a, const auto& b) {
		return a.second > b.second; // Compare by A Ranks
		});

	std::string top_rankedScore, top_totalScore, top_ss, top_s, top_a;

	// Generate the leaderboard for ranked score
	console_log(1, "Creating Leaderboard.");
	int nr = 1;
	for (const auto& entry : inc_rankedScores) {
		if (nr > 3) {
			break; // Exit the loop after processing the top 3 entries
		}
		std::string value = std::to_string(entry.second);
		value = FormatWithCommas(entry.second);
		std::replace(value.begin(), value.end(), ',', '.');
		top_rankedScore += "#" + std::to_string(nr) + " **" + entry.first + "** with **" + ReplaceString(value, ".000000", "") + "** increase in Ranked Score.\n";
		nr++; // Increment the counter
	}

	// Generate the leaderboard for total score
	nr = 1;
	for (const auto& entry : inc_totalScores) {
		if (nr > 3) {
			break; // Exit the loop after processing the top 3 entries
		}
		std::string value = std::to_string(entry.second);
		value = FormatWithCommas(entry.second);
		std::replace(value.begin(), value.end(), ',', '.');
		top_totalScore += "#" + std::to_string(nr) + " **" + entry.first + "** with **" + ReplaceString(value, ".000000", "") + "** increase in Total Score.\n";
		nr++; // Increment the counter
	}

	// Generate the leaderboard for SS Ranks
	nr = 1;
	for (const auto& entry : inc_ss_Counts) {
		if (nr > 3) {
			break; // Exit the loop after processing the top 3 entries
		}
		std::string value = std::to_string(entry.second);
		value = FormatWithCommas(entry.second);
		std::replace(value.begin(), value.end(), ',', '.');
		top_ss += "#" + std::to_string(nr) + " **" + entry.first + "** with **" + ReplaceString(value, ".000000", "") + "** increase in SS ranks.\n";
		nr++; // Increment the counter
	}

	// Generate the leaderboard for S Ranks
	nr = 1;
	for (const auto& entry : inc_s_Counts) {
		if (nr > 3) {
			break; // Exit the loop after processing the top 3 entries
		}
		std::string value = std::to_string(entry.second);
		value = FormatWithCommas(entry.second);
		std::replace(value.begin(), value.end(), ',', '.');
		top_s += "#" + std::to_string(nr) + " **" + entry.first + "** with **" + ReplaceString(value, ".000000", "") + "** increase in S ranks.\n";
		nr++; // Increment the counter
	}

	// Generate the leaderboard for A Ranks
	nr = 1;
	for (const auto& entry : inc_a_Counts) {
		if (nr > 3) {
			break; // Exit the loop after processing the top 3 entries
		}
		std::string value = std::to_string(entry.second);
		value = FormatWithCommas(entry.second);
		std::replace(value.begin(), value.end(), ',', '.');
		top_a += "#" + std::to_string(nr) + " **" + entry.first + "** with **" + ReplaceString(value, ".000000", "") + "** increase in A ranks.\n";
		nr++; // Increment the counter
	}
	
	console_log(1, "Creating Embed.");
	// MAKE EMBED AND RETURN:
	dpp::embed embed = dpp::embed().
		set_color(dpp::colors::sti_blue).
		set_title(dayTitle).
		add_field(
			"**Ranked Score**",
			top_rankedScore
		).
		add_field(
			"**Total Score**",
			top_totalScore
		).
		add_field(
			"**SS Ranks**",
			top_ss
		).
		add_field(
			"**S Ranks**",
			top_s
		).
		add_field(
			"**A Ranks**",
			top_a
		).

		set_timestamp(time(0));
	PostResult.clear();
	buffer.clear();
	console_log(1, "Retuning Embed.");
	return embed;
}


// IMPLEMENTED
dpp::embed compare_weekly(std::vector<std::string> id_url, std::string dayTitle) {
	console_log(1, "# COMPARE FUNCTION #");
	std::vector<std::string> filenames = { "data/dump_weekly_rankedscore.txt","data/dump_weekly_totalscore.txt", "data/dump_weekly_ss_count.txt", "data/dump_weekly_s_count.txt", "data/dump_weekly_a_count.txt" };
	// Collect ranked scores into a vector of pairs (userID, ranked_score)
	std::vector<std::pair<std::string, double>> rankedScores;
	std::vector<std::pair<std::string, double>> totalScores;
	std::vector<std::pair<std::string, double>> ss_Counts;
	std::vector<std::pair<std::string, double>> s_Counts;
	std::vector<std::pair<std::string, double>> a_Counts;

	//increasement
	std::vector<std::pair<std::string, double>> inc_rankedScores;
	std::vector<std::pair<std::string, double>> inc_totalScores;
	std::vector<std::pair<std::string, double>> inc_ss_Counts;
	std::vector<std::pair<std::string, double>> inc_s_Counts;
	std::vector<std::pair<std::string, double>> inc_a_Counts;

	std::stringstream buffer;
	json PostResult;

	std::vector<std::pair<std::string, double>> rankedScore_old, totalScore_old, ss_Count_old, s_Count_old, a_Count_old;

	double rankedScore_new, totalScore_new, ss_Count_new, s_Count_new, a_Count_new;

	// reading dumps
	for (const std::string& filename : filenames) {
		std::ifstream file(filename);
		if (!file.is_open()) {
			std::cerr << "Failed to open file: " << filename << std::endl;
			continue;
		}
		// init already dumped data
		std::string line;
		while (std::getline(file, line)) {
			if (containsSubstring(line, ",")) {
				if (filename == "data/dump_weekly_rankedscore.txt") {
					rankedScore_old.emplace_back(keepBeforeComma(line), std::stod(cutAfterComma(line)));
				}
				if (filename == "data/dump_weekly_totalscore.txt") {
					totalScore_old.emplace_back(keepBeforeComma(line), std::stod(cutAfterComma(line)));
				}
				if (filename == "data/dump_weekly_ss_count.txt") {
					ss_Count_old.emplace_back(keepBeforeComma(line), std::stod(cutAfterComma(line)));
				}
				if (filename == "data/dump_weekly_s_count.txt") {
					s_Count_old.emplace_back(keepBeforeComma(line), std::stod(cutAfterComma(line)));
				}
				if (filename == "data/dump_weekly_a_count.txt") {
					a_Count_old.emplace_back(keepBeforeComma(line), std::stod(cutAfterComma(line)));
				}
			}
		}
		file.close();
	}

	for (size_t i = 0; i < id_url.size(); ++i) {
		auto g = cpr::Get(cpr::Url{ "https://osu.ppy.sh/api/v2/users?" + id_url[i] },
			cpr::Header{
				{"Content-Type", "application/json"},
				{ "Accept", "application/json" },
			{ "Authorization", auth_token } });
		console_log(1, "RESULT: " + std::to_string(g.status_code) + " " + g.status_line);
		PostResult = json::parse(g.text);

		// Loop through each user and get their current data
		for (const auto& user : PostResult["users"]) {
			std::string userID = std::to_string(user.value("id", 0));
			std::string username = user.value("username", "Error");
			console_log(0, "LOOP: (" + userID + ") " + username);
			buffer << user["statistics_rulesets"]["osu"]["ranked_score"];
			rankedScore_new = std::stod(buffer.str());
			buffer.str("");
			rankedScores.emplace_back(username + "," + userID, rankedScore_new);

			buffer << user["statistics_rulesets"]["osu"]["total_score"];
			totalScore_new = std::stod(buffer.str());
			buffer.str("");
			totalScores.emplace_back(username + "," + userID, totalScore_new);

			buffer << user["statistics_rulesets"]["osu"]["grade_counts"]["ssh"];
			ss_Count_new = std::stod(buffer.str());
			buffer.str("");
			buffer << user["statistics_rulesets"]["osu"]["grade_counts"]["ss"];
			ss_Count_new = ss_Count_new + std::stod(buffer.str());
			buffer.str("");
			ss_Counts.emplace_back(username + "," + userID, ss_Count_new);

			buffer << user["statistics_rulesets"]["osu"]["grade_counts"]["sh"];
			s_Count_new = std::stod(buffer.str());
			buffer.str("");
			buffer << user["statistics_rulesets"]["osu"]["grade_counts"]["s"];
			s_Count_new = s_Count_new + std::stod(buffer.str());
			buffer.str("");
			s_Counts.emplace_back(username + "," + userID, s_Count_new);

			buffer << user["statistics_rulesets"]["osu"]["grade_counts"]["a"];
			a_Count_new = std::stod(buffer.str());
			buffer.str("");
			a_Counts.emplace_back(username + "," + userID, a_Count_new);
		}
	}
	console_log(1, "Sorting arrays...");
	// SORT THESE SHIT ARRAYS BECAUSE PEPPY SO STUPID RETURNING A SORTED RESPONSE BY USER_ID AND NOT BY MY ORDER HOW I SENT IT TO THE OSU! API

	std::sort(rankedScore_old.begin(), rankedScore_old.end(), [](const auto& a, const auto& b) {
		return std::stod(a.first) < std::stod(b.first);
		});

	std::sort(totalScore_old.begin(), totalScore_old.end(), [](const auto& a, const auto& b) {
		return std::stod(a.first) < std::stod(b.first);
		});

	std::sort(ss_Count_old.begin(), ss_Count_old.end(), [](const auto& a, const auto& b) {
		return std::stod(a.first) < std::stod(b.first);
		});

	std::sort(s_Count_old.begin(), s_Count_old.end(), [](const auto& a, const auto& b) {
		return std::stod(a.first) < std::stod(b.first);
		});

	std::sort(a_Count_old.begin(), a_Count_old.end(), [](const auto& a, const auto& b) {
		return std::stod(a.first) < std::stod(b.first);
		});
	console_log(1, "Sorting Done...");
	console_log(1, "Calculating difference.");
	// substract from past data to get the change value
	int loopIndex = 0;
	for (const auto& pair : rankedScores) {
		const std::string& key = pair.first;
		double value = pair.second;
		//
		std::pair<std::string, double> item = getDataAtIndex(rankedScore_old, loopIndex);
		if (containsSubstring(key, item.first)) {
			inc_rankedScores.emplace_back(ReplaceString(key, "," + item.first, ""), (value - item.second));
		}
		loopIndex++;
	}

	loopIndex = 0;
	for (const auto& pair : totalScores) {
		const std::string& key = pair.first;
		double value = pair.second;
		//
		std::pair<std::string, double> item = getDataAtIndex(totalScore_old, loopIndex);
		if (containsSubstring(key, item.first)) {
			inc_totalScores.emplace_back(ReplaceString(key, "," + item.first, ""), (value - item.second));
		}
		loopIndex++;
	}

	loopIndex = 0;
	for (const auto& pair : ss_Counts) {
		const std::string& key = pair.first;
		double value = pair.second;
		//
		std::pair<std::string, double> item = getDataAtIndex(ss_Count_old, loopIndex);
		if (containsSubstring(key, item.first)) {
			inc_ss_Counts.emplace_back(ReplaceString(key, "," + item.first, ""), (value - item.second));
		}
		loopIndex++;
	}

	loopIndex = 0;
	for (const auto& pair : s_Counts) {
		const std::string& key = pair.first;
		double value = pair.second;
		//
		std::pair<std::string, double> item = getDataAtIndex(s_Count_old, loopIndex);
		if (containsSubstring(key, item.first)) {
			inc_s_Counts.emplace_back(ReplaceString(key, "," + item.first, ""), (value - item.second));
		}
		loopIndex++;
	}

	loopIndex = 0;
	for (const auto& pair : a_Counts) {
		const std::string& key = pair.first;
		double value = pair.second;
		//
		std::pair<std::string, double> item = getDataAtIndex(a_Count_old, loopIndex);
		if (containsSubstring(key, item.first)) {
			inc_a_Counts.emplace_back(ReplaceString(key, "," + item.first, ""), (value - item.second));
		}
		loopIndex++;
	}
	console_log(1, "Sorting everything in descending order.");
	// Sort the ranked scores in descending order
	std::sort(inc_rankedScores.begin(), inc_rankedScores.end(), [](const auto& a, const auto& b) {
		return a.second > b.second; // Compare by ranked score
		});

	// Sort the total scores in descending order
	std::sort(inc_totalScores.begin(), inc_totalScores.end(), [](const auto& a, const auto& b) {
		return a.second > b.second; // Compare by total score
		});

	// Sort the SS scores in descending order
	std::sort(inc_ss_Counts.begin(), inc_ss_Counts.end(), [](const auto& a, const auto& b) {
		return a.second > b.second; // Compare by SS Ranks
		});

	// Sort the S scores in descending order
	std::sort(inc_s_Counts.begin(), inc_s_Counts.end(), [](const auto& a, const auto& b) {
		return a.second > b.second; // Compare by S Ranks
		});

	// Sort the A scores in descending order
	std::sort(inc_a_Counts.begin(), inc_a_Counts.end(), [](const auto& a, const auto& b) {
		return a.second > b.second; // Compare by A Ranks
		});

	std::string top_rankedScore, top_totalScore, top_ss, top_s, top_a;

	// Generate the leaderboard for ranked score
	console_log(1, "Creating Leaderboard.");
	int nr = 1;
	for (const auto& entry : inc_rankedScores) {
		if (nr > 3) {
			break; // Exit the loop after processing the top 3 entries
		}
		std::string value = std::to_string(entry.second);
		value = FormatWithCommas(entry.second);
		std::replace(value.begin(), value.end(), ',', '.');
		top_rankedScore += "#" + std::to_string(nr) + " **" + entry.first + "** with **" + ReplaceString(value, ".000000", "") + "** increase in Ranked Score.\n";
		nr++; // Increment the counter
	}

	// Generate the leaderboard for total score
	nr = 1;
	for (const auto& entry : inc_totalScores) {
		if (nr > 3) {
			break; // Exit the loop after processing the top 3 entries
		}
		std::string value = std::to_string(entry.second);
		value = FormatWithCommas(entry.second);
		std::replace(value.begin(), value.end(), ',', '.');
		top_totalScore += "#" + std::to_string(nr) + " **" + entry.first + "** with **" + ReplaceString(value, ".000000", "") + "** increase in Total Score.\n";
		nr++; // Increment the counter
	}

	// Generate the leaderboard for SS Ranks
	nr = 1;
	for (const auto& entry : inc_ss_Counts) {
		if (nr > 3) {
			break; // Exit the loop after processing the top 3 entries
		}
		std::string value = std::to_string(entry.second);
		value = FormatWithCommas(entry.second);
		std::replace(value.begin(), value.end(), ',', '.');
		top_ss += "#" + std::to_string(nr) + " **" + entry.first + "** with **" + ReplaceString(value, ".000000", "") + "** increase in SS ranks.\n";
		nr++; // Increment the counter
	}

	// Generate the leaderboard for S Ranks
	nr = 1;
	for (const auto& entry : inc_s_Counts) {
		if (nr > 3) {
			break; // Exit the loop after processing the top 3 entries
		}
		std::string value = std::to_string(entry.second);
		value = FormatWithCommas(entry.second);
		std::replace(value.begin(), value.end(), ',', '.');
		top_s += "#" + std::to_string(nr) + " **" + entry.first + "** with **" + ReplaceString(value, ".000000", "") + "** increase in S ranks.\n";
		nr++; // Increment the counter
	}

	// Generate the leaderboard for A Ranks
	nr = 1;
	for (const auto& entry : inc_a_Counts) {
		if (nr > 3) {
			break; // Exit the loop after processing the top 3 entries
		}
		std::string value = std::to_string(entry.second);
		value = FormatWithCommas(entry.second);
		std::replace(value.begin(), value.end(), ',', '.');
		top_a += "#" + std::to_string(nr) + " **" + entry.first + "** with **" + ReplaceString(value, ".000000", "") + "** increase in A ranks.\n";
		nr++; // Increment the counter
	}

	console_log(1, "Creating Embed.");
	// MAKE EMBED AND RETURN:
	dpp::embed embed = dpp::embed().
		set_color(dpp::colors::sti_blue).
		set_title(dayTitle).
		add_field(
			"**Ranked Score**",
			top_rankedScore
		).
		add_field(
			"**Total Score**",
			top_totalScore
		).
		add_field(
			"**SS Ranks**",
			top_ss
		).
		add_field(
			"**S Ranks**",
			top_s
		).
		add_field(
			"**A Ranks**",
			top_a
		).

		set_timestamp(time(0));
	PostResult.clear();
	buffer.clear();
	console_log(1, "Retuning Embed.");
	return embed;
}


std::string compare_all(std::vector<std::string> id_url, std::string dayTitle) {
	std::vector<std::string> filenames = { "data/dump_daily_rankedscore.txt","data/dump_daily_totalscore.txt", "data/dump_daily_ss_count.txt", "data/dump_daily_s_count.txt", "data/dump_daily_a_count.txt" };
	// Collect ranked scores into a vector of pairs (userID, ranked_score)
	std::vector<std::pair<std::string, double>> rankedScores;
	std::vector<std::pair<std::string, double>> totalScores;
	std::vector<std::pair<std::string, double>> ss_Counts;
	std::vector<std::pair<std::string, double>> s_Counts;
	std::vector<std::pair<std::string, double>> a_Counts;

	//increasement
	std::vector<std::pair<std::string, double>> inc_rankedScores;
	std::vector<std::pair<std::string, double>> inc_totalScores;
	std::vector<std::pair<std::string, double>> inc_ss_Counts;
	std::vector<std::pair<std::string, double>> inc_s_Counts;
	std::vector<std::pair<std::string, double>> inc_a_Counts;

	std::stringstream buffer;
	json PostResult;

	std::vector<std::pair<std::string, double>> rankedScore_old, totalScore_old, ss_Count_old, s_Count_old, a_Count_old;

	double rankedScore_new, totalScore_new, ss_Count_new, s_Count_new, a_Count_new;

	// reading dumps
	for (const std::string& filename : filenames) {
		std::ifstream file(filename);
		if (!file.is_open()) {
			std::cerr << "Failed to open file: " << filename << std::endl;
			continue;
		}
		// init already dumped data
		std::string line;
		while (std::getline(file, line)) {
			if (containsSubstring(line, ",")) {
				if (filename == "data/dump_daily_rankedscore.txt") {
					rankedScore_old.emplace_back(keepBeforeComma(line), std::stod(cutAfterComma(line)));
				}
				if (filename == "data/dump_daily_totalscore.txt") {
					totalScore_old.emplace_back(keepBeforeComma(line), std::stod(cutAfterComma(line)));
				}
				if (filename == "data/dump_daily_ss_count.txt") {
					ss_Count_old.emplace_back(keepBeforeComma(line), std::stod(cutAfterComma(line)));
				}
				if (filename == "data/dump_daily_s_count.txt") {
					s_Count_old.emplace_back(keepBeforeComma(line), std::stod(cutAfterComma(line)));
				}
				if (filename == "data/dump_daily_a_count.txt") {
					a_Count_old.emplace_back(keepBeforeComma(line), std::stod(cutAfterComma(line)));
				}
			}
		}
		file.close();
	}

	for (size_t i = 0; i < id_url.size(); ++i) {
		auto g = cpr::Get(cpr::Url{"https://osu.ppy.sh/api/v2/users?" + id_url[i]},
			cpr::Header{
				{"Content-Type", "application/json"},
				{ "Accept", "application/json" },
			{ "Authorization", auth_token }});

		PostResult = json::parse(g.text);

		// Loop through each user and get their current data
		for (const auto& user : PostResult["users"]) {
			std::string userID = std::to_string(user.value("id", 0));
			std::string username = user.value("username", "Error");

			buffer << user["statistics_rulesets"]["osu"]["ranked_score"];
			rankedScore_new = std::stod(buffer.str());
			buffer.str("");
			rankedScores.emplace_back(username + "," + userID, rankedScore_new);

			buffer << user["statistics_rulesets"]["osu"]["total_score"];
			totalScore_new = std::stod(buffer.str());
			buffer.str("");
			totalScores.emplace_back(username + "," + userID, totalScore_new);

			buffer << user["statistics_rulesets"]["osu"]["grade_counts"]["ssh"];
			ss_Count_new = std::stod(buffer.str());
			buffer.str("");
			buffer << user["statistics_rulesets"]["osu"]["grade_counts"]["ss"];
			ss_Count_new = ss_Count_new + std::stod(buffer.str());
			buffer.str("");
			ss_Counts.emplace_back(username + "," + userID, ss_Count_new);

			buffer << user["statistics_rulesets"]["osu"]["grade_counts"]["sh"];
			s_Count_new = std::stod(buffer.str());
			buffer.str("");
			buffer << user["statistics_rulesets"]["osu"]["grade_counts"]["s"];
			s_Count_new = s_Count_new + std::stod(buffer.str());
			buffer.str("");
			s_Counts.emplace_back(username + "," + userID, s_Count_new);

			buffer << user["statistics_rulesets"]["osu"]["grade_counts"]["a"];
			a_Count_new = std::stod(buffer.str());
			buffer.str("");
			a_Counts.emplace_back(username + "," + userID, a_Count_new);
		}
	}

	// SORT THESE SHIT ARRAYS BECAUSE PEPPY SO STUPID RETURNING A SORTED RESPONSE BY USER_ID AND NOT BY MY ORDER HOW I SENT IT TO THE OSU! API

	std::sort(rankedScore_old.begin(), rankedScore_old.end(), [](const auto& a, const auto& b) {
		return std::stod(a.first) < std::stod(b.first);
		});

	std::sort(totalScore_old.begin(), totalScore_old.end(), [](const auto& a, const auto& b) {
		return std::stod(a.first) < std::stod(b.first);
		});

	std::sort(ss_Count_old.begin(), ss_Count_old.end(), [](const auto& a, const auto& b) {
		return std::stod(a.first) < std::stod(b.first);
		});

	std::sort(s_Count_old.begin(), s_Count_old.end(), [](const auto& a, const auto& b) {
		return std::stod(a.first) < std::stod(b.first);
		});

	std::sort(a_Count_old.begin(), a_Count_old.end(), [](const auto& a, const auto& b) {
		return std::stod(a.first) < std::stod(b.first);
		});

	// substract from past data to get the change value
	int loopIndex = 0;
	for (const auto& pair : rankedScores) {
		const std::string& key = pair.first;
		double value = pair.second;
		//
		std::pair<std::string, double> item = getDataAtIndex(rankedScore_old, loopIndex);
		if (containsSubstring(key, item.first)) {
			inc_rankedScores.emplace_back(ReplaceString(key, "," + item.first, ""), (value - item.second));
		}
		loopIndex++;
	}

	loopIndex = 0;
	for (const auto& pair : totalScores) {
		const std::string& key = pair.first;
		double value = pair.second;
		//
		std::pair<std::string, double> item = getDataAtIndex(totalScore_old, loopIndex);
		if (containsSubstring(key, item.first)) {
			inc_totalScores.emplace_back(ReplaceString(key, "," + item.first, ""), (value - item.second));
		}
		loopIndex++;
	}

	loopIndex = 0;
	for (const auto& pair : ss_Counts) {
		const std::string& key = pair.first;
		double value = pair.second;
		//
		std::pair<std::string, double> item = getDataAtIndex(ss_Count_old, loopIndex);
		if (containsSubstring(key, item.first)) {
			inc_ss_Counts.emplace_back(ReplaceString(key, "," + item.first, ""), (value - item.second));
		}
		loopIndex++;
	}

	loopIndex = 0;
	for (const auto& pair : s_Counts) {
		const std::string& key = pair.first;
		double value = pair.second;
		//
		std::pair<std::string, double> item = getDataAtIndex(s_Count_old, loopIndex);
		if (containsSubstring(key, item.first)) {
			inc_s_Counts.emplace_back(ReplaceString(key, "," + item.first, ""), (value - item.second));
		}
		loopIndex++;
	}

	loopIndex = 0;
	for (const auto& pair : a_Counts) {
		const std::string& key = pair.first;
		double value = pair.second;
		//
		std::pair<std::string, double> item = getDataAtIndex(a_Count_old, loopIndex);
		if (containsSubstring(key, item.first)) {
			inc_a_Counts.emplace_back(ReplaceString(key, "," + item.first, ""), (value - item.second));
		}
		loopIndex++;
	}

	// Sort the ranked scores in descending order
	std::sort(inc_rankedScores.begin(), inc_rankedScores.end(), [](const auto& a, const auto& b) {
		return a.second > b.second; // Compare by ranked score
		});

	// Sort the total scores in descending order
	std::sort(inc_totalScores.begin(), inc_totalScores.end(), [](const auto& a, const auto& b) {
		return a.second > b.second; // Compare by total score
		});

	// Sort the SS scores in descending order
	std::sort(inc_ss_Counts.begin(), inc_ss_Counts.end(), [](const auto& a, const auto& b) {
		return a.second > b.second; // Compare by SS Ranks
		});

	// Sort the S scores in descending order
	std::sort(inc_s_Counts.begin(), inc_s_Counts.end(), [](const auto& a, const auto& b) {
		return a.second > b.second; // Compare by S Ranks
		});

	// Sort the A scores in descending order
	std::sort(inc_a_Counts.begin(), inc_a_Counts.end(), [](const auto& a, const auto& b) {
		return a.second > b.second; // Compare by A Ranks
		});

	std::string top_rankedScore, top_totalScore, top_ss, top_s, top_a;

	// Generate the leaderboard for ranked score
	int nr = 1;
	for (const auto& entry : inc_rankedScores) {
		std::string value = std::to_string(entry.second);
		value = FormatWithCommas(entry.second);
		std::replace(value.begin(), value.end(), ',', '.');
		top_rankedScore += "#" + std::to_string(nr) + " " + entry.first + " with " + ReplaceString(value, ".000000", "") + " increase in Ranked Score.\n";
		nr++; // Increment the counter
	}

	// Generate the leaderboard for total score
	nr = 1;
	for (const auto& entry : inc_totalScores) {
		std::string value = std::to_string(entry.second);
		value = FormatWithCommas(entry.second);
		std::replace(value.begin(), value.end(), ',', '.');
		top_totalScore += "#" + std::to_string(nr) + " " + entry.first + " with " + ReplaceString(value, ".000000", "") + " increase in Total Score.\n";
		nr++; // Increment the counter
	}

	// Generate the leaderboard for SS Ranks
	nr = 1;
	for (const auto& entry : inc_ss_Counts) {
		std::string value = std::to_string(entry.second);
		value = FormatWithCommas(entry.second);
		std::replace(value.begin(), value.end(), ',', '.');
		top_ss += "#" + std::to_string(nr) + " " + entry.first + " with " + ReplaceString(value, ".000000", "") + " increase in SS ranks.\n";
		nr++; // Increment the counter
	}

	// Generate the leaderboard for S Ranks
	nr = 1;
	for (const auto& entry : inc_s_Counts) {
		std::string value = std::to_string(entry.second);
		value = FormatWithCommas(entry.second);
		std::replace(value.begin(), value.end(), ',', '.');
		top_s += "#" + std::to_string(nr) + " " + entry.first + " with " + ReplaceString(value, ".000000", "") + " increase in S ranks.\n";
		nr++; // Increment the counter
	}

	// Generate the leaderboard for A Ranks
	nr = 1;
	for (const auto& entry : inc_a_Counts) {
		std::string value = std::to_string(entry.second);
		value = FormatWithCommas(entry.second);
		std::replace(value.begin(), value.end(), ',', '.');
		top_a += "#" + std::to_string(nr) + " " + entry.first + " with " + ReplaceString(value, ".000000", "") + " increase in A ranks.\n";
		nr++; // Increment the counter
	}

	std::string dump = dayTitle +
		"\n------------------\nRANKED SCORE:\n" +
		removeEmptyLineAtEnd(top_rankedScore) +
		"\n------------------\nTOTAL SCORE:\n" +
		removeEmptyLineAtEnd(top_totalScore) +
		"\n------------------\nSS COUNT:\n" +
		removeEmptyLineAtEnd(top_ss) +
		"\n------------------\nS COUNT:\n" +
		removeEmptyLineAtEnd(top_s) +
		"\n------------------\nA COUNT:\n" +
		removeEmptyLineAtEnd(top_a);

	PostResult.clear();
	buffer.clear();

	return dump;
}


// IMPLEMENTED
std::string compare_all_weekly(std::vector<std::string> id_url, std::string dayTitle) {
	std::vector<std::string> filenames = { "data/dump_weekly_rankedscore.txt","data/dump_weekly_totalscore.txt", "data/dump_weekly_ss_count.txt", "data/dump_weekly_s_count.txt", "data/dump_weekly_a_count.txt" };
	// Collect ranked scores into a vector of pairs (userID, ranked_score)
	std::vector<std::pair<std::string, double>> rankedScores;
	std::vector<std::pair<std::string, double>> totalScores;
	std::vector<std::pair<std::string, double>> ss_Counts;
	std::vector<std::pair<std::string, double>> s_Counts;
	std::vector<std::pair<std::string, double>> a_Counts;

	//increasement
	std::vector<std::pair<std::string, double>> inc_rankedScores;
	std::vector<std::pair<std::string, double>> inc_totalScores;
	std::vector<std::pair<std::string, double>> inc_ss_Counts;
	std::vector<std::pair<std::string, double>> inc_s_Counts;
	std::vector<std::pair<std::string, double>> inc_a_Counts;

	std::stringstream buffer;
	json PostResult;

	std::vector<std::pair<std::string, double>> rankedScore_old, totalScore_old, ss_Count_old, s_Count_old, a_Count_old;

	double rankedScore_new, totalScore_new, ss_Count_new, s_Count_new, a_Count_new;

	// reading dumps
	for (const std::string& filename : filenames) {
		std::ifstream file(filename);
		if (!file.is_open()) {
			std::cerr << "Failed to open file: " << filename << std::endl;
			continue;
		}
		// init already dumped data
		std::string line;
		while (std::getline(file, line)) {
			if (containsSubstring(line, ",")) {
				if (filename == "data/dump_weekly_rankedscore.txt") {
					rankedScore_old.emplace_back(keepBeforeComma(line), std::stod(cutAfterComma(line)));
				}
				if (filename == "data/dump_weekly_totalscore.txt") {
					totalScore_old.emplace_back(keepBeforeComma(line), std::stod(cutAfterComma(line)));
				}
				if (filename == "data/dump_weekly_ss_count.txt") {
					ss_Count_old.emplace_back(keepBeforeComma(line), std::stod(cutAfterComma(line)));
				}
				if (filename == "data/dump_weekly_s_count.txt") {
					s_Count_old.emplace_back(keepBeforeComma(line), std::stod(cutAfterComma(line)));
				}
				if (filename == "data/dump_weekly_a_count.txt") {
					a_Count_old.emplace_back(keepBeforeComma(line), std::stod(cutAfterComma(line)));
				}
			}
		}
		file.close();
	}

	for (size_t i = 0; i < id_url.size(); ++i) {
		auto g = cpr::Get(cpr::Url{ "https://osu.ppy.sh/api/v2/users?" + id_url[i] },
			cpr::Header{
				{"Content-Type", "application/json"},
				{ "Accept", "application/json" },
			{ "Authorization", auth_token } });

		PostResult = json::parse(g.text);

		// Loop through each user and get their current data
		for (const auto& user : PostResult["users"]) {
			std::string userID = std::to_string(user.value("id", 0));
			std::string username = user.value("username", "Error");

			buffer << user["statistics_rulesets"]["osu"]["ranked_score"];
			rankedScore_new = std::stod(buffer.str());
			buffer.str("");
			rankedScores.emplace_back(username + "," + userID, rankedScore_new);

			buffer << user["statistics_rulesets"]["osu"]["total_score"];
			totalScore_new = std::stod(buffer.str());
			buffer.str("");
			totalScores.emplace_back(username + "," + userID, totalScore_new);

			buffer << user["statistics_rulesets"]["osu"]["grade_counts"]["ssh"];
			ss_Count_new = std::stod(buffer.str());
			buffer.str("");
			buffer << user["statistics_rulesets"]["osu"]["grade_counts"]["ss"];
			ss_Count_new = ss_Count_new + std::stod(buffer.str());
			buffer.str("");
			ss_Counts.emplace_back(username + "," + userID, ss_Count_new);

			buffer << user["statistics_rulesets"]["osu"]["grade_counts"]["sh"];
			s_Count_new = std::stod(buffer.str());
			buffer.str("");
			buffer << user["statistics_rulesets"]["osu"]["grade_counts"]["s"];
			s_Count_new = s_Count_new + std::stod(buffer.str());
			buffer.str("");
			s_Counts.emplace_back(username + "," + userID, s_Count_new);

			buffer << user["statistics_rulesets"]["osu"]["grade_counts"]["a"];
			a_Count_new = std::stod(buffer.str());
			buffer.str("");
			a_Counts.emplace_back(username + "," + userID, a_Count_new);
		}
	}

	// SORT THESE SHIT ARRAYS BECAUSE PEPPY SO STUPID RETURNING A SORTED RESPONSE BY USER_ID AND NOT BY MY ORDER HOW I SENT IT TO THE OSU! API

	std::sort(rankedScore_old.begin(), rankedScore_old.end(), [](const auto& a, const auto& b) {
		return std::stod(a.first) < std::stod(b.first);
		});

	std::sort(totalScore_old.begin(), totalScore_old.end(), [](const auto& a, const auto& b) {
		return std::stod(a.first) < std::stod(b.first);
		});

	std::sort(ss_Count_old.begin(), ss_Count_old.end(), [](const auto& a, const auto& b) {
		return std::stod(a.first) < std::stod(b.first);
		});

	std::sort(s_Count_old.begin(), s_Count_old.end(), [](const auto& a, const auto& b) {
		return std::stod(a.first) < std::stod(b.first);
		});

	std::sort(a_Count_old.begin(), a_Count_old.end(), [](const auto& a, const auto& b) {
		return std::stod(a.first) < std::stod(b.first);
		});

	// substract from past data to get the change value
	int loopIndex = 0;
	for (const auto& pair : rankedScores) {
		const std::string& key = pair.first;
		double value = pair.second;
		//
		std::pair<std::string, double> item = getDataAtIndex(rankedScore_old, loopIndex);
		if (containsSubstring(key, item.first)) {
			inc_rankedScores.emplace_back(ReplaceString(key, "," + item.first, ""), (value - item.second));
		}
		loopIndex++;
	}

	loopIndex = 0;
	for (const auto& pair : totalScores) {
		const std::string& key = pair.first;
		double value = pair.second;
		//
		std::pair<std::string, double> item = getDataAtIndex(totalScore_old, loopIndex);
		if (containsSubstring(key, item.first)) {
			inc_totalScores.emplace_back(ReplaceString(key, "," + item.first, ""), (value - item.second));
		}
		loopIndex++;
	}

	loopIndex = 0;
	for (const auto& pair : ss_Counts) {
		const std::string& key = pair.first;
		double value = pair.second;
		//
		std::pair<std::string, double> item = getDataAtIndex(ss_Count_old, loopIndex);
		if (containsSubstring(key, item.first)) {
			inc_ss_Counts.emplace_back(ReplaceString(key, "," + item.first, ""), (value - item.second));
		}
		loopIndex++;
	}

	loopIndex = 0;
	for (const auto& pair : s_Counts) {
		const std::string& key = pair.first;
		double value = pair.second;
		//
		std::pair<std::string, double> item = getDataAtIndex(s_Count_old, loopIndex);
		if (containsSubstring(key, item.first)) {
			inc_s_Counts.emplace_back(ReplaceString(key, "," + item.first, ""), (value - item.second));
		}
		loopIndex++;
	}

	loopIndex = 0;
	for (const auto& pair : a_Counts) {
		const std::string& key = pair.first;
		double value = pair.second;
		//
		std::pair<std::string, double> item = getDataAtIndex(a_Count_old, loopIndex);
		if (containsSubstring(key, item.first)) {
			inc_a_Counts.emplace_back(ReplaceString(key, "," + item.first, ""), (value - item.second));
		}
		loopIndex++;
	}

	// Sort the ranked scores in descending order
	std::sort(inc_rankedScores.begin(), inc_rankedScores.end(), [](const auto& a, const auto& b) {
		return a.second > b.second; // Compare by ranked score
		});

	// Sort the total scores in descending order
	std::sort(inc_totalScores.begin(), inc_totalScores.end(), [](const auto& a, const auto& b) {
		return a.second > b.second; // Compare by total score
		});

	// Sort the SS scores in descending order
	std::sort(inc_ss_Counts.begin(), inc_ss_Counts.end(), [](const auto& a, const auto& b) {
		return a.second > b.second; // Compare by SS Ranks
		});

	// Sort the S scores in descending order
	std::sort(inc_s_Counts.begin(), inc_s_Counts.end(), [](const auto& a, const auto& b) {
		return a.second > b.second; // Compare by S Ranks
		});

	// Sort the A scores in descending order
	std::sort(inc_a_Counts.begin(), inc_a_Counts.end(), [](const auto& a, const auto& b) {
		return a.second > b.second; // Compare by A Ranks
		});

	std::string top_rankedScore, top_totalScore, top_ss, top_s, top_a;

	// Generate the leaderboard for ranked score
	int nr = 1;
	for (const auto& entry : inc_rankedScores) {
		std::string value = std::to_string(entry.second);
		value = FormatWithCommas(entry.second);
		std::replace(value.begin(), value.end(), ',', '.');
		top_rankedScore += "#" + std::to_string(nr) + " " + entry.first + " with " + ReplaceString(value, ".000000", "") + " increase in Ranked Score.\n";
		nr++; // Increment the counter
	}

	// Generate the leaderboard for total score
	nr = 1;
	for (const auto& entry : inc_totalScores) {
		std::string value = std::to_string(entry.second);
		value = FormatWithCommas(entry.second);
		std::replace(value.begin(), value.end(), ',', '.');
		top_totalScore += "#" + std::to_string(nr) + " " + entry.first + " with " + ReplaceString(value, ".000000", "") + " increase in Total Score.\n";
		nr++; // Increment the counter
	}

	// Generate the leaderboard for SS Ranks
	nr = 1;
	for (const auto& entry : inc_ss_Counts) {
		std::string value = std::to_string(entry.second);
		value = FormatWithCommas(entry.second);
		std::replace(value.begin(), value.end(), ',', '.');
		top_ss += "#" + std::to_string(nr) + " " + entry.first + " with " + ReplaceString(value, ".000000", "") + " increase in SS ranks.\n";
		nr++; // Increment the counter
	}

	// Generate the leaderboard for S Ranks
	nr = 1;
	for (const auto& entry : inc_s_Counts) {
		std::string value = std::to_string(entry.second);
		value = FormatWithCommas(entry.second);
		std::replace(value.begin(), value.end(), ',', '.');
		top_s += "#" + std::to_string(nr) + " " + entry.first + " with " + ReplaceString(value, ".000000", "") + " increase in S ranks.\n";
		nr++; // Increment the counter
	}

	// Generate the leaderboard for A Ranks
	nr = 1;
	for (const auto& entry : inc_a_Counts) {
		std::string value = std::to_string(entry.second);
		value = FormatWithCommas(entry.second);
		std::replace(value.begin(), value.end(), ',', '.');
		top_a += "#" + std::to_string(nr) + " " + entry.first + " with " + ReplaceString(value, ".000000", "") + " increase in A ranks.\n";
		nr++; // Increment the counter
	}

	std::string dump = dayTitle +
		"\n------------------\nRANKED SCORE:\n" +
		removeEmptyLineAtEnd(top_rankedScore) +
		"\n------------------\nTOTAL SCORE:\n" +
		removeEmptyLineAtEnd(top_totalScore) +
		"\n------------------\nSS COUNT:\n" +
		removeEmptyLineAtEnd(top_ss) +
		"\n------------------\nS COUNT:\n" +
		removeEmptyLineAtEnd(top_s) +
		"\n------------------\nA COUNT:\n" +
		removeEmptyLineAtEnd(top_a);

	PostResult.clear();
	buffer.clear();

	return dump;
}
