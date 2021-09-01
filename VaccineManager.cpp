#include <iostream>
#include <ostream>
#include <string>
#include <string_view>
#include <map>
#include "curl_easy.h"
#include "curl_form.h"
#include "curl_ios.h"
#include "curl_exception.h"
#include <stdio.h>
#include <string.h>
#include <curl/curl.h>
using namespace std;

#define FROM    "<fromid>"
#define TO      "<toid>"

static const char* payload_text[] = {
  "To: " TO "\r\n",
  "From: " FROM "\r\n",
  "Subject: Registration Successful\r\n",
  "\r\n", /* empty line to divide headers from body, see RFC5322 */
  "Thanks For Trying us.\r\n",
  "\r\n",
  "I hope you complete.\r\n",
  "\r\n",
  "1) Give Feedback.\r\n",
  "\r\n",
  "2) blaa.\r\n",
  "\r\n",

  "**Sent from Vaccine Managament App**\r\n",
  NULL
};

struct upload_status {
    int lines_read;
};

static size_t payload_source(void* ptr, size_t size, size_t nmemb, void* userp)
{
    struct upload_status* upload_ctx = (struct upload_status*)userp;
    const char* data;

    if ((size == 0) || (nmemb == 0) || ((size * nmemb) < 1)) {
        return 0;
    }

    data = payload_text[upload_ctx->lines_read];

    if (data) {
        size_t len = strlen(data);
        memcpy(ptr, data, len);
        upload_ctx->lines_read++;
        return len;
    }

    return 0;
}

void email(string ml)
{
    ml.insert(0, "<");
    ml += '>';
    cout << "We are sending you email at   * " << ml << " * " << endl;
    const char* p_data = ml.data();
    CURL* curl;
    CURLcode res = CURLE_OK;
    struct curl_slist* recipients = NULL;
    struct upload_status upload_ctx;

    upload_ctx.lines_read = 0;

    curl = curl_easy_init();
    if (curl) {
        curl_easy_setopt(curl, CURLOPT_USERNAME, "fromid");
        curl_easy_setopt(curl, CURLOPT_PASSWORD, "password fro account");
        curl_easy_setopt(curl, CURLOPT_URL, "smtp://smtp.gmail.com:587");
        curl_easy_setopt(curl, CURLOPT_USE_SSL, (long)CURLUSESSL_ALL);
        curl_easy_setopt(curl, CURLOPT_CAINFO, "google.pem");
        curl_easy_setopt(curl, CURLOPT_MAIL_FROM, FROM);
        recipients = curl_slist_append(recipients, p_data);
        curl_easy_setopt(curl, CURLOPT_MAIL_RCPT, recipients);
        curl_easy_setopt(curl, CURLOPT_READFUNCTION, payload_source);
        curl_easy_setopt(curl, CURLOPT_READDATA, &upload_ctx);
        curl_easy_setopt(curl, CURLOPT_UPLOAD, 1L);
        res = curl_easy_perform(curl);
        if (res != CURLE_OK)
        {
            cout << "Email not sent\n";
            fprintf(stderr, "%s\n",
                curl_easy_strerror(res));  // if curl eas perform fails .
        }
        else
            cout << "\nEmail Sent. Kindly Check Your Inbox!!!!!!!!!!!" << endl;

        curl_slist_free_all(recipients);

        curl_easy_cleanup(curl);
    }
}
int main(void)
{
    string ml;
    cout << "Enter your email(pls give legit):";
    cin >> ml;
    email(ml);
    
    return 0;
}