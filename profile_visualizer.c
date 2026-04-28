#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <time.h>
#include <stdbool.h>

#include <curl/curl.h>
#include <cjson/cJSON.h>

#include "http.h"
#include "api.h"
#include "menu.h"

int main() {

    const char *api_key = getenv("STEAM_API_KEY");

    if(!api_key) {
        printf("Erro: API Key em falta.\n");
        return 1;
    }

    char steamid[18];
    int option = 0;

    printf("Insira o steamID:");
    fgets(steamid, sizeof(steamid), stdin);
    steamid[strcspn(steamid, "\n")] = '\0';

    show_menu();

    char basic_profile_url[256];
    char recent_games_url[256];
    char games_owned_url[256];
    char steam_level_url[256];
    char friends_list_url[256];

    snprintf(basic_profile_url, sizeof(basic_profile_url), "https://api.steampowered.com/ISteamUser/GetPlayerSummaries/v2/?key=%s&steamids=%s",
    api_key, steamid);

    snprintf(recent_games_url, sizeof(recent_games_url), "https://api.steampowered.com/IPlayerService/GetRecentlyPlayedGames/v1/?key=%s&steamid=%s",
    api_key, steamid);

    snprintf(games_owned_url, sizeof(games_owned_url), "https://api.steampowered.com/IPlayerService/GetOwnedGames/v1/?key=%s&steamid=%s&include_appinfo=true",
    api_key, steamid);

    snprintf(steam_level_url, sizeof(steam_level_url), "https://api.steampowered.com/IPlayerService/GetSteamLevel/v1/?key=%s&steamid=%s",
    api_key, steamid);

    snprintf(friends_list_url, sizeof(friends_list_url), "https://api.steampowered.com/ISteamUser/GetFriendList/v0001/?key=%s&steamid=%s&relationship=friend",
    api_key, steamid);

    curl_global_init(CURL_GLOBAL_ALL);
    CURL *handle =  curl_easy_init();

    struct memory profileChunk = fetch_url(handle, basic_profile_url);
    struct memory recentGamesChunk = fetch_url(handle, recent_games_url);
    struct memory ownedGamesChunk = fetch_url(handle, games_owned_url);
    struct memory levelChunk = fetch_url(handle, steam_level_url);
    struct memory friendsChunk = fetch_url(handle, friends_list_url);

    cJSON *basic_profile_json = cJSON_Parse(profileChunk.response);
    cJSON *recent_games_json = cJSON_Parse(recentGamesChunk.response);
    cJSON *profile_level_json = cJSON_Parse(levelChunk.response);
    cJSON *owned_games_json = cJSON_Parse(ownedGamesChunk.response);
    cJSON *friends_list_json = cJSON_Parse(friendsChunk.response);


    while(true) {
        
        scanf("%i", &option);
        
        if(option >= 1 && option <= 9) {
            switch (option) {
                case 1:
                    print_basic_profile(basic_profile_json);
                    show_menu();
                    break;
                case 2: 
                    print_recent_games(recent_games_json);
                    show_menu();
                    break;
                case 3:
                    print_profile_level(profile_level_json);
                    show_menu();
                    break;
                case 4:
                    print_owned_games(owned_games_json);
                    show_menu();
                    break;
                case 5: 
                    print_friends_list(friends_list_json, handle, api_key);
                    show_menu();
                    break;
                default:
                    return 0;
            }
        } else {
            show_menu();
            printf("Digite um número válido entre 1 e 9:");
        }
    }
    
    free(recentGamesChunk.response);
    free(profileChunk.response);
    free(ownedGamesChunk.response);
    free(levelChunk.response);

    cJSON_Delete(basic_profile_json);
    cJSON_Delete(recent_games_json);
    cJSON_Delete(profile_level_json);
    cJSON_Delete(owned_games_json);

    curl_easy_cleanup(handle);
    
    curl_global_cleanup();
    return 0;
}