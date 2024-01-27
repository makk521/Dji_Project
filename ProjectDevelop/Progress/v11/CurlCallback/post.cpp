#include "post.hpp"

json parseKeyValuePairsToJSON(const std::string& input) {
    json jsonData;

    // 使用逗号分割字符串
    std::istringstream ss(input);
    std::string token;

    while (std::getline(ss, token, ',')) {
        // 使用冒号分割键值对
        size_t pos = token.find(':');
        if (pos != std::string::npos) {
            std::string key = token.substr(0, pos);
            std::string value = token.substr(pos + 1);

            // 移除可能存在的额外的空格
            key.erase(key.find_last_not_of(" \t\n\r\f\v") + 1);
            value.erase(0, value.find_first_not_of(" \t\n\r\f\v"));

            // 将键值对添加到 JSON 对象中
            jsonData[key] = value;
        }
    }

    return jsonData;
}

// Callback function to handle server response
size_t WriteCallback(void* contents, size_t size, size_t nmemb, std::string* output) {
    size_t total_size = size * nmemb;
    output->append((char*)contents, total_size);
    return total_size;
}

// Function to make a POST request
bool performPostRequest(const std::string& url, const std::string& postData, std::string& response) {
    // Initialize libcurl
    if (curl_global_init(CURL_GLOBAL_DEFAULT) != CURLE_OK) {
        std::cerr << "Failed to initialize libcurl." << std::endl;
        return false;
    }

    // Create a CURL object
    CURL* curl = curl_easy_init();
    if (curl) {
        // Set request URL
        curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
        json jsonData;
        json jsonBody;
        // 解析字符串为 JSON 对象
        json postDataJson = parseKeyValuePairsToJSON(postData);
        jsonData["cmdNum"] = postDataJson["cmdNum"];
        jsonData["timestamp"] = postDataJson["timestamp"];

        jsonBody["code"] = postDataJson["code"];
        jsonBody["data"] = jsonData;
        jsonBody["message"] = postDataJson["message"];
        // jsonData["body"] = postDataJson;
        // 将 JSON 对象转换为格式化字符串，确保空格的存在
        std::string postDataString = jsonBody.dump(4);
        std::cout << postDataString << std::endl;
        // 设置 POST 数据
        curl_easy_setopt(curl, CURLOPT_POSTFIELDS, postDataString.c_str());

        // 设置 Content-Type 为 application/json
        struct curl_slist* headers = nullptr;
        headers = curl_slist_append(headers, "Content-Type: application/json");
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);

        // 设置 write callback 函数处理服务器响应
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);

        // 执行请求
        CURLcode res = curl_easy_perform(curl);
        if (res != CURLE_OK) {
            std::cerr << "curl_easy_perform() failed: " << curl_easy_strerror(res) << std::endl;
            curl_easy_cleanup(curl);
            curl_slist_free_all(headers);
            curl_global_cleanup();
            return false;
        }
        // 清理 CURL 对象和 headers
        curl_easy_cleanup(curl);
        curl_slist_free_all(headers);
    } else {
        std::cerr << "Failed to initialize libcurl." << std::endl;
        curl_global_cleanup();
        return false;
    }

    // 清理 libcurl
    curl_global_cleanup();
    return true;
}
