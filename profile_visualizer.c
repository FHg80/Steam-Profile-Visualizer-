#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include <curl/curl.h>
#include <cjson/cJSON.h>


struct memory {
    char *response;
    size_t size;
};

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

int main() {

    const char *api_key = getenv("STEAM_API_KEY");

    if(!api_key) {
        printf("Erro: API Key em falta.\n");
        return 1;
    }

    struct memory chunk = {0};

    char steamid[18];

    printf("Insira o steamID:");
    fgets(steamid, sizeof(steamid), stdin);
    steamid[strcspn(steamid, "\n")] = '\0';


    char url[256];

    snprintf(url, sizeof(url), "https://api.steampowered.com/ISteamUser/GetPlayerSummaries/v2/?key=%s&steamids=%s",
    api_key, steamid);

    curl_global_init(CURL_GLOBAL_ALL);
    CURL *handle =  curl_easy_init();

    if(handle) {
        curl_easy_setopt(handle, CURLOPT_URL, url);
        curl_easy_setopt(handle, CURLOPT_WRITEFUNCTION, write_data);
        curl_easy_setopt(handle, CURLOPT_WRITEDATA, (void *)&chunk);

        CURLcode result = curl_easy_perform(handle);

        if(result == CURLE_OK && chunk.response) {
            cJSON *json = cJSON_Parse(chunk.response);

            if(json) {
                const cJSON *response = cJSON_GetObjectItemCaseSensitive(json, "response");
                const cJSON *players = cJSON_GetObjectItemCaseSensitive(response, "players");
                const cJSON *player = cJSON_GetArrayItem(players, 0);
                const cJSON *personaname = cJSON_GetObjectItemCaseSensitive(player, "personaname");
                const cJSON *realname = cJSON_GetObjectItemCaseSensitive(player, "realname");
                
                if(cJSON_IsString(personaname) && (personaname->valuestring != NULL)) {
                    printf("%s\n", personaname->valuestring);
                    printf("%s\n", realname->valuestring);
                } else {
                    printf("SLA\n");
                }
                cJSON_Delete(json);
            } else {
                printf("Deu pra parsear n.\n");
            }

            free(chunk.response);
        } else {
            printf("Erro na requisicao.\n");
        }

        curl_easy_cleanup(handle);
    }

    curl_global_cleanup();
    return 0;
}