#include <iostream>
#include <sstream>
#include <string>
#include <curl/curl.h>
#include "json.hpp"

using json = nlohmann::json;

json parseKeyValuePairsToJSON(const std::string& input);
size_t WriteCallback(void* contents, size_t size, size_t nmemb, std::string* output);
bool performPostRequest(const std::string& url, const std::string& postData, std::string& response);
bool performPostRequestOffline(const std::string& url, const std::string& postData, std::string& response);
