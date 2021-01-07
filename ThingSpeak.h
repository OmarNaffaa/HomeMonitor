#ifndef THINGSPEAK_H
#define THINGSPEAK_H

#include <iostream>
#include <string>
#include <vector>
#include <exception>
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
	ThingSpeak(char* tsChannel, char* tsKey, char* numRequests);
	~ThingSpeak();

	// Functions
	void printData();
	void getChannelData();
	string getMostRecentTemp(int fieldNum);
	vector<map<string, string>> getFieldResults();

protected:
	char* thingspeakKey;
	char* thingspeakChannel;
	char* thingspeakNumRequests;
	map<string, string> currResult;
	vector<map<string, string>> fieldResults;

	string buildUrl();
};

#endif // THINGSPEAK_H