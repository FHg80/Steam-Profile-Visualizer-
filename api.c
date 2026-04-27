#include "api.h"

void print_basic_profile(cJSON *basic_profile_json) {
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

        if(cJSON_IsString(personaname) && (personaname->valuestring != NULL)
            && cJSON_IsString(realname) && (realname->valuestring != NULL)
            && cJSON_IsString(loccountrycode) && (loccountrycode->valuestring != NULL)
            && cJSON_IsString(profileurl) && (profileurl->valuestring != NULL)
            && cJSON_IsNumber(timecreated) && (timecreated->valuedouble != 0)
            && cJSON_IsNumber(lastlogoff) && (lastlogoff->valuedouble != 0)
            && cJSON_IsNumber(personastate) && (personastate->valueint >= 0) && (personastate->valueint <= 2)) {

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
            fprintf(stderr, "Erro com os dados: %s\n", strerror(errno));
        }
            cJSON_Delete(basic_profile_json);
        } else {
            fprintf(stderr, "basic_profile_json em falta: %s\n", strerror(errno));
        }   
}

void print_recent_games(cJSON *recent_games_json) {

    if(recent_games_json) {
        const cJSON *response = cJSON_GetObjectItemCaseSensitive(recent_games_json, "response");
        const cJSON *total_count = cJSON_GetObjectItemCaseSensitive(response, "total_count");
        const cJSON *games = cJSON_GetObjectItemCaseSensitive(response, "games");
        cJSON *game = NULL;

        if(cJSON_IsNumber(total_count)) {
            printf("------------------------------\n");
            printf("Número de jogos jogados recentemente: %i\n", total_count->valueint);
            printf("------------------------------\n");
        }

        cJSON_ArrayForEach(game, games) {
            const cJSON *game_name = cJSON_GetObjectItemCaseSensitive(game, "name");
            const cJSON *playtime_forever = cJSON_GetObjectItemCaseSensitive(game, "playtime_forever");
            const cJSON *playtime_2weeks = cJSON_GetObjectItemCaseSensitive(game, "playtime_2weeks");

            if(cJSON_IsString(game_name) && (game_name->valuestring != NULL) 
            && cJSON_IsNumber(playtime_forever) && cJSON_IsNumber(playtime_2weeks)) {
                int horas_jogadas_totais = playtime_forever->valueint / 60;
                int horas_jogadas_2semanas = playtime_2weeks->valueint / 60;

                printf("Nome do Jogo: %s\n", game_name->valuestring);
                printf("Tempo jogado total: %i Hrs\n", horas_jogadas_totais);
                printf("Tempo jogado nas últimas 2 semanas: %i Hrs\n", horas_jogadas_2semanas);
                printf("------------------------------\n");
            } else {
                fprintf(stderr, "Erro com os dados: %s\n", strerror(errno));
            }   
        }
        cJSON_Delete(recent_games_json);
    } else {
        fprintf(stderr, "recent_games_json em falta: %s\n", strerror(errno));
    }
}

void print_profile_level(cJSON *profile_level_json) {
    const cJSON *response = cJSON_GetObjectItemCaseSensitive(profile_level_json, "response");
    const cJSON *player_level = cJSON_GetObjectItemCaseSensitive(response, "player_level");

    printf("------------------------------\n");
    printf("Nível do perfil: %i\n", player_level->valueint);
}

void print_owned_games(cJSON *owned_games_json) {
    const cJSON *response = cJSON_GetObjectItemCaseSensitive(owned_games_json, "response");
    const cJSON *game_count = cJSON_GetObjectItemCaseSensitive(response, "game_count");
    const cJSON *games = cJSON_GetObjectItemCaseSensitive(response, "games");
    cJSON *game = NULL;

    printf("Número de jogos possuídos: %i\n", game_count->valueint);

    cJSON_ArrayForEach(game, games) {
        const cJSON *name = cJSON_GetObjectItemCaseSensitive(game, "name");
        const cJSON *playtime_forever = cJSON_GetObjectItemCaseSensitive(game, "playtime_forever");

        int horas_jogadas = playtime_forever->valueint / 60;

        printf("------------------------------\n");
        printf("Nome: %s\n", name->valuestring); 
        printf("Total de horas jogadas: %i Hrs\n", horas_jogadas);

    }
}