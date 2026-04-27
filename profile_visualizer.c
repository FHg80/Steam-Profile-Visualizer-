#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <time.h>

#include <curl/curl.h>
#include <cjson/cJSON.h>

#include "http.h"
#include "api.h"

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
    char games_owned_url[256];
    char steam_level_url[256];

    snprintf(basic_profile_url, sizeof(basic_profile_url), "https://api.steampowered.com/ISteamUser/GetPlayerSummaries/v2/?key=%s&steamids=%s",
    api_key, steamid);

    snprintf(recent_games_url, sizeof(recent_games_url), "https://api.steampowered.com/IPlayerService/GetRecentlyPlayedGames/v1/?key=%s&steamid=%s",
    api_key, steamid);

    snprintf(games_owned_url, sizeof(games_owned_url), "https://api.steampowered.com/IPlayerService/GetOwnedGames/v1/?key=%s&steamid=%s&include_appinfo=true",
    api_key, steamid);

    snprintf(steam_level_url, sizeof(steam_level_url), "https://api.steampowered.com/IPlayerService/GetSteamLevel/v1/?key=%s&steamid=%s",
    api_key, steamid);

    curl_global_init(CURL_GLOBAL_ALL);
    CURL *handle =  curl_easy_init();

    struct memory profileChunk = fetch_url(handle, basic_profile_url);
    struct memory recentGamesChunk = fetch_url(handle, recent_games_url);
    struct memory ownedGamesChunk = fetch_url(handle, games_owned_url);
    struct memory levelChunk = fetch_url(handle, steam_level_url);

    cJSON *basic_profile_json = cJSON_Parse(profileChunk.response);
    cJSON *recent_games_json = cJSON_Parse(recentGamesChunk.response);
    cJSON *profile_level_json = cJSON_Parse(levelChunk.response);
    cJSON *owned_games_json = cJSON_Parse(ownedGamesChunk.response);

    print_basic_profile(basic_profile_json);
    print_recent_games(recent_games_json);
    print_profile_level(profile_level_json);
    print_owned_games(owned_games_json);

    free(recentGamesChunk.response);
    free(profileChunk.response);
    free(ownedGamesChunk.response);
    free(levelChunk.response);

    curl_easy_cleanup(handle);
    
    curl_global_cleanup();
    return 0;
}