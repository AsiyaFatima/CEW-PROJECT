#include <stdio.h>
#include <stdlib.h>
#include <curl/curl.h>

// Callback function to handle the response data
size_t write_callback(void *ptr, size_t size, size_t nmemb, FILE *stream) {
    return fwrite(ptr, size, nmemb, stream);
}

int main() {
    CURL *curl;
    CURLcode res;
    FILE *file;
    char url[1000];
    int days;
    char city[100];

    printf("Enter a city: ");
    scanf("%99[^\n]", city);

    printf("Numbers of days between 1 - 14: ");
    scanf("%d", &days);


    // Initialize libcurl
    curl = curl_easy_init();

    if (curl) {
        // Set the URL of the API endpoint

        sprintf(url, "http://api.weatherapi.com/v1/forecast.json?q=%s&days=%d&key=89786853cbd54402a2492317231712", city, days);

        curl_easy_setopt(curl, CURLOPT_URL, url);

        // Open a file to write the response
        file = fopen("response.txt", "w"); // "w" for write mode

        if (file == NULL) {
            fprintf(stderr, "Failed to open file for writing.\n");
            return 1;
        }

        // Set callback function to handle response data
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_callback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, file);

        // Perform the request
        res = curl_easy_perform(curl);

        if (res != CURLE_OK) {
            fprintf(stderr, "curl_easy_perform() failed: %s\n", curl_easy_strerror(res));
        } else {
            printf("API response has been written to response.txt\n", res);
        }

        // Cleanup
        curl_easy_cleanup(curl);
        fclose(file);
    } else {
        fprintf(stderr, "Error initializing curl\n");
        return 1;
    }

    return 0;
}





