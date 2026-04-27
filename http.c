#include "http.h"

size_t write_data(char *buffer, size_t size, size_t nmemb, void *userp) {
    size_t real_size = nmemb*size;
    struct memory *mem = (struct memory *)userp;

    char *ptr = realloc(mem->response, mem->size + real_size + 1);

    mem->response = ptr;
    memcpy(&(mem->response[mem->size]), buffer, real_size);
    mem->size += real_size;
    mem->response[mem->size] = 0;

    return real_size;
}

struct memory fetch_url(CURL *handle, const char *url) {
    struct memory chunk = {0};
    
    curl_easy_setopt(handle, CURLOPT_URL, url);
    curl_easy_setopt(handle, CURLOPT_WRITEFUNCTION, write_data);
    curl_easy_setopt(handle, CURLOPT_WRITEDATA, (void *)&chunk);
    
    curl_easy_perform(handle);

    return chunk;
}