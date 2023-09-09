#pragma once

#include <curl/curl.h>

void ReplaceStringInPlace(std::string& subject, const std::string& search, const std::string& replace) {
	size_t pos = 0;
	while ((pos = subject.find(search, pos)) != std::string::npos) {
		subject.replace(pos, search.length(), replace);
		pos += replace.length();
	}
}

std::string FormatWithCommas(double value)
{
	std::stringstream ss;
	ss.imbue(std::locale(""));
	ss << std::fixed << trunc(value);
	return ss.str();
}

static size_t data_write(void* buf, size_t size, size_t nmemb, void* userp)
{
	if (userp)
	{
		std::ostream& os = *static_cast<std::ostream*>(userp);
		std::streamsize len = size * nmemb;
		if (os.write(static_cast<char*>(buf), len))
			return len;
	}

	return 0;
}

CURLcode curl_read(const std::string& url, std::ostream& os, long timeout = 30)
{
	CURLcode code(CURLE_FAILED_INIT);
	CURL* curl = curl_easy_init();

	if (curl)
	{
		if (CURLE_OK == (code = curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, &data_write))
			&& CURLE_OK == (code = curl_easy_setopt(curl, CURLOPT_NOPROGRESS, 1L))
			&& CURLE_OK == (code = curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L))
			&& CURLE_OK == (code = curl_easy_setopt(curl, CURLOPT_FILE, &os))
			&& CURLE_OK == (code = curl_easy_setopt(curl, CURLOPT_TIMEOUT, timeout))
			&& CURLE_OK == (code = curl_easy_setopt(curl, CURLOPT_URL, url.c_str())))
		{
			code = curl_easy_perform(curl);
		}
		curl_easy_cleanup(curl);
	}
	return code;
}

bool containsSubstring(const std::string& mainString, const std::string& subString) {
	return mainString.find(subString) != std::string::npos;
}

// Function to format the duration into days, hours, minutes, seconds
std::string formatDuration(std::chrono::seconds duration)
{
	auto days = std::chrono::duration_cast<std::chrono::hours>(duration).count() / 24;
	duration -= std::chrono::hours(days * 24);

	auto hours = std::chrono::duration_cast<std::chrono::hours>(duration).count();
	duration -= std::chrono::hours(hours);

	auto minutes = std::chrono::duration_cast<std::chrono::minutes>(duration).count();
	duration -= std::chrono::minutes(minutes);

	auto seconds = duration.count();

	return std::to_string(days) + " days, " +
		std::to_string(hours) + " hours, " +
		std::to_string(minutes) + " minutes, " +
		std::to_string(seconds) + " seconds";
}

std::string boolToString(bool value) {
	if (value)
		return "true";
	else
		return "false";
}

static std::string current_time() {
	auto now = std::chrono::system_clock::now();
	std::time_t time = std::chrono::system_clock::to_time_t(now);
	struct tm localTimeInfo;
	localtime_s(&localTimeInfo, &time);
	char timestamp[64];
	return std::to_string(std::strftime(timestamp, sizeof(timestamp), "[%a %b %d %T %Y] ", &localTimeInfo)) + " ";
}

static std::string removeEmptyLineAtEnd(const std::string& input) {
	std::string result = input;
	size_t pos = result.find_last_not_of("\r\n"); // Find the last non-empty line
	if (pos != std::string::npos) {
		// If there's at least one non-empty line, extract the substring up to that point
		result = result.substr(0, pos + 1);
	}
	return result;
}

static std::string split_osuid(const std::string& input) {
	std::istringstream ss(input);
	std::vector<std::string> tokens;
	std::string token;

	while (std::getline(ss, token, ',')) {
		tokens.push_back(token);
	}

	if (tokens.size() >= 2) {
		return tokens[1];
	}
	else {
		return ""; // Return an empty string if the format is invalid.
	}
}

static std::string split_discordid(const std::string& input) {
	std::istringstream ss(input);
	std::vector<std::string> tokens;
	std::string token;

	while (std::getline(ss, token, ',')) {
		tokens.push_back(token);
	}

	if (tokens.size() >= 2) {
		return tokens[0];
	}
	else {
		return ""; // Return an empty string if the format is invalid.
	}
}

static std::string split_discordUsername(const std::string& input) {
	std::istringstream ss(input);
	std::vector<std::string> tokens;
	std::string token;

	while (std::getline(ss, token, ',')) {
		tokens.push_back(token);
	}

	if (tokens.size() >= 2) {
		return tokens[2];
	}
	else {
		return ""; // Return an empty string if the format is invalid.
	}
}

static int registered_user_count() {
	int index = 0;

	std::ifstream inputFile("data/register.txt");
	std::string line = "";
	if (inputFile) {
		while (std::getline(inputFile, line)) {
			if (containsSubstring(line, ",")) {
				index++;
			}
		}
		inputFile.close();

		return index;
	}
	else {
		return 0;
	}
}

std::string split_comma(const std::string& input, int index) {
	std::istringstream ss(input);
	std::vector<std::string> tokens;
	std::string token;

	while (std::getline(ss, token, ',')) {
		tokens.push_back(token);
	}

	if (!tokens.empty() && index >= 0 && index < tokens.size()) {
		return tokens[index];
	}
	else {
		return ""; // Handle invalid index or empty tokens appropriately.
	}
}

static std::pair<std::string, double> getDataAtIndex(const std::vector<std::pair<std::string, double>>& data, int index) {
	if (index >= 0 && index < data.size()) {
		return data[index];
	}
	else {
		throw std::out_of_range("Invalid index.");
	}
}

std::vector<std::string> splitString(const std::string& input, char delimiter) {
	std::vector<std::string> tokens;
	std::istringstream stream(input);
	std::string token;

	while (std::getline(stream, token, delimiter)) {
		tokens.push_back(token);
	}

	return tokens;
}

std::string keepBeforeComma(const std::string& inputString) {
	size_t commaPos = inputString.find(',');
	if (commaPos != std::string::npos) {
		return inputString.substr(0, commaPos);
	}
	else {
		return inputString;
	}
}

std::string cutAfterComma(const std::string& inputString) {
	size_t commaPos = inputString.find(',');
	if (commaPos != std::string::npos) {
		return inputString.substr(commaPos + 1);
	}
	else {
		return inputString;
	}
}

std::string ReplaceString(const std::string& subject, const std::string& search, const std::string& replace) {
	std::string result = subject;
	size_t pos = 0;
	while ((pos = result.find(search, pos)) != std::string::npos) {
		result.replace(pos, search.length(), replace);
		pos += replace.length();
	}
	return result;
}