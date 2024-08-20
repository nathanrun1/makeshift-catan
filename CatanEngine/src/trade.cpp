#include "trade.h"

Trade::Trade() : offer(), request() {};

Trade::Trade(const std::unordered_map<Resource, int> offer, const std::unordered_map<Resource, int> request) :
	offer(offer), request(request) {};

bool Trade::operator==(const Trade& other) const {
	return this->offer == other.offer && this->request == other.request;
}

bool Trade::operator!=(const Trade& other) const {
	return *this == other;
}

Trade::operator std::string() const {
	std::string repr("Acting player gives: ");
	for (const std::pair<const Resource, int>& rsc : offer) {
		if (rsc.second > 0) {
			repr += std::to_string(rsc.second) + " " + RscToString(rsc.first) + ", ";
		}
	}
	repr = repr.substr(0, repr.size() - 2);
	repr += "\nActing player takes: ";
	for (const std::pair<const Resource, int>& rsc : request) {
		if (rsc.second > 0) {
			repr += std::to_string(rsc.second) + " " + RscToString(rsc.first) + ", ";
		}
	}
	repr = repr.substr(0, repr.size() - 2);
	return repr;
}

bool Trade::is_valid() {
	if (std::max_element(offer.begin(), offer.end(),
		[](const std::pair<Resource, int>& a, const std::pair<Resource, int>& b)->bool { return a.second < b.second; })->second == 0) {
		return false;
	}
	if (std::max_element(request.begin(), request.end(), 
		[](const std::pair<Resource, int>& a, const std::pair<Resource, int>& b)->bool { return a.second < b.second; })->second == 0) {
		return false;
	}
	for (const std::pair<Resource, int>& rsc_amnt_pair : offer) {
		const Resource& rsc = rsc_amnt_pair.first;
		if (offer[rsc] > 0 && request[rsc] > 0) {
			return false;
		}
	}
	return true;
}
