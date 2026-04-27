#pragma once

#include <stdlib.h>
#include <string.h>

#include <curl/curl.h>


struct memory {
    char *response;
    size_t size;
};

size_t write_data(char *buffer, size_t size, size_t nmemb, void *userp);

struct memory fetch_url(CURL *handle, const char *url);
