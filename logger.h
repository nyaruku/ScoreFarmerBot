#pragma once
#include <string>
#include <iostream>
#include <chrono>
#include <ctime>
#include <iomanip>
#include <fstream>

static void console_log(const int level, const std::string content) {
	std::string color;
	std::string default_color = "[0;38;48m";
	std::string error;
	// 0 : debug
	// 1 : info
	// 2 : warning
	// 3 : error
	// 4 : FATAL
	

	if (level == 0) { // DEBUG
		error = "DEBUG";
		color = "[0;34;48m";
	}
	if (level == 1) { // INFO
		error = "INFO";
		color = "[0;37;48m";
	}
	if (level == 2) { // WARN
		error = "WARN";
		color = "[0;33;48m";
	}
	if (level == 3) { // ERROR
		error = "ERROR";
		color = "[0;31;48m";
	}
	if (level == 4) { // FATAL
		error = "FATAL";
		color = "[0;41m";
	}
	// Get the current system time
	auto currentTime = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());

	// Convert to a time structure
	std::tm timeInfo;
	localtime_s(&timeInfo, &currentTime);

	// Format the date and time as strings
	char dateBuffer[20];
	strftime(dateBuffer, sizeof(dateBuffer), "%Y-%m-%d", &timeInfo);

	char timeBuffer[9];
	strftime(timeBuffer, sizeof(timeBuffer), "%H:%M:%S", &timeInfo);

	// logging

	// Open the file in binary mode for appending with ANSI encoding
	std::ofstream file("data/log.ansi", std::ios::out | std::ios::app | std::ios::binary);

	// Text to append with a new line
	std::string text = color + "" + dateBuffer + " " + timeBuffer + " [" + error + "] " + content + default_color + "\n";

	// Appending the text to the file
	file.write(text.c_str(), text.length());

	// Close the file
	file.close();
	// log to console
	std::cout << "" << dateBuffer << " " << timeBuffer << " [" + error + "] " << content << std::endl;
}

static void clear_log() {
	// delete log file, and create a new one
	std::remove("data/log.ansi");
	std::ofstream outputFile("data/log.ansi");
	if (outputFile.is_open()) {
		outputFile.close();
		console_log(0, "Log File Created.");
	}
}