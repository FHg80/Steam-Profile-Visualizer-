#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <time.h>

#include <curl/curl.h>
#include <cjson/cJSON.h>

enum online_status{
    OFFLINE,
    ONLINE,
    BUSY
};


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

struct memory fetch_url(CURL *handle, const char *url) {
    struct memory chunk = {0};
    
    curl_easy_setopt(handle, CURLOPT_URL, url);
    curl_easy_setopt(handle, CURLOPT_WRITEFUNCTION, write_data);
    curl_easy_setopt(handle, CURLOPT_WRITEDATA, (void *)&chunk);
    
    curl_easy_perform(handle);

    return chunk;
}

int main() {

    const char *api_key = getenv("STEAM_API_KEY");

    if(!api_key) {
        printf("Erro: API Key em falta.\n");
        return 1;
    }

    char steamid[18];

    printf("Insira o steamID:");
    fgets(steamid, sizeof(steamid), stdin);
    steamid[strcspn(steamid, "\n")] = '\0';

    char basic_profile_url[256];
    char recent_games_url[256];

    snprintf(basic_profile_url, sizeof(basic_profile_url), "https://api.steampowered.com/ISteamUser/GetPlayerSummaries/v2/?key=%s&steamids=%s",
    api_key, steamid);

    snprintf(recent_games_url, sizeof(recent_games_url), "https://api.steampowered.com/IPlayerService/GetRecentlyPlayedGames/v1/?key=%s&steamid=%s",
    api_key, steamid);

    curl_global_init(CURL_GLOBAL_ALL);
    CURL *handle =  curl_easy_init();

    struct memory profileChunk = fetch_url(handle, basic_profile_url);
    struct memory recentGamesChunk = fetch_url(handle, recent_games_url);

    cJSON *basic_profile_json = cJSON_Parse(profileChunk.response);
    cJSON *recent_games_json = cJSON_Parse(recentGamesChunk.response);

    if(basic_profile_json) {
        const cJSON *response = cJSON_GetObjectItemCaseSensitive(basic_profile_json, "response");
        const cJSON *players = cJSON_GetObjectItemCaseSensitive(response, "players");
        const cJSON *player = cJSON_GetArrayItem(players, 0);
        const cJSON *personaname = cJSON_GetObjectItemCaseSensitive(player, "personaname");
        const cJSON *realname = cJSON_GetObjectItemCaseSensitive(player, "realname");
        const cJSON *loccountrycode = cJSON_GetObjectItemCaseSensitive(player, "loccountrycode");
        const cJSON *profileurl = cJSON_GetObjectItemCaseSensitive(player, "profileurl");
        const cJSON *timecreated = cJSON_GetObjectItemCaseSensitive(player, "timecreated");
        const cJSON *lastlogoff = cJSON_GetObjectItemCaseSensitive(player, "lastlogoff");
        const cJSON *personastate = cJSON_GetObjectItemCaseSensitive(player, "personastate");

        char tempo_criacao[80];
        char tempo_logoff[80];

        time_t t_creation = (time_t)timecreated->valuedouble;
        struct tm *tm_data_creation = localtime(&t_creation);
        strftime(tempo_criacao, sizeof(tempo_criacao), "%d/%m/%Y às %H:%M:%S", tm_data_creation);

        time_t t_logoff = (time_t)lastlogoff->valuedouble;
        struct tm *tm_data_logoff = localtime(&t_logoff);
        strftime(tempo_logoff, sizeof(tempo_logoff), "%d/%m/%Y às %H:%M:%S", tm_data_logoff);

        if(cJSON_IsString(personaname) && (personaname->valuestring != NULL)) {
            printf("Nome do perfil: %s\n", personaname->valuestring);
            printf("Nome real: %s\n", realname->valuestring);
            printf("País: %s\n", loccountrycode->valuestring);
            printf("URL do perfil: %s\n", profileurl->valuestring);
            printf("Último login em: %s\n", tempo_logoff);
            printf("Conta criada em: %s\n", tempo_criacao);

            if(personastate->valueint == OFFLINE) {
                printf("Status: Offline\n");
            } else if (personastate->valueint == ONLINE) {
                printf("Status: Online\n");
            } else if (personastate->valueint == BUSY) {
                printf("Status: Busy\n");
            }

        } else {
            printf("SLA\n");
        }
        cJSON_Delete(basic_profile_json);
    } else {
        printf("Deu pra parsear n.\n");
    }

    if(recent_games_json) {
        const cJSON *response = cJSON_GetObjectItemCaseSensitive(recent_games_json, "response");
        const cJSON *total_count = cJSON_GetObjectItemCaseSensitive(response, "total_count");
        const cJSON *games = cJSON_GetObjectItemCaseSensitive(response, "games");
        cJSON *game = NULL;

        if(cJSON_IsNumber(total_count)) {
            printf("Número de jogos jogados recentemente: %i\n", total_count->valueint);
        }

        cJSON_ArrayForEach(game, games) {
            const cJSON *game_name = cJSON_GetObjectItemCaseSensitive(game, "name");
            printf("%s\n", game_name->valuestring);
        }
    }

    free(recentGamesChunk.response);
    free(profileChunk.response);

    curl_easy_cleanup(handle);
    
    curl_global_cleanup();
    return 0;
}

