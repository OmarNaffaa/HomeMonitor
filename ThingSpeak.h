#ifndef THINGSPEAK_H
#define THINGSPEAK_H

#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <curl/curl.h>
#include <json/json.h>

using std::cout;
using std::endl;
using std::string;
using std::stringstream;
using std::vector;
using std::map;
using std::pair;

class ThingSpeak
{
public:
	// Constructor and deconstructor
	ThingSpeak(char* url);
	~ThingSpeak();

	// Functions
	void setUrl(const char *url);
	void printData();
	void getChannelData();
	vector<map<string, string>> getFieldResults();

protected:
	const char* thingspeakUrl;
	map<string, string> currResult;
	vector<map<string, string>> fieldResults;
};

#endif // THINGSPEAK_H