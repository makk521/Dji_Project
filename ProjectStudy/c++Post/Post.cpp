#include <iostream>
#include <curl/curl.h>

int main() {
    // 初始化libcurl
    CURL *curl = curl_easy_init();
    if (curl) {
        // 设置目标URL
        curl_easy_setopt(curl, CURLOPT_URL, "http://172.21.2.6:8000/ma");

        // 设置HTTP请求类型为POST
        curl_easy_setopt(curl, CURLOPT_POST, 1L);

        // 设置POST数据
        curl_easy_setopt(curl, CURLOPT_POSTFIELDS, "");

        // 执行HTTP请求
        CURLcode res = curl_easy_perform(curl);

        // 检查请求是否成功
        if (res != CURLE_OK) {
            std::cerr << "Failed to send POST request: " << curl_easy_strerror(res) << std::endl;
        }

        // 清理资源
        curl_easy_cleanup(curl);
    }

    return 0;
}
