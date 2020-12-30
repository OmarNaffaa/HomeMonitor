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
	void getChannelData();
	void printData();

protected:
	const char* thingspeakUrl;
	map<string, string> currResult;
	vector<map<string, string>> fieldResults;
	friend class DrawingArea;
};

#endif // THINGSPEAK_H