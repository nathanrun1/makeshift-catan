#pragma once

#include <iostream>
#include <string>

/// <summary>
/// Used for game console I/O
/// </summary>
class Catan_IO {
public:
	/// <summary>
	/// Use for prompting a player for input
	/// </summary>
	static void Prompt(std::string prompt) {
		std::cout << "[PROMPT]: " << prompt << "\n";
	}

	/// <summary>
	/// Use for displaying game info
	/// </summary>
	static void Info(std::string msg) {
		std::cout << "[INFO]: " << msg << "\n";
	}
	/// <summary>
	/// Use for debug messages
	/// Only runs if _CATAN_DEBUG_ macro is defined
	/// </summary>
	static void Debug(std::string msg) {
#ifdef _CATAN_DEBUG_
		std::cout << "[DEBUG]: " << msg << "\n";
#endif
	}
	/// <summary>
	/// Gets one line of input from console, stores in given string
	/// </summary>
	/// <param name="input">- String to store input</param>
	/// <returns></returns>
	static std::istream& GetLine(std::string& input) {
		return std::getline(std::cin, input);
	}
};