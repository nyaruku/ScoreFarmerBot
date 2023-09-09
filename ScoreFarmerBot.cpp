#pragma once
#include <dpp/dpp.h>
#include "config.h"
#include <iostream>
#include <chrono>
#include <thread>
#include "init.h"
#include "api.h"
#include "commands.h"
#include <filesystem>
#include "extension.h"
#include <ctime>
#include <iomanip>
#include <functional>
#include "logger.h"

using namespace std::placeholders;
namespace fs = std::filesystem;

int currentYear = -1; // Initialize to an invalid year
int currentYear_old = -1; // Initialize to an invalid year
bool newYear = false;
int dayCount = -1;    // Initialize to an invalid value

int weekCount = -1;    // Initialize to an invalid value

int exceptionCount = 0;
long secondsUntilNextExecution = 1;
long secondsUntilNextExecution_weekly = 1;
long authSeconds = 1;
bool doWork = false;
bool doWork_weekly = false;
bool doAuth = false;

bool dev = !production; // only false in Production
bool notify_user = production; // only true in Production
bool allow_owner = !production; // only false in Production
bool doTimers = production;  // only true in Production
// Global variables to store start time and exception count
std::chrono::high_resolution_clock::time_point startTime;

// Custom exception handler
void handleException()
{
	exceptionCount++;
}

void initializeDayCount() {
	// Get the current time in UTC
	std::time_t currentTime = std::time(nullptr);
	std::tm* utcTime = std::gmtime(&currentTime);

	// Set the current year and day count
	currentYear = utcTime->tm_year + 1900;
	dayCount = utcTime->tm_yday + 1; // tm_yday starts from 0
}

void initializeWeekCount() {
	// Get the current time in UTC
	std::time_t currentTime = std::time(nullptr);
	std::tm* utcTime = std::gmtime(&currentTime);

	// Calculate the day of the week (0 = Sunday, 1 = Monday, ..., 6 = Saturday)
	int currentDayOfWeek = utcTime->tm_wday;

	// Calculate the current week count based on the day of the week and the day of the year
	weekCount = (utcTime->tm_yday + 1 + 6 - currentDayOfWeek) / 7; // Adding 1 to tm_yday to start from 1

	// Set the current year and day count
	currentYear = utcTime->tm_year + 1900;
	dayCount = utcTime->tm_yday + 1; // tm_yday starts from 0
}

void dailyTask(dpp::cluster& bot) {
	if (dayCount == -1) {
		initializeDayCount();
	}
	bot.start_timer([&bot](dpp::timer h) {
		if (doWork == true) { //should not execute at very first launch of task
			// Perform your daily task here
			std::stringstream message;
			message << "Top Farmers Day " << dayCount << " of " << currentYear;
			bot.message_create(dpp::message(789503509493448769, dump_compare(message.str()))); // top3

			dpp::message msg(1139006502884098178, message.str());
			msg.add_file("daily.txt", dump_compare_all(message.str())); // dump all
			bot.message_create(msg); // full dump
			dump_daily(); // re-dump current users and stats
		}

		// Get the current time in UTC
		std::time_t currentTime = std::time(nullptr);
		std::tm* utcTime = std::gmtime(&currentTime);

		// Check if the year has changed
		if (utcTime->tm_year + 1900 != currentYear) {
			currentYear = utcTime->tm_year + 1900;
			dayCount = 1; // Reset the day count on a new year
		}
		else {
			dayCount = utcTime->tm_yday + 1; // Update the day count based on the day of the year
		}

		// Calculate the time until the next 00:00 UTC+0
		int hours = 24 - utcTime->tm_hour;
		int minutes = 60 - utcTime->tm_min;
		int seconds = 60 - utcTime->tm_sec;
		// Calculate the time until the next 00:00 UTC+0
		int hoursUntilNextExecution = 24 - utcTime->tm_hour;
		int minutesUntilNextExecution = 60 - utcTime->tm_min;

		// Calculate the total time in seconds until the next execution
		secondsUntilNextExecution = hoursUntilNextExecution * 3600 + minutesUntilNextExecution * 60;
		// Sleep until the next execution time
		if (doWork == false) {
			bot.message_create(dpp::message(1134814047137697803, "Task has been created.\nAnd daily will be posted in: " + std::to_string(secondsUntilNextExecution) + " total seconds...\nOr in: " +
				std::to_string(hours) + " hours and " +
				std::to_string(minutes) + " minutes and " +
				std::to_string(seconds) + " seonds."
			));
			currentYear_old = currentYear;
		}

		doWork = true;
		bot.stop_timer(h);
		dailyTask(bot);
		}, secondsUntilNextExecution);
}

void weeklyTask(dpp::cluster& bot) {
	if (dayCount == -1) {
		initializeDayCount();
	}

	if (weekCount == -1) {
		initializeWeekCount();
	}

	bot.start_timer([&bot](dpp::timer h) {
		if (doWork_weekly == true) { //should not execute at very first launch of task
			// Perform your weekly task here
			std::stringstream message;
			message << "Top Farmers Weeky " << weekCount << " of " << currentYear_old;
			currentYear_old = currentYear;
			bot.message_create(dpp::message(789503612111814687, dump_compare_weekly(message.str()))); // top3

			dpp::message msg(1150098000669065418, message.str());
			msg.add_file("weekly.txt", dump_compare_all_weekly(message.str())); // dump all
			bot.message_create(msg); // full dump
			dump_weekly(); // re-dump current users and stats
			if (newYear == false) {
				weekCount++;
			}
			if (newYear == true) {
				weekCount = 1;
				newYear = false;
			}
		}

		// Get the current time in UTC
		std::chrono::system_clock::time_point now = std::chrono::system_clock::now();
		std::time_t now_time_t = std::chrono::system_clock::to_time_t(now);
		std::tm now_tm = *std::gmtime(&now_time_t);

		// Calculate the time until the next Monday 00:00 UTC
		int days_until_monday = (8 - now_tm.tm_wday) % 7;
		std::chrono::system_clock::time_point next_monday = now + std::chrono::hours(days_until_monday * 24 - now_tm.tm_hour);

		// Check if the next Monday falls in the next year
		if (next_monday < now) {
			next_monday += std::chrono::hours(7 * 24); // Add a week if it's in the past (next year)
			newYear = true;
		}

		// Calculate the time difference in seconds
		std::chrono::seconds time_until_monday = std::chrono::duration_cast<std::chrono::seconds>(next_monday - now);

		// Calculate days, hours, minutes, and remaining seconds
		int days = time_until_monday.count() / (60 * 60 * 24);
		int hours = (time_until_monday.count() % (60 * 60 * 24)) / (60 * 60);
		int minutes = (time_until_monday.count() % (60 * 60)) / 60;
		int seconds = time_until_monday.count() % 60;

		secondsUntilNextExecution_weekly = time_until_monday.count();
		secondsUntilNextExecution_weekly += 300; // 5min buffer so daily task can execute first

		// Sleep until the next execution time
		if (doWork_weekly == false) {
			bot.message_create(dpp::message(1134814047137697803, "Task has been created.\nAnd Weekly will be posted in: " + std::to_string(secondsUntilNextExecution_weekly) + " total seconds...\nOr in: " +
				std::to_string(days) + " days and " +
				std::to_string(hours) + " hours and " +
				std::to_string(minutes) + " minutes and " +
				std::to_string(seconds) + " seonds.\n(5 minute delay is included so daily task can execute first)"
			));
		}

		doWork_weekly = true;
		bot.stop_timer(h);
		weeklyTask(bot);
		}, secondsUntilNextExecution_weekly);
}

void authTask(dpp::cluster& bot) {
	bot.start_timer([&bot](dpp::timer h) {
		if (doAuth) { //should not execute at very first launch of task, bc i already did Auth()
			Auth();
			bot.message_create(dpp::message(1133460451351601203, "Auth Token refreshed."));
		}
		else {
			authSeconds = 14400;
			bot.message_create(dpp::message(1133460451351601203, "Auth Task started, for every 4h | 14400 Seconds."));
			doAuth = true;
		}
		bot.stop_timer(h);
		authTask(bot);
		}, authSeconds);
}

int main() {
	pre();

	// Record the start time
	startTime = std::chrono::high_resolution_clock::now();

	// Set the custom exception handler
	std::set_terminate(handleException);

	dpp::cluster bot(bot_token, dpp::i_default_intents | dpp::i_message_content);

	// Start the daily/weekly task in a separate thread
	if (doTimers) {
		dailyTask(bot);
		weeklyTask(bot);
	}
	authTask(bot);

	try {
		bot.on_log([](const dpp::log_t& log) {
			if (log.severity == dpp::loglevel::ll_critical) {
				console_log(4, log.message);
			}
			if (log.severity == dpp::loglevel::ll_error) {
				console_log(3, log.message);
			}
			if (log.severity == dpp::loglevel::ll_warning) {
				console_log(2, log.message);
			}
			if (log.severity == dpp::loglevel::ll_info) {
				console_log(1, log.message);
			}
			if (log.severity == dpp::loglevel::ll_debug) {
				console_log(0, log.message);
			}
			});

		bot.on_ready([&bot](const dpp::ready_t& event) {
			// EVAPORATE all slash commands
			// std::vector<dpp::slashcommand> myEmptyVector;
			// bot.global_bulk_command_create(myEmptyVector);

			dpp::activity activity(dpp::activity_type::at_game, ">>help", "Playing osu!", "");
			dpp::presence presence(dpp::presence_status::ps_online, activity);
			bot.set_presence(presence);

			bot.message_create(dpp::message(logging_channel_id, "Bot Launched >> Username: " + bot.me.username));
			bot.message_create(dpp::message(logging_channel_id, "Performing API & CURL TEST..."));
			bot.message_create(dpp::message(logging_channel_id, "Acquiring Auth Token"));
			Auth();
			bot.message_create(dpp::message(logging_channel_id, "Auth Token acquired."));
			});

		bot.on_message_create([&bot](const dpp::message_create_t& event) {
			if (event.msg.author.is_bot()) return; // ignore bots
			std::string content = event.msg.content;
			if (content == prefix + "dev") {
				if (std::to_string(event.msg.author.id) == owner_discord_id) {
					dev = !dev;
					return bot.message_create(dpp::message(event.msg.channel_id, " Dev Mode is now: " + boolToString(dev)));
				}
				else {
					return bot.message_create(dpp::message(event.msg.channel_id, "No Permission to access this command."));
				}
			}
			if (dev) {
				if (content._Starts_with(">>")) {
					if (std::to_string(event.msg.author.id) == owner_discord_id) {
						if (allow_owner) {
						}
						else {
							return; // nothing dont annoy the owner
						}
					}
					else {
						if (notify_user) {
							return bot.message_create(dpp::message(event.msg.channel_id, "No Access to any commands, bot is currently in dev mode (maintenance mode) as the bot developer is currently working on it."));
						}
					}
				}
			}

			if (content == prefix + "help") {
				return bot.message_create(dpp::message(event.msg.channel_id, help()));
			}
			if (content == prefix + "about") {
				return bot.message_create(dpp::message(event.msg.channel_id, about(bot.me.get_avatar_url())));
			}
			if (content == prefix + "scorefarming") {
				return bot.message_create(dpp::message(event.msg.channel_id, event.msg.author.username + " loves Score Farming and then " + event.msg.author.username + " score farmed all over the place."));
			}
			if (content._Starts_with(prefix + "link ")) {
				// Extract the argument after the "link" command
				std::string arg = content.substr(prefix.length() + 5); // +5 for "link" and the space after it

				// Check if the argument contains only digits
				bool isNumeric = true;
				for (char c : arg) {
					if (!std::isdigit(c)) {
						isNumeric = false;
						break;
					}
				}

				if (isNumeric) {
					bot.message_create(dpp::message(event.msg.channel_id, "Registering osu!UserID: " + arg + "..."));
					return bot.message_create(dpp::message(event.msg.channel_id, register_player(std::to_string(event.msg.author.id), arg, event.msg.author.username)).set_reference(event.msg.id));
				}
				else {
					return bot.message_create(dpp::message(event.msg.channel_id, "Invalid argument. Argument must contain only numbers.").set_reference(event.msg.id));
				}
			}
			if (content == prefix + "profile") {
				std::string osuid = get_userid(std::to_string(event.msg.author.id));
				if (osuid == "") {
					return bot.message_create(dpp::message(event.msg.channel_id, "Something went wrong or you are not linked.").set_reference(event.msg.id));
				}
				else {
					return bot.message_create(dpp::message(event.msg.channel_id, profile_stats(osuid, default_mode)).set_reference(event.msg.id));
				}
			}
			if (content._Starts_with(prefix + "profile.")) {
				std::string osu_name = content;
				ReplaceStringInPlace(osu_name, prefix + "profile.", "");
				return bot.message_create(dpp::message(event.msg.channel_id, profile_stats_name(osu_name, default_mode)).set_reference(event.msg.id));
			}
			if (content._Starts_with(prefix + "profile ")) {
				std::string arg = content.substr(prefix.length() + 8); // +8 for "profile" and the space after it

				// Check if the argument contains only digits
				bool isNumeric = true;
				for (char c : arg) {
					if (!std::isdigit(c)) {
						isNumeric = false;
						break;
					}
				}

				if (isNumeric) {
					return bot.message_create(dpp::message(event.msg.channel_id, profile_stats(arg, default_mode)).set_reference(event.msg.id));
				}
				else {
					return bot.message_create(dpp::message(event.msg.channel_id, "Invalid argument. Argument must contain only numbers."));
				}
			}
			if (content == prefix + "runtime") {
				// Calculate the runtime of the application
				auto endTime = std::chrono::high_resolution_clock::now();
				auto duration = std::chrono::duration_cast<std::chrono::seconds>(endTime - startTime);
				return bot.message_create(dpp::message(event.msg.channel_id, "**Exceptions: **" + std::to_string(exceptionCount) + "\nActive Time: " + formatDuration(duration) + "\nLinked users: " + std::to_string(registered_user_count())));
			}
			if (content == prefix + "log") {
				// what am i even doing
				dpp::message msg(event.msg.channel_id, "Log File:");
				// attach the file to the message
				msg.add_file("log.ansi", dpp::utility::read_file(log_file));
				return bot.message_create(msg.set_reference(event.msg.id));
			}
			if (content == prefix + "clear_log") {
				if (std::to_string(event.msg.author.id) == owner_discord_id) {
					bot.message_create(dpp::message(event.msg.channel_id, "Clearing Log File..."));
					clear_log();
				}
				else {
					return bot.message_create(dpp::message(event.msg.channel_id, "No Permission to access this command."));
				}
			}
			if (content == prefix + "linked") {
				std::string response = get_all_users();
				if (response == "no file") {
					return bot.message_create(dpp::message(event.msg.channel_id, "No File."));
				}
				else {
					dpp::message msg(event.msg.channel_id, "");

					// attach the file to the message
					msg.add_file("content.txt", response);

					// send the message
					return bot.message_create(msg);
				}
			}
			if (content == prefix + "lb_rs") {
				std::string response = get_all_data("rs");
				if (response == "no file") {
					return bot.message_create(dpp::message(event.msg.channel_id, "No File."));
				}
				else {
					dpp::message msg(event.msg.channel_id, "");

					// attach the file to the message
					msg.add_file("rankedscore.txt", response);

					// send the message
					return bot.message_create(msg);
				}
			}
			if (content == prefix + "lb_ts") {
				std::string response = get_all_data("ts");
				if (response == "no file") {
					return bot.message_create(dpp::message(event.msg.channel_id, "No File."));
				}
				else {
					dpp::message msg(event.msg.channel_id, "");

					// attach the file to the message
					msg.add_file("totalscore.txt", response);

					// send the message
					return bot.message_create(msg);
				}
			}
			if (content == prefix + "lb_ss") {
				std::string response = get_all_data("ss");
				if (response == "no file") {
					return bot.message_create(dpp::message(event.msg.channel_id, "No File."));
				}
				else {
					dpp::message msg(event.msg.channel_id, "");

					// attach the file to the message
					msg.add_file("ss.txt", response);

					// send the message
					return bot.message_create(msg);
				}
			}
			if (content == prefix + "lb_s") {
				std::string response = get_all_data("s");
				if (response == "no file") {
					return bot.message_create(dpp::message(event.msg.channel_id, "No File."));
				}
				else {
					dpp::message msg(event.msg.channel_id, "");

					// attach the file to the message
					msg.add_file("s.txt", response);

					// send the message
					return bot.message_create(msg);
				}
			}
			if (content == prefix + "lb_a") {
				std::string response = get_all_data("a");
				if (response == "no file") {
					return bot.message_create(dpp::message(event.msg.channel_id, "No File."));
				}
				else {
					dpp::message msg(event.msg.channel_id, "");

					// attach the file to the message
					msg.add_file("a.txt", response);

					// send the message
					return bot.message_create(msg);
				}
			}
			if (content == prefix + "lb_sr") {
				std::string response = get_all_data("sr");
				if (response == "no file") {
					return bot.message_create(dpp::message(event.msg.channel_id, "No File."));
				}
				else {
					dpp::message msg(event.msg.channel_id, "");

					// attach the file to the message
					msg.add_file("sr.txt", response);

					// send the message
					return bot.message_create(msg);
				}
			}
			if (content == prefix + "dump_daily") {
				if (std::to_string(event.msg.author.id) == owner_discord_id) {
					dump_daily();
					return bot.message_create(dpp::message(event.msg.channel_id, "Daily Stats manually dumped!"));
				}
				else {
					return bot.message_create(dpp::message(event.msg.channel_id, "No Permission to access this command."));
				}
			}
			if (content == prefix + "dump_weekly") {
				if (std::to_string(event.msg.author.id) == owner_discord_id) {
					dump_weekly();
					return bot.message_create(dpp::message(event.msg.channel_id, "Weekly Stats manually dumped!"));
				}
				else {
					return bot.message_create(dpp::message(event.msg.channel_id, "No Permission to access this command."));
				}
			}
			if (content == prefix + "daily") {
				// Get the current time in UTC
				std::time_t currentTime = std::time(nullptr);
				std::tm* utcTime = std::gmtime(&currentTime);
				return bot.message_create(dpp::message(event.msg.channel_id, dump_compare("Top Farmers Day " + std::to_string(utcTime->tm_yday + 1) + " of " + std::to_string(utcTime->tm_year + 1900))));
			}

			if (content == prefix + "weekly") {
				// Get the current time in UTC
				std::time_t currentTime = std::time(nullptr);
				std::tm* utcTime = std::gmtime(&currentTime);
				// Calculate the current week count
				int currentWeek = (utcTime->tm_yday + 1) / 7 + 1; // Assuming weeks start from 1

				//return bot.message_create(dpp::message(event.msg.channel_id, dump_compare("Top Farmers Week " + std::to_string(currentWeek) + " of " + std::to_string(utcTime->tm_year + 1900))));
				return bot.message_create(dpp::message(event.msg.channel_id, dump_compare("Top Farmers Week " + std::to_string(weekCount) + " of " + std::to_string(currentYear_old))));
			}
			if (content == prefix + "daily_all") {
				// Get the current time in UTC
				std::time_t currentTime = std::time(nullptr);
				std::tm* utcTime = std::gmtime(&currentTime);
				dpp::message msg(event.msg.channel_id, "Top Farmers Day " + std::to_string(utcTime->tm_yday + 1) + " of " + std::to_string(utcTime->tm_year + 1900));
				msg.add_file("daily.txt", dump_compare_all("Top Farmers Day " + std::to_string(utcTime->tm_yday + 1) + " of " + std::to_string(utcTime->tm_year + 1900)));
				return bot.message_create(msg);
			}

			if (content == prefix + "weekly_all") {
				// Get the current time in UTC
			//	std::time_t currentTime = std::time(nullptr);
			//	std::tm* utcTime = std::gmtime(&currentTime);

				// Calculate the current week count
			//	int currentWeek = (utcTime->tm_yday + 1) / 7 + 1; // Assuming weeks start from 1

			//	dpp::message msg(event.msg.channel_id, "Top Farmers Week " + std::to_string(weekCount) + " of " + std::to_string(utcTime->tm_year + 1900));
				dpp::message msg(event.msg.channel_id, "Top Farmers Week " + std::to_string(weekCount) + " of " + std::to_string(currentYear_old));
				//	msg.add_file("weekly.txt", dump_compare_all("Top Farmers Week " + std::to_string(weekCount) + " of " + std::to_string(utcTime->tm_year + 1900)));
				msg.add_file("weekly.txt", dump_compare_all("Top Farmers Week " + std::to_string(weekCount) + " of " + std::to_string(currentYear_old)));
				return bot.message_create(msg);
			}
			if (content == prefix + "when_daily") {
				// Get the current time in UTC
				std::time_t currentTime = std::time(nullptr);
				std::tm* utcTime = std::gmtime(&currentTime);
				// Calculate the time until the next 00:00 UTC+0
				int hours = 24 - utcTime->tm_hour;
				int minutes = 60 - utcTime->tm_min;
				int seconds = 60 - utcTime->tm_sec;

				std::stringstream response;
				response << "The daily task will be executed in " << hours << " hours and "
					<< minutes << " minutes and " << seconds << " seonds.";
				return bot.message_create(dpp::message(event.msg.channel_id, response.str()));
			}

			if (content == prefix + "when_weekly") {
				// Get the current time in UTC
				std::time_t currentTime = std::time(nullptr);
				std::tm* utcTime = std::gmtime(&currentTime);

				// Calculate the time until the next Monday 00:00 UTC+0
				int daysUntilMonday = (7 - utcTime->tm_wday) % 7;
				int hours = 24 - utcTime->tm_hour;
				int minutes = 60 - utcTime->tm_min;
				int seconds = 60 - utcTime->tm_sec;

				// Calculate the total time until Monday 00:00 UTC+0
				int totalHours = daysUntilMonday * 24 + hours;
				int totalMinutes = totalHours * 60 + minutes;
				int totalSeconds = totalMinutes * 60;

				std::stringstream response;
				response << "The weekly task will be executed in " << daysUntilMonday << " days, "
					<< hours << " hours, " << minutes << " minutes, and " << seconds << " seconds.\n(5 minute delay is included so daily task can execute first)";

				return bot.message_create(dpp::message(event.msg.channel_id, response.str()));
			}

			if (content == prefix + "auth") {
				if (std::to_string(event.msg.author.id) == owner_discord_id) {
					Auth();
					return bot.message_create(dpp::message(event.msg.channel_id, "Auth Token refreshed."));
				}
				else {
					return bot.message_create(dpp::message(event.msg.channel_id, "No Permission to access this command."));
				}
			}
			if (content == prefix + "owner") {
				dpp::embed embed = dpp::embed().
					set_color(dpp::colors::sti_blue).
					set_title("Owner Help").
					set_description("**Owner Available Commands**\nPrefix: '>>'").
					add_field(
						"**auth**",
						"Refreshes Auth token."
					).
					add_field(
						"**dump_daily/weekly**",
						"Manually dumps stats."
					).
					add_field(
						"**clear_log**",
						"Clears log."
					).
					add_field(
						"**dev**",
						"Enables dev mode.\nDisables Bot interaction for all users."
					).
					set_timestamp(time(0));
				return bot.message_create(dpp::message(event.msg.channel_id, embed));
			}
			});
	}
	catch (...) {
		bot.message_create(dpp::message(logging_channel_id, "Some Error:"));
		dpp::message msg(logging_channel_id, "Log File:");
		msg.add_file("log.ansi", dpp::utility::read_file(log_file));
		bot.message_create(msg);
	}
	bot.start(dpp::st_wait);
	return 0;
}