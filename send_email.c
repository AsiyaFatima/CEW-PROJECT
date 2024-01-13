#include <stdio.h>
#include <curl/curl.h>
#include "send_email.h"


void initEmailConfig(EmailConfig *email, const char *server, int port, const char *sender,
                     const char *recipient, const char *user, const char *pass) {
    email->smtp_server = (char *)server;
    email->smtp_port = port;
    email->sender_email = (char *)sender;
    email->recipient_email = (char *)recipient;
    email->username = (char *)user;
    email->password = (char *)pass;
}

int sendEmail(EmailConfig *email, const char *subject, const char *body, const char *attachment_file) {
    CURL *curl;
    CURLcode res = CURLE_OK;
    struct curl_slist *recipients = NULL;

    curl = curl_easy_init();
    if (curl) {
        curl_easy_setopt(curl, CURLOPT_URL, email->smtp_server);

        curl_easy_setopt(curl, CURLOPT_MAIL_FROM, email->sender_email);

        recipients = curl_slist_append(recipients, email->recipient_email);
        curl_easy_setopt(curl, CURLOPT_MAIL_RCPT, recipients);

        curl_easy_setopt(curl, CURLOPT_USERNAME, email->username);
        curl_easy_setopt(curl, CURLOPT_PASSWORD, email->password);

        struct curl_slist *headers = NULL;
        headers = curl_slist_append(headers, "Content-Type: text/plain;");
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);

        struct curl_slist *mime_parts = NULL;
        mime_parts = curl_slist_append(mime_parts, "Subject: ");
        mime_parts = curl_slist_append(mime_parts, subject);
        mime_parts = curl_slist_append(mime_parts, "Content-Disposition: attachment; filename=\"temperature_result.txt\"");
        mime_parts = curl_slist_append(mime_parts, "Content-Type: text/plain;");

        struct curl_httppost *formpost = NULL;
        struct curl_httppost *lastptr = NULL;
	struct curl_forms form[] = {
    	   CURLFORM_COPYNAME, "subject",
           CURLFORM_COPYCONTENTS, subject,
           CURLFORM_COPYNAME, "body",
           CURLFORM_COPYCONTENTS, body,
           CURLFORM_COPYNAME, "file",
           CURLFORM_FILE, attachment_file,
           CURLFORM_END
      };

        curl_formadd(&formpost, &lastptr, CURLFORM_COPYNAME, "from", CURLFORM_COPYCONTENTS, email->sender_email, CURLFORM_END);

        curl_easy_setopt(curl, CURLOPT_MIMEPOST, formpost);
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, mime_parts);

        res = curl_easy_perform(curl);

        curl_slist_free_all(recipients);
        curl_slist_free_all(headers);
        curl_slist_free_all(mime_parts);
        curl_easy_cleanup(curl);
    }

    return (int)res;
}
