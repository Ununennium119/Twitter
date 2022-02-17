#include <stdio.h>
#include <stdbool.h>
#include <windows.h>
#include "cJSON.h"

#define LOG_IN 1
#define SIGN_UP 2
#define SEND_TWEET 3
#define RETWEET 4
#define DELETE_TWEET 5
#define REFRESH 6
#define LIKE 7
#define COMMENT 8
#define SEE_COMMENTS 9
#define SEARCH 10
#define SEARCH_TWEET 11
#define PROFILE 12
#define FOLLOW 13
#define UNFOLLOW 14
#define SET_BIO 15
#define CHANGE_PASS 16
#define LOG_OUT 17
#define WELCOME 21
#define MAIN_MENU 22
#define REFRESH_MENU 23
#define SEARCH_MENU 24
#define PROFILE_MENU 25
#define PERSONAL_AREA 26
#define TIMELINE 27

#define PORT 12345
#define IP "127.0.0.1"
#define PASS_MIN_LEN 6
#define PASS_MAX_LEN 16
#define USER_MIN_LEN 1
#define USER_MAX_LEN 16
#define TWEET_MAX_LEN 200
#define COMMENT_MAX_LEN 100
#define BIO_MAX_LEN 50
#define TAG_MAX_LEN 50

unsigned int cJSON_GetObjectSize(cJSON *);

int initialize(int port);

char *send_data(char *data);

void start();

int welcome();

int log_in();

int sign_up();

int main_menu();

int timeline();

int send_tweet();

int retweet();

int delete_tweet();

int refresh();

int refresh_menu();

int like_tweet();

int comment();

int search();

int advanced_search();

int search_tweet();

int search_menu();

int see_comments();

int profile();

int profile_menu();

int follow();

int unfollow();

int personal_area();

int set_bio();

int change_pass();

int log_out();

int s_index(const char str[], char key);

void cprint(char message[], int color);

WORD set_console_attr(int color);

struct sockaddr_in server;
char token[60];
cJSON *glob_all_comments = NULL;
char *glob_username = NULL;
bool glob_follow_status;
int glob_last_menu = MAIN_MENU;

int main() {
    if (initialize(PORT) == 0) {
        start();
    }
    return 0;
}

unsigned int cJSON_GetObjectSize(cJSON *object) {
    if (object == NULL) {
        return -1;
    }

    cJSON *item;
    unsigned int size = 0;
    cJSON_ArrayForEach(item, object) {
        size++;
    }

    return size;
}

int initialize(int port) {
    WSADATA wsadata;
    int wsaStartUp = WSAStartup(MAKEWORD(2, 2), &wsadata);
    if (wsaStartUp != 0) {
        WORD settings = set_console_attr(FOREGROUND_RED);
        printf("An error occurred. Error code: %d\n", WSAGetLastError());
        set_console_attr(settings);
        return -1;
    }

    server.sin_family = AF_INET;
    server.sin_addr.s_addr = inet_addr(IP);
    server.sin_port = htons(port);

    return 0;
}

char *send_data(char *data) {
    char *buffer = (char *) malloc(100000);
    memset(buffer, 0, 100000);

    int client_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (client_socket == INVALID_SOCKET) {
        WORD settings = set_console_attr(FOREGROUND_RED);
        printf("An error occurred. Error code: %d\n", WSAGetLastError());
        set_console_attr(settings);
        return buffer;
    }

    int can_connect = connect(client_socket, (struct sockaddr *) &server, sizeof(server));
    if (can_connect != 0) {
        WORD settings = set_console_attr(FOREGROUND_RED);
        printf("An error occurred. Error code: %d\n", WSAGetLastError());
        set_console_attr(settings);
        return buffer;
    }

    send(client_socket, data, (int) strlen(data), 0);
    recv(client_socket, buffer, 99999, 0);
    closesocket(client_socket);

    return buffer;
}

void start() {
    int func_num = WELCOME;
    while (true) {
        switch (func_num) {
            case LOG_IN: {
                func_num = log_in();
                break;
            }
            case SIGN_UP: {
                func_num = sign_up();
                break;
            }
            case SEND_TWEET: {
                func_num = send_tweet();
                break;
            }
            case RETWEET: {
                func_num = retweet();
                break;
            }
            case DELETE_TWEET: {
                func_num = delete_tweet();
                break;
            }
            case REFRESH: {
                func_num = refresh();
                break;
            }
            case LIKE: {
                func_num = like_tweet();
                break;
            }
            case COMMENT: {
                func_num = comment();
                break;
            }
            case SEE_COMMENTS: {
                func_num = see_comments();
                break;
            }
            case SEARCH: {
                func_num = advanced_search();
                break;
            }
            case SEARCH_TWEET: {
                func_num = search_tweet();
                break;
            }
            case PROFILE: {
                func_num = profile();
                break;
            }
            case FOLLOW: {
                func_num = follow();
                break;
            }
            case UNFOLLOW: {
                func_num = unfollow();
                break;
            }
            case SET_BIO: {
                func_num = set_bio();
                break;
            }
            case CHANGE_PASS: {
                func_num = change_pass();
                break;
            }
            case LOG_OUT: {
                func_num = log_out();
                break;
            }
            case WELCOME: {
                func_num = welcome();
                break;
            }
            case MAIN_MENU: {
                func_num = main_menu();
                break;
            }
            case REFRESH_MENU: {
                func_num = refresh_menu();
                break;
            }
            case SEARCH_MENU: {
                func_num = search_menu();
                break;
            }
            case PROFILE_MENU: {
                func_num = profile_menu();
                break;
            }
            case PERSONAL_AREA: {
                func_num = personal_area();
                break;
            }
            case TIMELINE: {
                func_num = timeline();
                break;
            }
        }
    }
}

int welcome() {
    printf("\n====================================================================\n");
    printf("\nLog in or Register if you don't have account:\n");
    printf("1. Log-in\n2. Sign-up\n");

    int choose;
    scanf("%d", &choose);
    fflush(stdin);

    while (choose != 1 && choose != 2) {
        cprint("Invalid input!\n", FOREGROUND_RED);
        scanf("%d", &choose);
        fflush(stdin);
    }

    switch (choose) {
        case 1: {
            return LOG_IN;
        }
        case 2: {
            return SIGN_UP;
        }
        default: {
            cprint("Invalid input!\n", FOREGROUND_RED);
            return WELCOME;
        }
    }
}

int main_menu() {
    printf("\n====================================================================\n");
    printf("\nMain Menu\n");
    printf("1. Timeline\n2. Search User\n3. Search Tweet\n4. Tweet profile\n5. Personal area\n6. Logout\n");

    int choose;
    scanf("%d", &choose);
    fflush(stdin);

    while (!(choose >= 1 && choose <= 6)) {
        cprint("Invalid input!\n", FOREGROUND_RED);
        scanf("%d", &choose);
        fflush(stdin);
    }

    switch (choose) {
        case 1: {
            return TIMELINE;
        }
        case 2: {
            return SEARCH;
        }
        case 3: {
            return SEARCH_TWEET;
        }
        case 4: {
            return PROFILE;
        }
        case 5: {
            return PERSONAL_AREA;
        }
        case 6: {
            return LOG_OUT;
        }
        default: {
            cprint("Invalid input!\n", FOREGROUND_RED);
            return MAIN_MENU;
        }
    }
}

int timeline() {
    printf("\n====================================================================\n");
    printf("\nTimeline\n");
    printf("1. Send tweet\n2. Refresh\n3. Back\n");

    int choose;
    scanf("%d", &choose);
    fflush(stdin);

    while (choose != 1 && choose != 2 && choose != 3) {
        cprint("Invalid input!\n", FOREGROUND_RED);
        scanf("%d", &choose);
        fflush(stdin);
    }

    switch (choose) {
        case 1: {
            return SEND_TWEET;
        }
        case 2: {
            return REFRESH;
        }
        case 3: {
            return MAIN_MENU;
        }
        default: {
            cprint("Invalid input!\n", FOREGROUND_RED);
            return TIMELINE;
        }
    }
}

int refresh_menu() {
    glob_last_menu = REFRESH_MENU;
    printf("\n--------------------------------------------------------------------\n");
    printf("\n1. Like\n2. Comment\n3. Retweet\n4. See Comments\n5. Back\n");
    int choose;
    scanf("%d", &choose);
    fflush(stdin);
    while (choose < 1 || choose > 5) {
        cprint("Invalid input!\n", FOREGROUND_RED);
        scanf("%d", &choose);
        fflush(stdin);
    }
    switch (choose) {
        case 1: {
            return LIKE;
        }
        case 2: {
            return COMMENT;
        }
        case 3: {
            return RETWEET;
        }
        case 4: {
            return SEE_COMMENTS;
        }
        case 5: {
            return MAIN_MENU;
        }
        default: {
            cprint("Invalid input!\n", FOREGROUND_RED);
            return REFRESH_MENU;
        }
    }
}

int search_menu() {
    glob_last_menu = SEARCH_MENU;
    printf("\n--------------------------------------------------------------------\n\n");
    if (glob_follow_status) {
        printf("1. Unfollow\n");
    } else {
        printf("1. Follow\n");
    }
    printf("2. See comments\n3. Retweet\n4. Back\n");

    int choose;
    scanf("%d", &choose);
    fflush(stdin);
    while (!(choose >= 1 && choose <= 4)) {
        cprint("Invalid input!\n", FOREGROUND_RED);
        scanf("%d", &choose);
        fflush(stdin);
    }

    switch (choose) {
        case 1:
            if (glob_follow_status) {
                return UNFOLLOW;
            } else {
                return FOLLOW;
            }
        case 2: {
            return SEE_COMMENTS;
        }
        case 3: {
            return RETWEET;
        }
        case 4: {
            return MAIN_MENU;
        }
        default: {
            cprint("Invalid input!\n", FOREGROUND_RED);
            return SEARCH_MENU;
        }
    }
}

int personal_area() {
    printf("\n====================================================================\n");
    printf("\nPersonal Area\n");
    printf("1. Set bio\n2. Change password\n3. Back\n");

    int choose;
    scanf("%d", &choose);
    fflush(stdin);
    while (choose != 1 && choose != 2 && choose != 3) {
        cprint("Invalid input!\n", FOREGROUND_RED);
        scanf("%d", &choose);
        fflush(stdin);
    }
    switch (choose) {
        case 1: {
            return SET_BIO;
        }
        case 2: {
            return CHANGE_PASS;
        }
        case 3: {
            return MAIN_MENU;
        }
        default: {
            cprint("Invalid input!\n", FOREGROUND_RED);
            return PERSONAL_AREA;
        }
    }
}

int profile_menu() {
    glob_last_menu = PROFILE_MENU;
    printf("\n--------------------------------------------------------------------\n");
    printf("\n1. See comments\n2. Delete a tweet\n3. Back\n");

    int choose;
    scanf("%d", &choose);
    fflush(stdin);
    while (choose != 1 && choose != 2 && choose != 3) {
        cprint("Invalid input!\n", FOREGROUND_RED);
        scanf("%d", &choose);
        fflush(stdin);
    }

    switch (choose) {
        case 1: {
            return SEE_COMMENTS;
        }
        case 2: {
            return DELETE_TWEET;
        }
        case 3: {
            return MAIN_MENU;
        }
        default: {
            cprint("Invalid input!\n", FOREGROUND_RED);
            return PROFILE_MENU;
        }
    }
}

int log_in() {
    char *username = (char *) malloc(USER_MAX_LEN + 1);
    memset(username, 0, USER_MAX_LEN + 1);
    char *password = (char *) malloc(PASS_MAX_LEN + 1);
    memset(password, 0, USER_MAX_LEN + 1);
    char *format = (char *) malloc(100 * sizeof(char));

    printf("\nLog in to your account\n");
    printf("Username:\n");
    sprintf(format, "%%%d[^\n]s", USER_MAX_LEN);
    scanf(format, username);
    fflush(stdin);

    printf("\nPassword:\n");
    sprintf(format, "%%%d[^\n]s", PASS_MAX_LEN);
    scanf(format, password);
    fflush(stdin);
    free(format);
    printf("\n");

    cJSON *json_request = cJSON_CreateObject();
    cJSON_AddStringToObject(json_request, "type", "login");
    cJSON_AddStringToObject(json_request, "username", username);
    cJSON_AddStringToObject(json_request, "password", password);
    char *request = cJSON_PrintUnformatted(json_request);

    char *server_reply;
    server_reply = send_data(request);

    free(username);
    free(password);
    free(request);
    if (strcmp(server_reply, "\0") == 0) {
        free(server_reply);
        return WELCOME;
    }

    cJSON *root = cJSON_Parse(server_reply);
    cJSON *type = cJSON_GetObjectItem(root, "type");
    cJSON *message = cJSON_GetObjectItem(root, "message");

    if (strcmp(type->valuestring, "Error") == 0) {
        WORD settings = set_console_attr(FOREGROUND_RED);
        printf("%s\n", message->valuestring);
        set_console_attr(settings);

        free(server_reply);
        cJSON_Delete(root);
        return WELCOME;
    } else if (strcmp(type->valuestring, "Token") == 0) {
        strcpy(token, message->valuestring);
        cprint("Logged in successfully!\n", FOREGROUND_GREEN);

        free(server_reply);
        cJSON_Delete(root);
        return MAIN_MENU;
    } else {
        cprint("Something went wrong!\n", FOREGROUND_RED);

        free(server_reply);
        cJSON_Delete(root);
        return WELCOME;
    }
}

int sign_up() {
    char *username = (char *) malloc(USER_MAX_LEN + 2);
    memset(username, 0, USER_MAX_LEN + 2);
    char *password = (char *) malloc(PASS_MAX_LEN + 2);
    memset(username, 0, PASS_MAX_LEN + 2);
    char *format = (char *) malloc(100 * sizeof(char));

    printf("\nCreate an account\n");
    printf("Username: (min length: %d, max length: %d)\n", USER_MIN_LEN, USER_MAX_LEN);
    sprintf(format, "%%%d[^\n]s", USER_MAX_LEN + 1);
    scanf(format, username);
    fflush(stdin);

    printf("\nPassword: (min length: %d, max length: %d)\n", PASS_MIN_LEN, PASS_MAX_LEN);
    sprintf(format, "%%%d[^\n]s", PASS_MAX_LEN + 1);
    scanf(format, password);
    fflush(stdin);
    free(format);
    printf("\n");

    if (strlen(username) > USER_MAX_LEN) {
        WORD settings = set_console_attr(FOREGROUND_RED);
        printf("Maximum length of username is %d characters!\n", USER_MAX_LEN);
        set_console_attr(settings);
        return WELCOME;
    } else if (strlen(password) > PASS_MAX_LEN) {
        WORD settings = set_console_attr(FOREGROUND_RED);
        printf("Maximum length of password is %d characters!\n", PASS_MAX_LEN);
        set_console_attr(settings);
        return WELCOME;
    } else if (strlen(username) < USER_MIN_LEN) {
        WORD settings = set_console_attr(FOREGROUND_RED);
        printf("Minimum length of username is %d characters!\n", USER_MIN_LEN);
        set_console_attr(settings);
        return WELCOME;
    } else if (strlen(password) < PASS_MIN_LEN) {
        WORD settings = set_console_attr(FOREGROUND_RED);
        printf("Minimum length of password is %d characters!\n", PASS_MIN_LEN);
        set_console_attr(settings);
        return WELCOME;
    } else if (s_index(username, ' ') != -1 || s_index(password, ' ') != -1) {
        cprint("Username and password can't contain whitespace!\n", FOREGROUND_RED);
        return WELCOME;
    } else {
        cJSON *json_request = cJSON_CreateObject();
        cJSON_AddStringToObject(json_request, "type", "signup");
        cJSON_AddStringToObject(json_request, "username", username);
        cJSON_AddStringToObject(json_request, "password", password);
        char *request = cJSON_PrintUnformatted(json_request);

        free(username);
        free(password);
        cJSON_Delete(json_request);

        char *server_reply;
        server_reply = send_data(request);
        free(request);
        if (strcmp(server_reply, "\0") == 0) {
            free(server_reply);
            return WELCOME;
        }

        cJSON *root = cJSON_Parse(server_reply);
        free(server_reply);
        cJSON *type = cJSON_GetObjectItem(root, "type");
        cJSON *message = cJSON_GetObjectItem(root, "message");

        if (strcmp(type->valuestring, "Error") == 0) {
            WORD settings = set_console_attr(FOREGROUND_RED);
            printf("%s\n", message->valuestring);
            set_console_attr(settings);

            cJSON_Delete(root);
            return WELCOME;
        } else if (strcmp(type->valuestring, "Successful") == 0) {
            WORD settings = set_console_attr(FOREGROUND_GREEN);
            printf("%s\n", message->valuestring);
            set_console_attr(settings);

            cJSON_Delete(root);
            return LOG_IN;
        } else {
            cprint("Something went wrong!\n", FOREGROUND_RED);

            cJSON_Delete(root);
            return WELCOME;
        }
    }
}

int send_tweet() {
    char *tweet = (char *) malloc(TWEET_MAX_LEN + 2);
    memset(tweet, 0, TWEET_MAX_LEN + 2);
    char *format = (char *) malloc(100);

    printf("\nEnter your tweet: (max length: %d)\n", TWEET_MAX_LEN);
    sprintf(format, "%%%d[^\n]s", TWEET_MAX_LEN + 1);
    scanf(format, tweet);
    fflush(stdin);
    printf("\n");

    if (strlen(tweet) > TWEET_MAX_LEN) {
        WORD settings = set_console_attr(FOREGROUND_RED);
        printf("Maximum length of tweet is %d characters!\n", TWEET_MAX_LEN);
        set_console_attr(settings);
        return TIMELINE;
    } else if (strlen(tweet) == 0) {
        cprint("Please enter something!\n", FOREGROUND_RED);
        return TIMELINE;
    } else {
        cJSON *json_request = cJSON_CreateObject();
        cJSON_AddStringToObject(json_request, "type", "send tweet");
        cJSON_AddStringToObject(json_request, "token", token);
        cJSON_AddStringToObject(json_request, "tweet", tweet);
        char *request = cJSON_PrintUnformatted(json_request);
        free(tweet);
        cJSON_Delete(json_request);

        char *server_reply;
        server_reply = send_data(request);
        free(request);
        if (strcmp(server_reply, "\0") == 0) {
            free(server_reply);
            return TIMELINE;
        }

        cJSON *root = cJSON_Parse(server_reply);
        cJSON *type = cJSON_GetObjectItem(root, "type");
        cJSON *message = cJSON_GetObjectItem(root, "message");

        if (strcmp(type->valuestring, "Error") == 0) {
            WORD settings = set_console_attr(FOREGROUND_RED);
            printf("%s\n", message->valuestring);
            set_console_attr(settings);
        } else if (strcmp(type->valuestring, "Successful") == 0) {
            WORD settings = set_console_attr(FOREGROUND_GREEN);
            printf("%s\n", message->valuestring);
            set_console_attr(settings);
        } else {
            cprint("Something went wrong!\n", FOREGROUND_RED);
        }

        free(server_reply);
        cJSON_Delete(root);
        return TIMELINE;
    }
}

int retweet() {
    int tweet_id;

    printf("\nLike a tweet\n");
    printf("Tweet id:\n");
    scanf("%d", &tweet_id);
    fflush(stdin);
    printf("\n");

    cJSON *json_request = cJSON_CreateObject();
    cJSON_AddStringToObject(json_request, "type", "retweet");
    cJSON_AddStringToObject(json_request, "token", token);
    cJSON_AddNumberToObject(json_request, "id", tweet_id);
    char *request = cJSON_PrintUnformatted(json_request);
    cJSON_Delete(json_request);

    char *server_reply;
    server_reply = send_data(request);
    free(request);
    if (strcmp(server_reply, "\0") == 0) {
        free(server_reply);
        return glob_last_menu;
    }

    cJSON *root = cJSON_Parse(server_reply);
    cJSON *type = cJSON_GetObjectItem(root, "type");
    cJSON *message = cJSON_GetObjectItem(root, "message");

    if (strcmp(type->valuestring, "Error") == 0) {
        WORD settings = set_console_attr(FOREGROUND_RED);
        printf("%s\n", message->valuestring);
        set_console_attr(settings);
    } else if (strcmp(type->valuestring, "Successful") == 0) {
        WORD settings = set_console_attr(FOREGROUND_GREEN);
        printf("%s\n", message->valuestring);
        set_console_attr(settings);
    } else {
        cprint("Something went wrong!\n", FOREGROUND_RED);
    }

    free(server_reply);
    cJSON_Delete(root);
    return glob_last_menu;
}

int delete_tweet() {
    int tweet_id;

    printf("\nDelete a tweet\n");
    printf("Tweet id:\n");
    scanf("%d", &tweet_id);
    fflush(stdin);
    printf("\n");

    cJSON *json_request = cJSON_CreateObject();
    cJSON_AddStringToObject(json_request, "type", "delete tweet");
    cJSON_AddStringToObject(json_request, "token", token);
    cJSON_AddNumberToObject(json_request, "id", tweet_id);
    char *request = cJSON_PrintUnformatted(json_request);
    cJSON_Delete(json_request);

    char *server_reply;
    server_reply = send_data(request);
    free(request);
    if (strcmp(server_reply, "\0") == 0) {
        free(server_reply);
        return PROFILE_MENU;
    }

    cJSON *root = cJSON_Parse(server_reply);
    cJSON *type = cJSON_GetObjectItem(root, "type");
    cJSON *message = cJSON_GetObjectItem(root, "message");

    if (strcmp(type->valuestring, "Error") == 0) {
        WORD settings = set_console_attr(FOREGROUND_RED);
        printf("%s\n\n", message->valuestring);
        set_console_attr(settings);
    } else if (strcmp(type->valuestring, "Successful") == 0) {
        WORD settings = set_console_attr(FOREGROUND_GREEN);
        printf("%s\n\n", message->valuestring);
        set_console_attr(settings);
    } else {
        cprint("Something went wrong!\n\n", FOREGROUND_RED);
    }

    free(server_reply);
    cJSON_Delete(root);
    return PROFILE_MENU;
}

int refresh() {
    cJSON *json_request = cJSON_CreateObject();
    cJSON_AddStringToObject(json_request, "type", "refresh");
    cJSON_AddStringToObject(json_request, "token", token);
    char *request = cJSON_PrintUnformatted(json_request);
    cJSON_Delete(json_request);

    char *server_reply;
    server_reply = send_data(request);
    free(request);
    if (strcmp(server_reply, "\0") == 0) {
        free(server_reply);
        return TIMELINE;
    }

    cJSON *root = cJSON_Parse(server_reply);
    cJSON *type = cJSON_GetObjectItem(root, "type");
    cJSON *message = cJSON_GetObjectItem(root, "message");

    if (strcmp(type->valuestring, "List") == 0) {
        printf("\n");
        int size = cJSON_GetArraySize(message);
        if (size == 0) {
            cprint("No new tweet!\n", FOREGROUND_RED);
            return TIMELINE;
        }
        printf("\n--------------------------------------------------------------------\n");
        cJSON *comments = cJSON_CreateObject();
        for (int i = 0; i < size; i++) {
            cJSON *elem = cJSON_GetArrayItem(message, i);
            cJSON *id = cJSON_GetObjectItem(elem, "id");
            cJSON *author = cJSON_GetObjectItem(elem, "author");
            cJSON *source = cJSON_GetObjectItem(elem, "source");
            cJSON *content = cJSON_GetObjectItem(elem, "content");
            cJSON *all_comments = cJSON_GetObjectItem(elem, "comments");
            cJSON *likes = cJSON_GetObjectItem(elem, "likes");

            int comments_count = 0;
            cJSON *curt_comment = NULL;
            cJSON *tweet_comments = cJSON_CreateArray();
            cJSON_ArrayForEach(curt_comment, all_comments) {
                cJSON *comment = cJSON_Parse(cJSON_PrintUnformatted(curt_comment));
                cJSON_AddItemToArray(tweet_comments, comment);
                comments_count++;
            }
            char *str_id = (char *) malloc(20 * sizeof(char));
            sprintf(str_id, "%d", id->valueint);
            cJSON_AddItemToObject(comments, str_id, tweet_comments);

            printf("\n%s (%s)\tTweet id: %d\n", author->valuestring, source->valuestring, id->valueint);
            printf("%s\n", content->valuestring);
            printf("Likes: %d, Comments: %d\n", cJSON_GetArraySize(likes), comments_count);
        }
        if (glob_all_comments != NULL) {
            cJSON_Delete(glob_all_comments);
        }
        glob_all_comments = comments;

        free(server_reply);
        cJSON_Delete(root);
        return REFRESH_MENU;
    } else if (strcmp(type->valuestring, "Error") == 0) {
        WORD settings = set_console_attr(FOREGROUND_RED);
        printf("%s\n", message->valuestring);
        set_console_attr(settings);

        free(server_reply);
        cJSON_Delete(root);
        return TIMELINE;
    } else {
        cprint("Something went wrong!\n", FOREGROUND_RED);

        free(server_reply);
        cJSON_Delete(root);
        return TIMELINE;
    }
}

int like_tweet() {
    int tweet_id;

    printf("\nLike a tweet\n");
    printf("Tweet id:\n");
    scanf("%d", &tweet_id);
    fflush(stdin);
    printf("\n");

    cJSON *json_request = cJSON_CreateObject();
    cJSON_AddStringToObject(json_request, "type", "like");
    cJSON_AddStringToObject(json_request, "token", token);
    cJSON_AddNumberToObject(json_request, "id", tweet_id);
    char *request = cJSON_PrintUnformatted(json_request);
    cJSON_Delete(json_request);

    char *server_reply;
    server_reply = send_data(request);
    free(request);
    if (strcmp(server_reply, "\0") == 0) {
        free(server_reply);
        return REFRESH_MENU;
    }

    cJSON *root = cJSON_Parse(server_reply);
    cJSON *type = cJSON_GetObjectItem(root, "type");
    cJSON *message = cJSON_GetObjectItem(root, "message");

    if (strcmp(type->valuestring, "Error") == 0) {
        WORD settings = set_console_attr(FOREGROUND_RED);
        printf("%s\n", message->valuestring);
        set_console_attr(settings);
    } else if (strcmp(type->valuestring, "Successful") == 0) {
        WORD settings = set_console_attr(FOREGROUND_GREEN);
        printf("%s\n", message->valuestring);
        set_console_attr(settings);
    } else {
        cprint("Something went wrong!\n", FOREGROUND_RED);
    }

    free(server_reply);
    cJSON_Delete(root);
    return REFRESH_MENU;
}

int comment() {
    int tweet_id;
    char *comment = (char *) malloc(COMMENT_MAX_LEN + 2);
    memset(comment, 0, COMMENT_MAX_LEN + 2);

    char *format = (char *) malloc(20 * sizeof(char));

    printf("\nComment\n");
    printf("Tweet id:\n");
    scanf("%d", &tweet_id);
    fflush(stdin);

    printf("Your comment: (max length: %d)\n", COMMENT_MAX_LEN);
    sprintf(format, "%%%d[^\n]s", COMMENT_MAX_LEN + 1);
    scanf(format, comment);
    fflush(stdin);
    free(format);
    printf("\n");

    if (strlen(comment) > COMMENT_MAX_LEN) {
        WORD settings = set_console_attr(FOREGROUND_RED);
        printf("Maximum length of comment is %d characters!\n", COMMENT_MAX_LEN);
        set_console_attr(settings);
        return REFRESH_MENU;
    } else if (strlen(comment) == 0) {
        cprint("Please enter something!\n", FOREGROUND_RED);
        return REFRESH_MENU;
    } else {
        cJSON *json_request = cJSON_CreateObject();
        cJSON_AddStringToObject(json_request, "type", "comment");
        cJSON_AddStringToObject(json_request, "token", token);
        cJSON_AddNumberToObject(json_request, "id", tweet_id);
        cJSON_AddStringToObject(json_request, "comment", comment);
        char *request = cJSON_PrintUnformatted(json_request);
        free(comment);
        cJSON_Delete(json_request);

        char *server_reply;
        server_reply = send_data(request);
        free(request);
        if (strcmp(server_reply, "\0") == 0) {
            free(server_reply);
            return REFRESH_MENU;
        }

        cJSON *root = cJSON_Parse(server_reply);
        cJSON *type = cJSON_GetObjectItem(root, "type");
        cJSON *message = cJSON_GetObjectItem(root, "message");

        if (strcmp(type->valuestring, "Error") == 0) {
            WORD settings = set_console_attr(FOREGROUND_RED);
            printf("%s\n\n", message->valuestring);
            set_console_attr(settings);
        } else if (strcmp(type->valuestring, "Successful") == 0) {
            WORD settings = set_console_attr(FOREGROUND_GREEN);
            printf("%s\n\n", message->valuestring);
            set_console_attr(settings);
        } else {
            cprint("Something went wrong!\n\n", FOREGROUND_RED);
        }

        free(server_reply);
        return REFRESH_MENU;
    }
}

int search() {
    char *username = (char *) malloc(USER_MAX_LEN + 2);
    memset(username, 0, USER_MAX_LEN + 2);

    printf("Enter username to search:\n");
    scanf("%s", username);
    fflush(stdin);
    printf("\n");

    cJSON *json_request = cJSON_CreateObject();
    cJSON_AddStringToObject(json_request, "type", "search");
    cJSON_AddStringToObject(json_request, "token", token);
    cJSON_AddStringToObject(json_request, "username", username);
    char *request = cJSON_PrintUnformatted(json_request);
    cJSON_Delete(json_request);
    free(username);

    char *server_reply;
    server_reply = send_data(request);
    free(request);
    if (strcmp(server_reply, "\0") == 0) {
        free(server_reply);
        return MAIN_MENU;
    }

    cJSON *root = cJSON_Parse(server_reply);
    cJSON *type = cJSON_GetObjectItem(root, "type");
    cJSON *message = cJSON_GetObjectItem(root, "message");

    if (strcmp(type->valuestring, "Profile") == 0) {
        cJSON *name = cJSON_GetObjectItem(message, "username");
        cJSON *bio = cJSON_GetObjectItem(message, "bio");
        cJSON *followers = cJSON_GetObjectItem(message, "numberofFollowers");
        cJSON *followings = cJSON_GetObjectItem(message, "numberofFollowings");
        cJSON *follow_status = cJSON_GetObjectItem(message, "followStatus");
        cJSON *tweets = cJSON_GetObjectItem(message, "allTweets");

        printf("\nUsername: %s\n", name->valuestring);
        printf("bio: %s\n", bio->valuestring);
        printf("Followers: %d\tFollowings: %d\n", followers->valueint, followings->valueint);
        printf("%s\n\n", follow_status->valuestring);

        cJSON *comments = cJSON_CreateObject();
        int tweets_count = cJSON_GetArraySize(tweets);
        for (int i = 0; i < tweets_count; i++) {
            cJSON *elem = cJSON_GetArrayItem(tweets, i);
            cJSON *id = cJSON_GetObjectItem(elem, "id");
            cJSON *source = cJSON_GetObjectItem(elem, "source");
            cJSON *content = cJSON_GetObjectItem(elem, "content");
            cJSON *all_comments = cJSON_GetObjectItem(elem, "comments");
            cJSON *likes = cJSON_GetObjectItem(elem, "likes");

            int comments_count = 0;
            cJSON *curt_comment = NULL;
            cJSON *tweet_comments = cJSON_CreateArray();
            cJSON_ArrayForEach(curt_comment, all_comments) {
                cJSON *comment = cJSON_Parse(cJSON_PrintUnformatted(curt_comment));
                cJSON_AddItemToArray(tweet_comments, comment);
                comments_count++;
            }
            char *str_id = (char *) malloc(20 * sizeof(char));
            sprintf(str_id, "%d", id->valueint);
            cJSON_AddItemToObject(comments, str_id, tweet_comments);

            if (source->valuestring[0] != '\0') {
                printf("(%s)\n", source->valuestring);
            }
            printf("%s\n", content->valuestring);
            printf("Likes: %d, Comments: %d, Tweet id: %d\n\n", cJSON_GetArraySize(likes), comments_count,
                   id->valueint);
        }
        if (glob_all_comments != NULL) {
            cJSON_Delete(glob_all_comments);
        }
        glob_all_comments = comments;

        char *user_name = (char *) malloc((USER_MAX_LEN + 1) * sizeof(char));
        strcpy(user_name, name->valuestring);
        if (glob_username != NULL) {
            free(glob_username);
        }
        glob_username = user_name;
        if (strcmp(follow_status->valuestring, "Followed") == 0) {
            glob_follow_status = true;
        } else {
            glob_follow_status = false;
        }

        free(server_reply);
        cJSON_Delete(root);
        return SEARCH_MENU;
    } else if (strcmp(type->valuestring, "Error") == 0) {
        WORD settings = set_console_attr(FOREGROUND_RED);
        printf("%s\n\n", message->valuestring);
        set_console_attr(settings);

        free(server_reply);
        cJSON_Delete(root);
        return MAIN_MENU;
    } else {
        cprint("Something went wrong!\n\n", FOREGROUND_RED);

        free(server_reply);
        cJSON_Delete(root);
        return MAIN_MENU;
    }
}

int advanced_search() {
    char *pattern = (char *) malloc(USER_MAX_LEN + 2);
    memset(pattern, 0, USER_MAX_LEN + 2);

    printf("\nSearch User\n");
    printf("Enter a pattern to search:\n");
    scanf("%s", pattern);
    fflush(stdin);
    printf("\n");

    cJSON *json_request = cJSON_CreateObject();
    cJSON_AddStringToObject(json_request, "type", "search username");
    cJSON_AddStringToObject(json_request, "token", token);
    cJSON_AddStringToObject(json_request, "pattern", pattern);
    char *request = cJSON_PrintUnformatted(json_request);
    cJSON_Delete(json_request);
    free(pattern);

    char *server_reply;
    server_reply = send_data(request);
    free(request);
    if (strcmp(server_reply, "\0") == 0) {
        free(server_reply);
        return MAIN_MENU;
    }

    cJSON *response = cJSON_Parse(server_reply);
    cJSON *type = cJSON_GetObjectItem(response, "type");
    cJSON *message = cJSON_GetObjectItem(response, "message");

    if (strcmp(type->valuestring, "List") == 0) {
        int list_size = cJSON_GetArraySize(message);
        if (list_size == 0) {
            printf("No match found!\n\n");
            cJSON_Delete(response);
            free(server_reply);
            return MAIN_MENU;
        } else {
            printf("Users list:\n");
            for (int i = 0; i < list_size; i++) {
                printf("%d. %s\n", i + 1, cJSON_GetArrayItem(message, i)->valuestring);
            }

            int username_id;
            printf("\nUsername num:\n");
            scanf("%d", &username_id);
            fflush(stdin);
            while (username_id < 1 || username_id > list_size) {
                cprint("\nInvalid input!\n", FOREGROUND_RED);
                scanf("%d", &username_id);
                fflush(stdin);
            }

            json_request = cJSON_CreateObject();
            cJSON_AddStringToObject(json_request, "type", "search user");
            cJSON_AddStringToObject(json_request, "token", token);
            cJSON_AddNumberToObject(json_request, "id", username_id - 1);
            request = cJSON_PrintUnformatted(json_request);
            cJSON_Delete(json_request);

            server_reply = send_data(request);
            free(request);
            if (strcmp(server_reply, "\0") == 0) {
                free(server_reply);
                return MAIN_MENU;
            }

            cJSON *root = cJSON_Parse(server_reply);
            type = cJSON_GetObjectItem(root, "type");
            message = cJSON_GetObjectItem(root, "message");

            if (strcmp(type->valuestring, "Profile") == 0) {
                cJSON *name = cJSON_GetObjectItem(message, "username");
                cJSON *bio = cJSON_GetObjectItem(message, "bio");
                cJSON *followers = cJSON_GetObjectItem(message, "numberofFollowers");
                cJSON *followings = cJSON_GetObjectItem(message, "numberofFollowings");
                cJSON *follow_status = cJSON_GetObjectItem(message, "followStatus");
                cJSON *tweets = cJSON_GetObjectItem(message, "allTweets");

                printf("\n--------------------------------------------------------------------\n");
                printf("\nUsername: %s\n", name->valuestring);
                printf("bio: %s\n", bio->valuestring);
                printf("Followers: %d\tFollowings: %d\n", followers->valueint, followings->valueint);
                printf("%s\n", follow_status->valuestring);
                printf("\nTweets:\n");

                cJSON *comments = cJSON_CreateObject();
                int tweets_count = cJSON_GetArraySize(tweets);
                for (int i = 0; i < tweets_count; i++) {
                    cJSON *elem = cJSON_GetArrayItem(tweets, i);
                    cJSON *id = cJSON_GetObjectItem(elem, "id");
                    cJSON *source = cJSON_GetObjectItem(elem, "source");
                    cJSON *content = cJSON_GetObjectItem(elem, "content");
                    cJSON *all_comments = cJSON_GetObjectItem(elem, "comments");
                    cJSON *likes = cJSON_GetObjectItem(elem, "likes");

                    int comments_count = 0;
                    cJSON *curt_comment = NULL;
                    cJSON *tweet_comments = cJSON_CreateArray();
                    cJSON_ArrayForEach(curt_comment, all_comments) {
                        cJSON *comment = cJSON_Parse(cJSON_PrintUnformatted(curt_comment));
                        cJSON_AddItemToArray(tweet_comments, comment);
                        comments_count++;
                    }
                    char *str_id = (char *) malloc(20 * sizeof(char));
                    sprintf(str_id, "%d", id->valueint);
                    cJSON_AddItemToObject(comments, str_id, tweet_comments);

                    printf("\n");
                    if (source->valuestring[0] != '\0') {
                        printf("(%s)\n", source->valuestring);
                    }
                    printf("%s\n", content->valuestring);
                    printf("Likes: %d, Comments: %d, Tweet id: %d\n", cJSON_GetArraySize(likes), comments_count,
                           id->valueint);
                }
                if (glob_all_comments != NULL) {
                    cJSON_Delete(glob_all_comments);
                }
                glob_all_comments = comments;

                char *user_name = (char *) malloc((USER_MAX_LEN + 1) * sizeof(char));
                strcpy(user_name, name->valuestring);
                if (glob_username != NULL) {
                    free(glob_username);
                }
                glob_username = user_name;
                if (strcmp(follow_status->valuestring, "Followed") == 0) {
                    glob_follow_status = true;
                } else {
                    glob_follow_status = false;
                }

                free(server_reply);
                cJSON_Delete(root);
                return SEARCH_MENU;
            } else if (strcmp(type->valuestring, "Error") == 0) {
                WORD settings = set_console_attr(FOREGROUND_RED);
                printf("%s\n\n", message->valuestring);
                set_console_attr(settings);

                free(server_reply);
                cJSON_Delete(root);
                return MAIN_MENU;
            } else {
                cprint("Something went wrong!\n\n", FOREGROUND_RED);

                free(server_reply);
                cJSON_Delete(root);
                return MAIN_MENU;
            }
        }
    } else if (strcmp(type->valuestring, "Error") == 0) {
        WORD settings = set_console_attr(FOREGROUND_RED);
        printf("%s\n", message->valuestring);
        set_console_attr(settings);

        free(server_reply);
        cJSON_Delete(response);
        return MAIN_MENU;
    } else {
        cprint("Something went wrong!\n", FOREGROUND_RED);

        free(server_reply);
        cJSON_Delete(response);
        return MAIN_MENU;
    }
}

int search_tweet() {
    char *tag = (char *) malloc(TAG_MAX_LEN + 2);
    memset(tag, 0, TAG_MAX_LEN + 2);
    char *format = malloc(100 * sizeof(char));

    printf("\nSearch Tweet\n");
    printf("Enter tag to search: (max length: %d)\n", TAG_MAX_LEN);
    sprintf(format, "%%%d[^\n]s", TAG_MAX_LEN + 1);
    scanf(format, tag);
    fflush(stdin);
    free(format);

    cJSON *json_request = cJSON_CreateObject();
    cJSON_AddStringToObject(json_request, "type", "search tweet");
    cJSON_AddStringToObject(json_request, "token", token);
    cJSON_AddStringToObject(json_request, "tag", tag);
    char *request = cJSON_PrintUnformatted(json_request);
    cJSON_Delete(json_request);
    free(tag);

    char *server_reply;
    server_reply = send_data(request);
    free(request);
    if (strcmp(server_reply, "\0") == 0) {
        free(server_reply);
        return MAIN_MENU;
    }

    cJSON *root = cJSON_Parse(server_reply);
    cJSON *type = cJSON_GetObjectItem(root, "type");
    cJSON *tweets = cJSON_GetObjectItem(root, "message");

    if (strcmp(type->valuestring, "List") == 0) {
        cJSON *comments = cJSON_CreateObject();
        int tweets_count = cJSON_GetArraySize(tweets);
        if (tweets_count == 0) {
            printf("No tweet found!\n");
            return MAIN_MENU;
        } else {
            printf("\n--------------------------------------------------------------------\n");
            for (int i = 0; i < tweets_count; i++) {
                cJSON *elem = cJSON_GetArrayItem(tweets, i);
                cJSON *author = cJSON_GetObjectItem(elem, "author");
                cJSON *id = cJSON_GetObjectItem(elem, "id");
                cJSON *source = cJSON_GetObjectItem(elem, "source");
                cJSON *content = cJSON_GetObjectItem(elem, "content");
                cJSON *all_comments = cJSON_GetObjectItem(elem, "comments");
                cJSON *likes = cJSON_GetObjectItem(elem, "likes");

                int comments_count = 0;
                cJSON *curt_comment = NULL;
                cJSON *tweet_comments = cJSON_CreateArray();
                cJSON_ArrayForEach(curt_comment, all_comments) {
                    cJSON *comment = cJSON_Parse(cJSON_PrintUnformatted(curt_comment));
                    cJSON_AddItemToArray(tweet_comments, comment);
                    comments_count++;
                }
                char *str_id = (char *) malloc(20 * sizeof(char));
                sprintf(str_id, "%d", id->valueint);
                cJSON_AddItemToObject(comments, str_id, tweet_comments);

                printf("\n%s ", author->valuestring);
                if (source->valuestring[0] != '\0') {
                    printf("(%s)", source->valuestring);
                }
                printf("\tTweet id: %d\n", id->valueint);
                printf("%s\n", content->valuestring);
                printf("Likes: %d, Comments: %d\n", cJSON_GetArraySize(likes), comments_count);
            }
        }
        if (glob_all_comments != NULL) {
            cJSON_Delete(glob_all_comments);
        }
        glob_all_comments = comments;

        free(server_reply);
        cJSON_Delete(root);
        return REFRESH_MENU;
    } else if (strcmp(type->valuestring, "Error") == 0) {
        WORD settings = set_console_attr(FOREGROUND_RED);
        printf("%s\n", tweets->valuestring);
        set_console_attr(settings);

        free(server_reply);
        cJSON_Delete(root);
        return MAIN_MENU;
    } else {
        cprint("Something went wrong!\n", FOREGROUND_RED);

        free(server_reply);
        cJSON_Delete(root);
        return MAIN_MENU;
    }
}

int profile() {
    cJSON *json_request = cJSON_CreateObject();
    cJSON_AddStringToObject(json_request, "type", "profile");
    cJSON_AddStringToObject(json_request, "token", token);
    char *request = cJSON_PrintUnformatted(json_request);
    cJSON_Delete(json_request);

    char *server_reply;
    server_reply = send_data(request);
    free(request);
    if (strcmp(server_reply, "\0") == 0) {
        free(server_reply);
        return MAIN_MENU;
    }
    cJSON *root = cJSON_Parse(server_reply);
    cJSON *type = cJSON_GetObjectItem(root, "type");
    cJSON *message = cJSON_GetObjectItem(root, "message");

    if (strcmp(type->valuestring, "Profile") == 0) {
        cJSON *username = cJSON_GetObjectItem(message, "username");
        cJSON *bio = cJSON_GetObjectItem(message, "bio");
        cJSON *followers = cJSON_GetObjectItem(message, "numberofFollowers");
        cJSON *followings = cJSON_GetObjectItem(message, "numberofFollowings");
        cJSON *tweets = cJSON_GetObjectItem(message, "allTweets");

        printf("\n--------------------------------------------------------------------\n");
        printf("\nUsername: %s\n", username->valuestring);
        printf("bio: %s\n", bio->valuestring);
        printf("Followers: %d\tFollowings: %d\n", followers->valueint, followings->valueint);

        cJSON *comments = cJSON_CreateObject();
        int tweets_count = cJSON_GetArraySize(tweets);
        for (int i = 0; i < tweets_count; i++) {
            cJSON *elem = cJSON_GetArrayItem(tweets, i);
            cJSON *id = cJSON_GetObjectItem(elem, "id");
            cJSON *source = cJSON_GetObjectItem(elem, "source");
            cJSON *author = cJSON_GetObjectItem(elem, "author");
            cJSON *content = cJSON_GetObjectItem(elem, "content");
            cJSON *all_comments = cJSON_GetObjectItem(elem, "comments");
            cJSON *likes = cJSON_GetObjectItem(elem, "likes");

            int comments_count = 0;
            cJSON *curt_comment = NULL;
            cJSON *tweet_comments = cJSON_CreateArray();
            cJSON_ArrayForEach(curt_comment, all_comments) {
                cJSON *comment = cJSON_Parse(cJSON_PrintUnformatted(curt_comment));
                cJSON_AddItemToArray(tweet_comments, comment);
                comments_count++;
            }
            char *str_id = (char *) malloc(20 * sizeof(char));
            sprintf(str_id, "%d", id->valueint);
            cJSON_AddItemToObject(comments, str_id, tweet_comments);

            printf("\n");
            if (source->valuestring[0] != '\0') {
                printf("(%s)\n", source->valuestring);
            }
            printf("%s\n", content->valuestring);
            printf("Likes: %d, Comments: %d, Tweet id: %d\n", likes->valueint, comments_count, id->valueint);
        }
        if (glob_all_comments != NULL) {
            cJSON_Delete(glob_all_comments);
        }
        glob_all_comments = comments;

        free(server_reply);
        cJSON_Delete(root);
        return PROFILE_MENU;
    } else if (strcmp(type->valuestring, "Error") == 0) {
        WORD settings = set_console_attr(FOREGROUND_RED);
        printf("%s\n", message->valuestring);
        set_console_attr(settings);

        free(server_reply);
        cJSON_Delete(root);
        return MAIN_MENU;
    } else {
        cprint("Something went wrong!\n", FOREGROUND_RED);

        free(server_reply);
        cJSON_Delete(root);
        return MAIN_MENU;
    }
}

int see_comments() {
    char *tweet_id = (char *) malloc(20);
    memset(tweet_id, 0, 20);

    printf("\nSee Comments\n");
    printf("Tweet id:\n");
    scanf("%s", tweet_id);
    fflush(stdin);
    printf("\n");

    cJSON *tweet_comments = cJSON_GetObjectItem(glob_all_comments, tweet_id);
    if (tweet_comments == NULL) {
        cprint("Invalid tweet id!\n", FOREGROUND_RED);
        return glob_last_menu;
    }
    if (cJSON_GetObjectSize(tweet_comments) > 0) {
        cJSON *comment = NULL;
        printf("Tweet #%s comments:\n", tweet_id);
        cJSON_ArrayForEach(comment, tweet_comments) {
            cJSON *name = cJSON_GetObjectItem(comment, "name");
            cJSON *content = cJSON_GetObjectItem(comment, "content");
            printf("%s: %s\n", name->valuestring, content->valuestring);
        }
    } else {
        printf("No comment!\n");
    }

    return glob_last_menu;
}

int unfollow() {
    cJSON *json_request = cJSON_CreateObject();
    cJSON_AddStringToObject(json_request, "type", "unfollow");
    cJSON_AddStringToObject(json_request, "token", token);
    cJSON_AddStringToObject(json_request, "username", glob_username);
    char *request = cJSON_PrintUnformatted(json_request);
    cJSON_Delete(json_request);

    char *server_reply;
    server_reply = send_data(request);
    free(request);
    if (strcmp(server_reply, "\0") == 0) {
        free(server_reply);
        return MAIN_MENU;
    }

    cJSON *root = cJSON_Parse(server_reply);
    cJSON *type = cJSON_GetObjectItem(root, "type");
    cJSON *message = cJSON_GetObjectItem(root, "message");

    if (strcmp(type->valuestring, "Error") == 0) {
        WORD settings = set_console_attr(FOREGROUND_RED);
        printf("%s\n", message->valuestring);
        set_console_attr(settings);
    } else if (strcmp(type->valuestring, "Successful") == 0) {
        WORD settings = set_console_attr(FOREGROUND_GREEN);
        printf("%s\n", message->valuestring);
        set_console_attr(settings);
    } else {
        cprint("Something went wrong!\n", FOREGROUND_RED);
    }

    free(server_reply);
    cJSON_Delete(root);
    return MAIN_MENU;
}

int follow() {
    cJSON *json_request = cJSON_CreateObject();
    cJSON_AddStringToObject(json_request, "type", "follow");
    cJSON_AddStringToObject(json_request, "token", token);
    cJSON_AddStringToObject(json_request, "username", glob_username);
    char *request = cJSON_PrintUnformatted(json_request);
    cJSON_Delete(json_request);

    char *server_reply;
    server_reply = send_data(request);
    free(request);
    if (strcmp(server_reply, "\0") == 0) {
        free(server_reply);
        return MAIN_MENU;
    }
    cJSON *root = cJSON_Parse(server_reply);
    cJSON *type = cJSON_GetObjectItem(root, "type");
    cJSON *message = cJSON_GetObjectItem(root, "message");

    if (strcmp(type->valuestring, "Error") == 0) {
        WORD settings = set_console_attr(FOREGROUND_RED);
        printf("%s\n", message->valuestring);
        set_console_attr(settings);
    } else if (strcmp(type->valuestring, "Successful") == 0) {
        WORD settings = set_console_attr(FOREGROUND_GREEN);
        printf("%s\n", message->valuestring);
        set_console_attr(settings);
    } else {
        cprint("Something went wrong!\n", FOREGROUND_RED);
    }

    free(server_reply);
    cJSON_Delete(root);
    return MAIN_MENU;
}

int set_bio() {
    char *bio = (char *) malloc(BIO_MAX_LEN + 2);
    memset(bio, 0, BIO_MAX_LEN + 2);
    char *format = (char *) malloc(20);
    sprintf(format, "%%%d[^\n]s", BIO_MAX_LEN + 1);

    printf("\nSet Bio\n");
    printf("bio: (max length: %d)\n", BIO_MAX_LEN);
    scanf(format, bio);
    fflush(stdin);
    free(format);
    printf("\n");

    if (strlen(bio) > BIO_MAX_LEN) {
        WORD settings = set_console_attr(FOREGROUND_RED);
        printf("Maximum length of bio is %d characters!\n", BIO_MAX_LEN);
        set_console_attr(settings);
        return PERSONAL_AREA;
    } else if (strlen(bio) == 0) {
        cprint("Please enter something!\n", FOREGROUND_RED);
        return PERSONAL_AREA;
    } else {
        cJSON *json_request = cJSON_CreateObject();
        cJSON_AddStringToObject(json_request, "type", "set bio");
        cJSON_AddStringToObject(json_request, "token", token);
        cJSON_AddStringToObject(json_request, "bio", bio);
        char *request = cJSON_PrintUnformatted(json_request);
        cJSON_Delete(json_request);
        free(bio);

        char *server_reply;
        server_reply = send_data(request);
        free(request);
        if (strcmp(server_reply, "\0") == 0) {
            free(server_reply);
            return PERSONAL_AREA;
        }

        cJSON *root = cJSON_Parse(server_reply);
        cJSON *type = cJSON_GetObjectItem(root, "type");
        cJSON *message = cJSON_GetObjectItem(root, "message");

        if (strcmp(type->valuestring, "Error") == 0) {
            WORD settings = set_console_attr(FOREGROUND_RED);
            printf("%s\n", message->valuestring);
            set_console_attr(settings);
        } else if (strcmp(type->valuestring, "Successful") == 0) {
            WORD settings = set_console_attr(FOREGROUND_GREEN);
            printf("%s\n", message->valuestring);
            set_console_attr(settings);
        } else {
            cprint("Something went wrong!\n", FOREGROUND_RED);
        }

        free(server_reply);
        cJSON_Delete(root);
        return PERSONAL_AREA;
    }
}

int change_pass() {
    char *curt_pass = (char *) malloc(PASS_MAX_LEN + 1);
    memset(curt_pass, 0, PASS_MAX_LEN + 1);
    char *new_pass = (char *) malloc(PASS_MAX_LEN + 2);
    memset(new_pass, 0, PASS_MAX_LEN + 2);
    char *format = (char *) malloc(20);

    printf("\nChange password\n");
    printf("Current password:\n");
    scanf("%s", curt_pass);
    fflush(stdin);

    printf("New password: (max length: %d)\n", PASS_MAX_LEN);
    sprintf(format, "%%%d[^\n]s", PASS_MAX_LEN + 1);
    scanf(format, new_pass);
    fflush(stdin);
    free(format);
    printf("\n");

    if (strlen(new_pass) > PASS_MAX_LEN) {
        WORD settings = set_console_attr(FOREGROUND_RED);
        printf("Maximum length of password is %d characters!\n", PASS_MAX_LEN);
        set_console_attr(settings);
        return PERSONAL_AREA;
    } else if (strlen(new_pass) < PASS_MIN_LEN) {
        WORD settings = set_console_attr(FOREGROUND_RED);
        printf("Minimum length of password is %d characters!\n", PASS_MIN_LEN);
        set_console_attr(settings);
        return PERSONAL_AREA;
    } else if (s_index(new_pass, ' ') != -1) {
        cprint("Password can't contain whitespace!\n", FOREGROUND_RED);
        return PERSONAL_AREA;
    } else {
        cJSON *json_request = cJSON_CreateObject();
        cJSON_AddStringToObject(json_request, "type", "change password");
        cJSON_AddStringToObject(json_request, "token", token);
        cJSON_AddStringToObject(json_request, "old_pass", curt_pass);
        cJSON_AddStringToObject(json_request, "new_pass", new_pass);
        char *request = cJSON_PrintUnformatted(json_request);
        cJSON_Delete(json_request);
        free(curt_pass);
        free(new_pass);

        char *server_reply;
        server_reply = send_data(request);
        free(request);
        if (strcmp(server_reply, "\0") == 0) {
            free(server_reply);
            return PERSONAL_AREA;
        }

        cJSON *root = cJSON_Parse(server_reply);
        cJSON *type = cJSON_GetObjectItem(root, "type");
        cJSON *message = cJSON_GetObjectItem(root, "message");

        if (strcmp(type->valuestring, "Error") == 0) {
            WORD settings = set_console_attr(FOREGROUND_RED);
            printf("%s\n", message->valuestring);
            set_console_attr(settings);
        } else if (strcmp(type->valuestring, "Successful") == 0) {
            WORD settings = set_console_attr(FOREGROUND_GREEN);
            printf("%s\n", message->valuestring);
            set_console_attr(settings);
        } else {
            cprint("Something went wrong!\n", FOREGROUND_RED);
        }

        free(server_reply);
        cJSON_Delete(root);
        return PERSONAL_AREA;
    }
}

int log_out() {
    cJSON *json_request = cJSON_CreateObject();
    cJSON_AddStringToObject(json_request, "type", "logout");
    cJSON_AddStringToObject(json_request, "token", token);
    char *request = cJSON_PrintUnformatted(json_request);

    char *server_reply;
    server_reply = send_data(request);
    free(request);
    if (strcmp(server_reply, "\0") == 0) {
        free(server_reply);
        return MAIN_MENU;
    }

    cJSON *root = cJSON_Parse(server_reply);
    cJSON *type = cJSON_GetObjectItem(root, "type");
    cJSON *message = cJSON_GetObjectItem(root, "message");

    if (strcmp(type->valuestring, "Error") == 0) {
        WORD settings = set_console_attr(FOREGROUND_RED);
        printf("\n%s\n", message->valuestring);
        set_console_attr(settings);

        free(server_reply);
        cJSON_Delete(root);
        return MAIN_MENU;
    } else if (strcmp(type->valuestring, "Successful") == 0) {
        cprint("\nLogged out successfully!\n", FOREGROUND_GREEN);

        free(server_reply);
        cJSON_Delete(root);
        return WELCOME;
    } else {
        cprint("\nSomething went wrong!\n", FOREGROUND_RED);

        free(server_reply);
        cJSON_Delete(root);
        return MAIN_MENU;
    }
}

int s_index(const char str[], char key) {
    for (int i = 0; str[i] != '\0'; i++) {
        if (str[i] == key) {
            return i;
        }
    }
    return -1;
}

void cprint(char message[], int color) {
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    CONSOLE_SCREEN_BUFFER_INFO consoleInfo;
    WORD saved_attributes;

    GetConsoleScreenBufferInfo(hConsole, &consoleInfo);
    saved_attributes = consoleInfo.wAttributes;

    SetConsoleTextAttribute(hConsole, color);
    printf("%s", message);
    SetConsoleTextAttribute(hConsole, saved_attributes);
}

WORD set_console_attr(int color) {
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    CONSOLE_SCREEN_BUFFER_INFO consoleInfo;
    WORD saved_attributes;

    GetConsoleScreenBufferInfo(hConsole, &consoleInfo);
    saved_attributes = consoleInfo.wAttributes;

    SetConsoleTextAttribute(hConsole, color);
    return saved_attributes;
}
