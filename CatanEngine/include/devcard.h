#pragma once

#include <unordered_map>
#include <string>

enum class DevCard {
	VictoryPoint, Monopoly, RoadBuilder, YearOfPlenty, Knight
};

const std::unordered_map<DevCard, std::string> devcard_str_map = {
	{DevCard::VictoryPoint, "Victory Point"},
	{DevCard::Monopoly, "Monopoly"},
	{DevCard::RoadBuilder, "Road Builder"},
	{DevCard::YearOfPlenty, "Year of Plenty"},
	{DevCard::Knight, "Knight"}
};