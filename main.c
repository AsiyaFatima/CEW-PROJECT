#include <stdio.h>
#include <stdlib.h>
#include <curl/curl.h>
#include <cjson/cJSON.h>
#include <string.h>
#include "send_email.h"

// Callback function to handle the response data
size_t write_callback(void *ptr, size_t size, size_t nmemb, FILE *stream)
{
    return fwrite(ptr, size, nmemb, stream);
}

int main()
{
    CURL *curl;
    CURLcode res;
    FILE *file;
    char url[1000];
    int days;
    char buffer[1024];

    printf("Numbers of days between 1 - 14: ");
    scanf("%d", &days);

    // Validate user input
    if (days < 1 || days > 14) {
        printf("Invalid input. Days must be between 1 and 14.\n");
        return 1;  
    }
    // Initialize libcurl
    curl = curl_easy_init();

    if (curl)
    {
        // Set the URL of the API endpoint

        sprintf(url, "http://api.weatherapi.com/v1/forecast.json?q=karachi&days=%d&key=fc12e5b1bdd64f18aff182512240901", days);

        curl_easy_setopt(curl, CURLOPT_URL, url);

        // Open a file to write the response
        file = fopen("response.json", "w"); // "w" for write mode

        if (file == NULL)
        {
            fprintf(stderr, "Failed to open file for writing.\n");
            return 1;
        }

        // Set callback function to handle response data
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_callback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, file);

        // Perform the request
        res = curl_easy_perform(curl);

        if (res != CURLE_OK)
        {
            fprintf(stderr, "curl_easy_perform() failed: %s\n", curl_easy_strerror(res));
        }
        else
        {
            //printf("API response has been written to response.json\n", res);
            printf("API response has been written to response.json. Res: %d\n", res);
        }

        // Cleanup
        curl_easy_cleanup(curl);
        fclose(file);

        FILE *file = fopen("response.json", "r");
        if (file == NULL)
        {
            printf("Error opening file.\n");
            return 1;
        }

        // Get file size to allocate memory
        fseek(file, 0, SEEK_END);
        long fileSize = ftell(file);
        fseek(file, 0, SEEK_SET);

        // Read file content into a buffer
        char *jsonBuffer = (char *)malloc(fileSize + 1);
        fread(jsonBuffer, 1, fileSize, file);
        jsonBuffer[fileSize] = '\0'; // Null-terminate the string

        // Close the file
        fclose(file);

        // Parse JSON data
        cJSON *root = cJSON_Parse(jsonBuffer);
        if (root == NULL)
        {
            printf("Error parsing JSON.\n");
            free(jsonBuffer);
            return 1;
        }

        // Now you can work with the JSON data
        // Example: Print the parsed JSON
        char *formattedJson = cJSON_Print(root);

        cJSON *forecastObj = cJSON_GetObjectItem(root, "forecast");
        if (forecastObj == NULL || !cJSON_IsObject(forecastObj))
        {
            printf("Error retrieving forecast object.\n");
            free(jsonBuffer);
            cJSON_Delete(root);
            return 1;
        }

        // Get the "people" array
        cJSON *dayArray = cJSON_GetObjectItem(forecastObj, "forecastday");
        if (dayArray == NULL || !cJSON_IsArray(dayArray))
        {
            printf("Error retrieving days array.\n");
            free(jsonBuffer);
            cJSON_Delete(root);
            return 1;
        }

        //File handling to save the result into a file
        FILE *files = fopen("temperature_result.txt", "w");
        if (files == NULL)
        {
            printf("Error opening file.\n");
            return 1;
        }
        
        // Iterate through each object in the array
        int arraySize = cJSON_GetArraySize(dayArray);

        for (int i = 0; i < arraySize; ++i)
        {

            char result[500];

            cJSON *dayParentObj = cJSON_GetArrayItem(dayArray, i);

            // Getting day wise date
            cJSON *date = cJSON_GetObjectItem(dayParentObj, "date");

            // Access values within each object
            cJSON *day = cJSON_GetObjectItem(dayParentObj, "day");

            printf("Array - \n %s\n", cJSON_Print(day));

            cJSON *avgTemp = cJSON_GetObjectItem(day, "avgtemp_c");
            cJSON *minTemp = cJSON_GetObjectItem(day, "mintemp_c");
    	    cJSON *maxTemp = cJSON_GetObjectItem(day, "maxtemp_c");

            // Get the float value from the object
            float temperature = (float)avgTemp->valuedouble;
            float minTemperature = (float)minTemp->valuedouble;
            float maxTemperature = (float)maxTemp->valuedouble;
            
            if (temperature < 20.0)
            {
                sprintf(result,"Date: %s \nThe weather is cold. Dress warmly and enjoy a cozy day!\n", date->valuestring);
            }

            // Moderate condition: Temperature between 10 and 23 degrees Celsius
            if (temperature >= 20.0 && temperature <= 23.0)
            {
                sprintf(result,"Date: %s \nThe weather is moderate. Enjoy the pleasant temperature!\n", date->valuestring);
            } 

            // Hot condition: Temperature above 23 degrees Celsius
            if (temperature > 23.0)
            {
                sprintf(result,"Date: %s \nThe weather is hot. Stay cool and hydrated!\n", date->valuestring);
                
            }
            
            // Append minTemp and maxTemp information to the result string
    	    sprintf(result + strlen(result), "Min Temperature: %.2f\n", minTemperature);
    	    sprintf(result + strlen(result), "Max Temperature: %.2f\n", maxTemperature);
    	    sprintf(result + strlen(result), "Average Temperature: %.2f\n", temperature);

    	    // Print the Report
    	    
            printf("********REPORT********\n");
    	    printf("Date: %s\n", date->valuestring);
    	    printf("Average Temperature: %.2f\n", temperature);
    	    printf("Min Temperature: %.2f\n", minTemperature);
    	    printf("Max Temperature: %.2f\n", maxTemperature);
    	    printf("**********************\n");
    	    
            fprintf(file, "%s", result);
        }

        // Close the file
        fclose(files);
        
        printf("Results for all temperatures saved in temperature_result.txt file.\n");

        EmailConfig email;
        initEmailConfig(&email, "smtp://smtp.gmail.com:587", 587,
                        "ahsanali0332.aa@gmail.com", "ahsanali0332.aa@gmail.com", "ahsanali0332.aa@gmail.com", "elitzmgweojlnvqr");

        const char *attachment = "temperature_result.txt";
        const char *subject = "Email Subject";
        const char *body = "This is the email body content.";
        int result = sendEmail(&email, subject, body, attachment);

        if (result == CURLE_OK)
        {
            printf("Email sent successfully.\n");
            return 0;
        }
        else
        {
            fprintf(stderr, "Failed to send email: %s\n", curl_easy_strerror(result));
            return 1;
        }
        // Clean up memory
        free(jsonBuffer);
        free(formattedJson);
        cJSON_Delete(root);

        return 0;
    }
    else
    {
        fprintf(stderr, "Error initializing curl\n");
        return 1;
    }

    return 0;
}





