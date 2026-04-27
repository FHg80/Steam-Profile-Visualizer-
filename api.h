#pragma once

#include <stdio.h>
#include <time.h>

#include <cjson/cJSON.h>

enum online_status{
    OFFLINE,
    ONLINE,
    BUSY
};

void print_basic_profile(cJSON *basic_profile_json);
void print_recent_games(cJSON *recent_games_json);
void print_profile_level(cJSON *profile_level_json);
void print_owned_games(cJSON *owned_games_json);