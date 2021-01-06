#include "ThingSpeak.h"

namespace
{
    std::size_t callback(const char* in, std::size_t size, std::size_t num, char* out)
    {
        std::string data(in, (std::size_t) size * num);
        *((std::stringstream*) out) << data;
        return size * num;
    }
}

ThingSpeak::ThingSpeak(char* tsChannel, char* tsKey, char* tsRequests) : 
    thingspeakChannel(tsChannel), thingspeakKey(tsKey), thingspeakNumRequests(tsRequests){}

ThingSpeak::~ThingSpeak() {}

void ThingSpeak::getChannelData()
{
    // clear previous data before storing new set
    fieldResults.clear();

    CURL* curl = curl_easy_init();

    // Set remote URL.
    string thingspeakUrl = buildUrl();
    curl_easy_setopt(curl, CURLOPT_URL, thingspeakUrl.c_str());

    // Don't bother trying IPv6, which would increase DNS resolution time.
    curl_easy_setopt(curl, CURLOPT_IPRESOLVE, CURL_IPRESOLVE_V4);

    // Don't wait forever, time out after 10 seconds.
    curl_easy_setopt(curl, CURLOPT_TIMEOUT, 10);

    // Follow HTTP redirects if necessary.
    curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);

    // Response information.
    long httpCode(0);
    stringstream httpData;

    // Hook up data handling function.
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, callback);

    // Hook up data container (will be passed as the last parameter to the
    // callback handling function).  Can be any pointer type, since it will
    // internally be passed as a void pointer.
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &httpData);

    // Run our HTTP GET command, capture the HTTP response code, and clean up.
    curl_easy_perform(curl);
    curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &httpCode);
    curl_easy_cleanup(curl);

    if (httpCode == 200) // valid response
    {
        std::cout << "\nGot successful response from " << thingspeakUrl << std::endl;

        Json::Value jsonData;
        Json::CharReaderBuilder jsonReader;
        string errs;

        if (Json::parseFromStream(jsonReader, httpData, &jsonData, &errs))
        {
            const Json::Value jsonValues = jsonData["feeds"];

            for (int i = 0; i < jsonValues.size(); i++)
            {
                // iterate and store thingspeak values to field vector
                currResult.insert(pair<string, string>("created_at", jsonValues[i][jsonValues[i].getMemberNames()[0]].asString()));
                currResult.insert(pair<string, string>("entry_id", jsonValues[i][jsonValues[i].getMemberNames()[1]].asString()));
                currResult.insert(pair<string, string>("field1", jsonValues[i][jsonValues[i].getMemberNames()[2]].asString()));
                currResult.insert(pair<string, string>("field2", jsonValues[i][jsonValues[i].getMemberNames()[3]].asString()));
                currResult.insert(pair<string, string>("field3", jsonValues[i][jsonValues[i].getMemberNames()[4]].asString()));
                currResult.insert(pair<string, string>("field4", jsonValues[i][jsonValues[i].getMemberNames()[5]].asString()));
                currResult.insert(pair<string, string>("field5", jsonValues[i][jsonValues[i].getMemberNames()[6]].asString()));
                currResult.insert(pair<string, string>("field6", jsonValues[i][jsonValues[i].getMemberNames()[7]].asString()));
                currResult.insert(pair<string, string>("field7", jsonValues[i][jsonValues[i].getMemberNames()[8]].asString()));
                currResult.insert(pair<string, string>("field8", jsonValues[i][jsonValues[i].getMemberNames()[9]].asString()));

                fieldResults.push_back(currResult);

                currResult.clear();
            }
        }
        else
        {
            cout << "Could not parse HTTP data as JSON" << endl;
            cout << "HTTP data was: " << httpData.str() << std::endl;
        }
    }
    else
    {
        std::cout << "Couldn't GET from " << thingspeakUrl << " - exiting" << std::endl;
    }
}

void ThingSpeak::printData()
{
    for (auto& myMap : fieldResults)
    {
        for (auto it = myMap.cbegin(); it != myMap.cend(); ++it)
        {
            cout << it->first << " " << it->second << "\n";
        }

        cout << endl;
    }
}

vector<map<string, string>> ThingSpeak::getFieldResults()
{
    return fieldResults;
}

string ThingSpeak::buildUrl()
{
    string url = "https://api.thingspeak.com/channels/";
    url += thingspeakChannel;
    url += "/feeds.json?api_key=";
    url += thingspeakKey;
    url += "&results=";
    url += thingspeakNumRequests;

    return url;
}