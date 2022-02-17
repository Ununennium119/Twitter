#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>
#include <string.h>
#include <windows.h>
#include <dirent.h>
#include <errno.h>
#include "cJSON.h"

#define CHECK(item)                 \
if (item == NULL) {                 \
    cJSON_Delete(json_response);    \
    return bad_request_format();    \
}

#define PASS_MIN_LEN 6
#define PASS_MAX_LEN 16
#define USER_MIN_LEN 1
#define USER_MAX_LEN 16
#define TWEET_MAX_LEN 200
#define BIO_MAX_LEN 50
#define TAG_MAX_LEN 50
#define TOKEN_SIZE 32

void cJSON_DeleteStringFromArrayByElem(cJSON *, char *);

unsigned int cJSON_GetObjectSize(cJSON *);

int s_size(const char *);

int s_index(const char *, char);

int s_find(const char *, const char *, int);

bool is_file_exist(char *);

int remove_files_in_dir(char *);

int create_database();

char *get_response(char *);

int initialize(int);

void start(int);

char *create_token(char *);

int update_user(cJSON *);

int update_tweet(cJSON *);

char *get_username_by_token(char *);

cJSON *get_user_by_username(char *);

cJSON *get_usernames_by_pattern(char *);

cJSON *get_tweet_by_id(int);

cJSON *get_tweets_by_author(char *);

cJSON *get_tweets_by_tag(char *);

void remove_retweet_id(int);

int get_last_tweet_id();

void increment_last_tweet_id();

bool is_followed(char *, char *);

bool is_tweet_seen(int, char *);

bool is_username_unique(char *);

bool authenticate(char *, char *);

bool is_user_active(char *);

char *sign_up(cJSON *);

char *log_in(cJSON *);

char *log_out(cJSON *);

char *set_bio(cJSON *);

char *change_pass(cJSON *);

char *search_user_by_username(cJSON *, bool);

char *search_username_by_pattern(cJSON *);

char *advanced_user_search(cJSON *);

char *search_tweet_by_tag(cJSON *);

char *follow(cJSON *);

char *unfollow(cJSON *);

char *send_tweet(cJSON *);

char *retweet(cJSON *);

char *delete_tweet(cJSON *);

char *send_comment(cJSON *);

char *refresh(cJSON *);

char *like_tweet(cJSON *);

char *bad_request_format();

const char charset[] = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQURSTUWXYZ0123456789!@#$%^&*";
cJSON *glob_username_list = NULL;

int main() {
    if (create_database() == 0) {
        int server_socket = initialize(12345);
        if (server_socket != INVALID_SOCKET) {
            start(server_socket);
        }
    } else {
        printf("Could not create database!\n");
    }

    return 0;
}

void cJSON_DeleteStringFromArrayByElem(cJSON *array, char *item) {
    int array_size = cJSON_GetArraySize(array);
    for (int i = 0; i < array_size; i++) {
        if (strcmp(cJSON_GetArrayItem(array, i)->valuestring, item) == 0) {
            cJSON_DeleteItemFromArray(array, i);
            break;
        }
    }
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

int s_size(const char *str) {
    int size = 0;
    while (1) {
        if (str[size] == '\0') {
            break;
        }
        size++;
    }
    return size;
}

int s_index(const char *str, char key) {
    for (int i = 0; str[i] != '\0'; i++) {
        if (str[i] == key) {
            return i;
        }
    }
    return -1;
}

int s_find(const char *str, const char *word, int start) {
    int size_str = s_size(str);
    int size_word = s_size(word);
    short int status = 0;
    int start_index, current_index = 0, found_count = 0;
    for (int i = start; i < size_str; i++) {
        if (str[i] == word[current_index]) {
            if (status == 0) {
                start_index = i;
            }
            status = 1;
            found_count++;
            current_index++;
            if (found_count == size_word) {
                return start_index;
            }
        } else if (status == 1) {
            status = 0;
            i = start_index + 1;
            current_index = 0;
            found_count = 0;
        }
    }
    return -1;
}

bool is_file_exist(char *address) {
    FILE *file;
    if ((file = fopen(address, "r"))) {
        fclose(file);
        return true;
    }

    return false;
}

int remove_files_in_dir(char *dir_address) {
    DIR *dir;
    struct dirent *entry;
    char *address = (char *) malloc(1000 * sizeof(char));

    dir = opendir(dir_address);
    while ((entry = readdir(dir)) != NULL) {
        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0) {
            continue;
        }

        sprintf(address, "%s\\%s", dir_address, entry->d_name);
        if (remove(address) != 0) {
            return -1;
        }
    }
    closedir(dir);
    free(address);

    return 0;
}

int create_database() {
    // Create database dir if not exist
    int dir_res;
    dir_res = _mkdir("..\\Database");
    if (!(dir_res == 0 || errno == EEXIST)) {
        return -1;
    }
    dir_res = _mkdir("..\\Database\\Users");
    if (!(dir_res == 0 || errno == EEXIST)) {
        return -1;
    }
    dir_res = _mkdir("..\\Database\\Tweets");
    if (!(dir_res == 0 || errno == EEXIST)) {
        return -1;
    }
    dir_res = _mkdir("..\\Database\\Active Users");
    if (!(dir_res == 0 || errno == EEXIST)) {
        return -1;
    }
    if (!is_file_exist("..\\Database\\last_id.txt")) {
        FILE *last_id = fopen("..\\Database\\last_id.txt", "w");
        if (last_id == NULL) {
            return -1;
        }
        fputs("-1", last_id);
        fclose(last_id);
    }
    if (remove_files_in_dir("..\\Database\\Active Users") != 0) {
        return -1;
    }

    return 0;
}

char *get_response(char *buffer) {
    cJSON *request = cJSON_Parse(buffer);
    free(buffer);

    if (request == NULL) {
        return bad_request_format();
    }
    cJSON *type = cJSON_GetObjectItem(request, "type");
    if (type == NULL) {
        return bad_request_format();
    }

    if (strcmp(type->valuestring, "signup") == 0) {
        return sign_up(request);
    } else if (strcmp(type->valuestring, "login") == 0) {
        return log_in(request);
    } else if (strcmp(type->valuestring, "logout") == 0) {
        return log_out(request);
    } else if (strcmp(type->valuestring, "send tweet") == 0) {
        return send_tweet(request);
    } else if (strcmp(type->valuestring, "retweet") == 0) {
        return retweet(request);
    } else if (strcmp(type->valuestring, "delete tweet") == 0) {
        return delete_tweet(request);
    } else if (strcmp(type->valuestring, "like") == 0) {
        return like_tweet(request);
    } else if (strcmp(type->valuestring, "refresh") == 0) {
        return refresh(request);
    } else if (strcmp(type->valuestring, "search") == 0) {
        return search_user_by_username(request, false);
    } else if (strcmp(type->valuestring, "search username") == 0) {
        return search_username_by_pattern(request);
    } else if (strcmp(type->valuestring, "search user") == 0) {
        return advanced_user_search(request);
    } else if (strcmp(type->valuestring, "search tweet") == 0) {
        return search_tweet_by_tag(request);
    } else if (strcmp(type->valuestring, "follow") == 0) {
        return follow(request);
    } else if (strcmp(type->valuestring, "unfollow") == 0) {
        return unfollow(request);
    } else if (strcmp(type->valuestring, "set bio") == 0) {
        return set_bio(request);
    } else if (strcmp(type->valuestring, "change password") == 0) {
        return change_pass(request);
    } else if (strcmp(type->valuestring, "comment") == 0) {
        return send_comment(request);
    } else if (strcmp(type->valuestring, "profile") == 0) {
        return search_user_by_username(request, true);
    } else {
        return bad_request_format();
    }
    cJSON_Delete(request);
}

int initialize(int port) {
    // Start WSA
    WSADATA wsadata;
    int wsaStartUp = WSAStartup(MAKEWORD(2, 2), &wsadata);
    if (wsaStartUp != 0) {
        printf("Failed to run WSA. Error code: %d\n", WSAGetLastError());
        return INVALID_SOCKET;
    }
    printf("WSA started up...\n");

    // Create server socket
    int server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (server_socket == INVALID_SOCKET) {
        printf("Failed to create socket. Error code: %d\n", WSAGetLastError());
        return INVALID_SOCKET;
    }
    printf("Server socket created...\n");

    struct sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
    server_addr.sin_port = htons(port);

    int bind_res = bind(server_socket, (struct sockaddr *) &server_addr, sizeof(server_addr));
    if (bind_res == SOCKET_ERROR) {
        printf("Bind failed. Error code: %d\n", WSAGetLastError());
        return INVALID_SOCKET;
    }
    printf("Socket bound...\n");

    int listen_res = listen(server_socket, SOMAXCONN);
    if (listen_res == SOCKET_ERROR) {
        printf("Listen failed. Error code: %d\n", WSAGetLastError());
        return INVALID_SOCKET;
    }
    printf("Listening...\n");

    return server_socket;
}

void start(int server_socket) {
    int len = sizeof(struct sockaddr_in);
    int client_socket;
    struct sockaddr_in client_addr;
    char *response = NULL;

    while ((client_socket = accept(server_socket, (struct sockaddr *) &client_addr, &len)) != INVALID_SOCKET) {
        printf("\nClient connected...\n");

        char *buffer = (char *) malloc(1000 * sizeof(char));
        memset(buffer, 0, 1000);

        recv(client_socket, buffer, 999, 0);
        printf("Request: %s\n", buffer);

        response = get_response(buffer);
        send(client_socket, response, (int) strlen(response), 0);

        closesocket(client_socket);
        if (response != NULL) {
            free(response);
        }
    }
}

char *create_token(char *username) {
    char *token = (char *) malloc((USER_MAX_LEN + TOKEN_SIZE + 1) * sizeof(char));
    memset(token, 0, (USER_MAX_LEN + TOKEN_SIZE + 1));
    char random_str[TOKEN_SIZE + 1] = "";

    srand(clock());
    for (int i = 0; i < TOKEN_SIZE; i++) {
        random_str[i] = charset[rand() % strlen(charset)];
    }
    sprintf(token, "%s%s", username, random_str);

    return token;
}

int update_user(cJSON *user) {
    cJSON *username = cJSON_GetObjectItem(user, "username");

    char *address = (char *) malloc(100 * sizeof(char));
    sprintf(address, "..\\Database\\Users\\%s.user.json", username->valuestring);
    FILE *user_file = fopen(address, "w");
    free(address);

    if (user_file == NULL) {
        return -1;
    }

    char *content = cJSON_PrintUnformatted(user);
    fputs(content, user_file);
    free(content);
    fclose(user_file);

    return 0;
}

int update_tweet(cJSON *tweet) {
    cJSON *id = cJSON_GetObjectItem(tweet, "id");

    char *address = (char *) malloc(100 * sizeof(char));
    sprintf(address, "..\\Database\\Tweets\\#%d.tweet.json", id->valueint);
    FILE *tweet_file = fopen(address, "w");
    free(address);
    if (tweet_file == NULL) {
        return -1;
    }

    char *content = cJSON_PrintUnformatted(tweet);
    fputs(content, tweet_file);
    fclose(tweet_file);

    return 0;
}

char *get_username_by_token(char *token) {
    char *username = (char *) malloc((USER_MAX_LEN + 1) * sizeof(char));
    memset(username, 0, (USER_MAX_LEN + 1));

    DIR *users_dir;
    users_dir = opendir("..\\Database\\Active Users");
    if (users_dir == NULL) {
        return username;
    }

    char *curt_user = (char *) malloc(100000 * sizeof(char));
    char *address = (char *) malloc(100 * sizeof(char));
    struct dirent *entry;
    while ((entry = readdir(users_dir)) != NULL) {
        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0) {
            continue;
        }

        sprintf(address, "..\\Database\\Active Users\\%s", entry->d_name);
        FILE *curt_user_file = fopen(address, "r");
        if (curt_user_file == NULL) {
            free(address);
            free(curt_user);
            continue;
        }
        fgets(curt_user, 99999, curt_user_file);
        fclose(curt_user_file);

        cJSON *curt_user_json = cJSON_Parse(curt_user);
        cJSON *curt_user_token = cJSON_GetObjectItem(curt_user_json, "token");
        if (strcmp(curt_user_token->valuestring, token) == 0) {
            cJSON *curt_username = cJSON_GetObjectItem(curt_user_json, "username");
            strcpy(username, curt_username->valuestring);
            cJSON_Delete(curt_user_json);
            break;
        }
        cJSON_Delete(curt_user_json);
    }
    closedir(users_dir);
    free(address);
    free(curt_user);

    return username;
}

cJSON *get_user_by_username(char *username) {
    cJSON *user = NULL;

    DIR *users_dir;
    users_dir = opendir("..\\Database\\Users");
    if (users_dir == NULL) {
        return user;
    }

    char *curt_user = (char *) malloc(100000 * sizeof(char));
    char *address = (char *) malloc(100 * sizeof(char));
    struct dirent *entry;
    while ((entry = readdir(users_dir)) != NULL) {
        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0) {
            continue;
        }

        sprintf(address, "..\\Database\\Users\\%s", entry->d_name);
        FILE *curt_user_file = fopen(address, "r");
        if (curt_user_file == NULL) {
            free(curt_user);
            continue;
        }
        fgets(curt_user, 99999, curt_user_file);
        fclose(curt_user_file);

        cJSON *curt_user_json = cJSON_Parse(curt_user);
        cJSON *curt_username = cJSON_GetObjectItem(curt_user_json, "username");
        if (strcmp(curt_username->valuestring, username) == 0) {
            user = curt_user_json;
            break;
        }
        cJSON_Delete(curt_user_json);
    }
    closedir(users_dir);
    free(address);
    free(curt_user);

    return user;
}

cJSON *get_usernames_by_pattern(char *pattern) {
    cJSON *usernames_list = cJSON_CreateArray();

    DIR *users_dir;
    users_dir = opendir("..\\Database\\Users");

    char *curt_user = (char *) malloc(100000 * sizeof(char));
    char *address = (char *) malloc(100 * sizeof(char));
    struct dirent *entry;
    while ((entry = readdir(users_dir)) != NULL) {
        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0) {
            continue;
        }

        sprintf(address, "..\\Database\\Users\\%s", entry->d_name);
        FILE *curt_user_file = fopen(address, "r");
        if (curt_user_file == NULL) {
            free(curt_user);
            continue;
        }
        fgets(curt_user, 99999, curt_user_file);
        fclose(curt_user_file);

        cJSON *curt_user_json = cJSON_Parse(curt_user);
        cJSON *curt_username = cJSON_GetObjectItem(curt_user_json, "username");
        if (s_find(curt_username->valuestring, pattern, 0) != -1) {
            cJSON_AddItemToArray(usernames_list, cJSON_CreateString(curt_username->valuestring));
        }
        cJSON_Delete(curt_user_json);
    }
    closedir(users_dir);
    free(address);
    free(curt_user);

    return usernames_list;
}

cJSON *get_tweet_by_id(int id) {
    cJSON *tweet = NULL;

    DIR *tweets_dir;
    tweets_dir = opendir("..\\Database\\Tweets");
    if (tweets_dir == NULL) {
        return tweet;
    }

    struct dirent *entry;
    char *curt_tweet = (char *) malloc(100000 * sizeof(char));
    char *address = (char *) malloc(100 * sizeof(char));
    while ((entry = readdir(tweets_dir)) != NULL) {
        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0) {
            continue;
        }

        sprintf(address, "..\\Database\\Tweets\\%s", entry->d_name);
        FILE *curt_tweet_file = fopen(address, "r");
        if (curt_tweet_file == NULL) {
            free(address);
            free(curt_tweet);
            continue;
        }

        fgets(curt_tweet, 99999, curt_tweet_file);
        fclose(curt_tweet_file);

        cJSON *curt_tweet_json = cJSON_Parse(curt_tweet);
        cJSON *curt_id = cJSON_GetObjectItem(curt_tweet_json, "id");
        if (curt_id->valueint == id) {
            tweet = curt_tweet_json;
            break;
        }
        cJSON_Delete(curt_tweet_json);
    }
    closedir(tweets_dir);
    free(address);
    free(curt_tweet);

    return tweet;
}

cJSON *get_tweets_by_author(char *author) {
    cJSON *tweets = cJSON_CreateArray();

    DIR *tweets_dir;
    tweets_dir = opendir("..\\Database\\Tweets");
    if (tweets_dir == NULL) {
        return tweets;
    }

    struct dirent *entry;
    char *curt_tweet = (char *) malloc(100000 * sizeof(char));
    char *address = (char *) malloc(100 * sizeof(char));
    while ((entry = readdir(tweets_dir)) != NULL) {
        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0) {
            continue;
        }

        sprintf(address, "..\\Database\\Tweets\\%s", entry->d_name);
        FILE *curt_tweet_file = fopen(address, "r");
        if (curt_tweet_file == NULL) {
            free(address);
            free(curt_tweet_file);
            continue;
        }

        fgets(curt_tweet, 99999, curt_tweet_file);
        fclose(curt_tweet_file);

        cJSON *curt_tweet_json = cJSON_Parse(curt_tweet);
        cJSON *curt_author = cJSON_GetObjectItem(curt_tweet_json, "author");
        if (strcmp(curt_author->valuestring, author) == 0) {
            cJSON_AddItemToArray(tweets, curt_tweet_json);
        }
    }
    closedir(tweets_dir);
    free(address);
    free(curt_tweet);

    return tweets;
}

cJSON *get_tweets_by_tag(char *tag) {
    int tag_len = s_size(tag);
    char *pattern = malloc((tag_len + 2) * sizeof(char));
    sprintf(pattern, "#%s", tag);
    cJSON *tweets = cJSON_CreateArray();

    DIR *tweets_dir;
    tweets_dir = opendir("..\\Database\\Tweets");
    if (tweets_dir == NULL) {
        return tweets;
    }

    struct dirent *entry;
    char *curt_tweet = (char *) malloc(100000 * sizeof(char));
    char *address = (char *) malloc(100 * sizeof(char));
    while ((entry = readdir(tweets_dir)) != NULL) {
        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0) {
            continue;
        }

        sprintf(address, "..\\Database\\Tweets\\%s", entry->d_name);
        FILE *curt_tweet_file = fopen(address, "r");
        if (curt_tweet_file == NULL) {
            free(address);
            free(curt_tweet_file);
            continue;
        }

        fgets(curt_tweet, 99999, curt_tweet_file);
        fclose(curt_tweet_file);

        cJSON *curt_tweet_json = cJSON_Parse(curt_tweet);
        cJSON *curt_tweet_content = cJSON_GetObjectItem(curt_tweet_json, "content");
        if (s_find(curt_tweet_content->valuestring, pattern, 0) != -1) {
            cJSON_AddItemToArray(tweets, curt_tweet_json);
        }
    }
    closedir(tweets_dir);
    free(pattern);
    free(address);
    free(curt_tweet);

    return tweets;
}

void remove_retweet_id(int id) {
    DIR *tweets_dir;
    tweets_dir = opendir("..\\Database\\Tweets");

    char *curt_tweet = (char *) malloc(100000 * sizeof(char));
    char *address = (char *) malloc(100 * sizeof(char));
    struct dirent *entry;
    bool is_deleted = false;
    while ((entry = readdir(tweets_dir)) != NULL) {
        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0) {
            continue;
        }

        sprintf(address, "..\\Database\\Tweets\\%s", entry->d_name);
        FILE *curt_tweet_file = fopen(address, "r");
        if (curt_tweet_file == NULL) {
            free(curt_tweet);
            continue;
        }
        fgets(curt_tweet, 99999, curt_tweet_file);
        fclose(curt_tweet_file);

        cJSON *curt_tweet_json = cJSON_Parse(curt_tweet);
        cJSON *curt_tweet_source = cJSON_GetObjectItem(curt_tweet_json, "source");
        if (curt_tweet_source->valuestring[0] != '\0') {
            cJSON_Delete(curt_tweet_json);
            continue;
        }

        cJSON *curt_tweet_retweets = cJSON_GetObjectItem(curt_tweet_json, "retweets");
        int curt_tweet_retweets_size = cJSON_GetArraySize(curt_tweet_retweets);
        for (int i = 0; i < curt_tweet_retweets_size; i++) {
            printf("%d\n", cJSON_GetArrayItem(curt_tweet_retweets, i)->valueint);
            if (cJSON_GetArrayItem(curt_tweet_retweets, i)->valueint == id) {
                cJSON_DeleteItemFromArray(curt_tweet_retweets, i);
                update_tweet(curt_tweet_json);
                printf("deleted\n");
                is_deleted = true;
                break;
            }
        }
        cJSON_Delete(curt_tweet_json);

        if (is_deleted) {
            break;
        }
    }
    closedir(tweets_dir);
    free(address);
    free(curt_tweet);
}

int get_last_tweet_id() {
    FILE *last_tweet_id = fopen("..\\Database\\last_id.txt", "r");
    if (last_tweet_id == NULL) {
        return -1;
    }
    char *str_id = (char *) malloc(11 * sizeof(char));

    fgets(str_id, 10, last_tweet_id);
    int id = strtol(str_id, NULL, 0);

    fclose(last_tweet_id);
    free(str_id);

    return id;
}

void increment_last_tweet_id() {
    FILE *last_tweet_id = fopen("..\\Database\\last_id.txt", "r");
    char *str_id = (char *) malloc(11 * sizeof(char));

    fgets(str_id, 10, last_tweet_id);
    int id = strtol(str_id, NULL, 0);
    freopen("..\\Database\\last_id.txt", "w", last_tweet_id);
    fprintf(last_tweet_id, "%d", id + 1);

    fclose(last_tweet_id);
    free(str_id);
}

bool is_followed(char *followed_username, char *following_username) {
    cJSON *following_user = get_user_by_username(following_username);
    if (following_user == NULL) {
        return false;
    }
    cJSON *followings = cJSON_GetObjectItem(following_user, "followings");

    cJSON *following = NULL;
    cJSON_ArrayForEach(following, followings) {
        if (strcmp(following->valuestring, followed_username) == 0) {
            cJSON_Delete(following_user);
            return true;
        }
    }

    cJSON_Delete(following_user);
    return false;
}

bool is_tweet_seen(int tweet_id, char *username) {
    cJSON *user = get_user_by_username(username);
    if (user == NULL) {
        return false;
    }
    cJSON *seen_list = cJSON_GetObjectItem(user, "seen_tweets");

    bool is_seen = false;
    cJSON *seen_id;
    cJSON_ArrayForEach(seen_id, seen_list) {
        if (seen_id->valueint == tweet_id) {
            is_seen = true;
            break;
        }
    }
    cJSON_Delete(user);

    return is_seen;
}

bool is_username_unique(char *username) {
    bool is_unique = true;

    DIR *users_dir;
    users_dir = opendir("..\\Database\\Users");

    struct dirent *entry;
    char *address = (char *) malloc(100 * sizeof(char));
    char *curt_user = (char *) malloc(100000 * sizeof(char));
    while ((entry = readdir(users_dir)) != NULL) {
        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0) {
            continue;
        }

        sprintf(address, "..\\Database\\Users\\%s", entry->d_name);
        FILE *curt_user_file = fopen(address, "r");

        fgets(curt_user, 99999, curt_user_file);
        fclose(curt_user_file);

        cJSON *curt_user_json = cJSON_Parse(curt_user);
        if (curt_user_json == NULL) {
            cJSON_Delete(curt_user_json);
            continue;
        }

        cJSON *curt_user_username = cJSON_GetObjectItem(curt_user_json, "username");
        if (strcmp(curt_user_username->valuestring, username) == 0) {
            is_unique = false;
            cJSON_Delete(curt_user_json);
            break;
        }
        cJSON_Delete(curt_user_json);
    }
    closedir(users_dir);
    free(curt_user);
    free(address);

    return is_unique;
}

bool authenticate(char *username, char *password) {
    bool is_valid = false;

    DIR *users_dir;
    users_dir = opendir("..\\Database\\Users");

    struct dirent *entry;
    char *address = (char *) malloc(100 * sizeof(char));
    char *curt_user = (char *) malloc(100000 * sizeof(char));
    while ((entry = readdir(users_dir)) != NULL) {
        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0) {
            continue;
        }

        sprintf(address, "..\\Database\\Users\\%s", entry->d_name);
        FILE *curt_user_file = fopen(address, "r");

        fgets(curt_user, 99999, curt_user_file);
        fclose(curt_user_file);

        cJSON *curt_user_json = cJSON_Parse(curt_user);
        if (curt_user_json == NULL) {
            cJSON_Delete(curt_user_json);
            continue;
        }

        cJSON *curt_user_username = cJSON_GetObjectItem(curt_user_json, "username");
        cJSON *curt_user_password = cJSON_GetObjectItem(curt_user_json, "password");
        if (strcmp(curt_user_username->valuestring, username) == 0 &&
            strcmp(curt_user_password->valuestring, password) == 0) {
            is_valid = true;
            cJSON_Delete(curt_user_json);
            break;
        }
        cJSON_Delete(curt_user_json);
    }
    closedir(users_dir);
    free(curt_user);
    free(address);

    return is_valid;
}

bool is_user_active(char *username) {
    bool is_active = false;

    DIR *active_users_dir;
    active_users_dir = opendir("..\\Database\\Active Users");

    struct dirent *active_entry;
    char *active_address = (char *) malloc(100 * sizeof(char));
    char *curt_active_user = (char *) malloc(100000 * sizeof(char));
    while ((active_entry = readdir(active_users_dir)) != NULL) {
        if (strcmp(active_entry->d_name, ".") == 0 || strcmp(active_entry->d_name, "..") == 0) {
            continue;
        }

        sprintf(active_address, "..\\Database\\Active Users\\%s", active_entry->d_name);
        FILE *curt_active_user_file = fopen(active_address, "r");

        fgets(curt_active_user, 99999, curt_active_user_file);
        fclose(curt_active_user_file);

        cJSON *curt_active_user_json = cJSON_Parse(curt_active_user);
        if (curt_active_user_json == NULL) {
            cJSON_Delete(curt_active_user_json);
            continue;
        }

        cJSON *curt_active_user_username = cJSON_GetObjectItem(curt_active_user_json, "username");
        if (strcmp(curt_active_user_username->valuestring, username) == 0) {
            is_active = true;
            cJSON_Delete(curt_active_user_json);
            break;
        }
        cJSON_Delete(curt_active_user_json);
    }
    closedir(active_users_dir);
    free(curt_active_user);
    free(active_address);

    return is_active;
}

char *sign_up(cJSON *request) {
    if (cJSON_GetObjectSize(request) != 3) {
        return bad_request_format();
    }

    cJSON *json_response = cJSON_CreateObject();

    cJSON *username = cJSON_GetObjectItem(request, "username");
    CHECK(username)
    cJSON *password = cJSON_GetObjectItem(request, "password");
    CHECK(password)

    unsigned int username_len = strlen(username->valuestring);
    unsigned int password_len = strlen(password->valuestring);
    if (username_len > USER_MAX_LEN || username_len < USER_MIN_LEN || s_index(username->valuestring, ' ') != -1) {
        cJSON_AddStringToObject(json_response, "type", "Error");
        cJSON_AddStringToObject(json_response, "message", "Entered username is not valid!");
    } else if (password_len > PASS_MAX_LEN || password_len < PASS_MIN_LEN ||
               s_index(password->valuestring, ' ') != -1) {
        cJSON_AddStringToObject(json_response, "type", "Error");
        cJSON_AddStringToObject(json_response, "message", "Entered password is not valid!");
    } else {
        bool is_unique = is_username_unique(username->valuestring);

        if (is_unique) {
            cJSON *user = cJSON_CreateObject();
            cJSON_AddStringToObject(user, "username", username->valuestring);
            cJSON_AddStringToObject(user, "password", password->valuestring);
            cJSON_AddStringToObject(user, "bio", "");
            cJSON_AddItemToObject(user, "followers", cJSON_CreateArray());
            cJSON_AddItemToObject(user, "followings", cJSON_CreateArray());
            cJSON_AddItemToObject(user, "tweets", cJSON_CreateArray());
            cJSON_AddItemToObject(user, "seen_tweets", cJSON_CreateArray());

            char *address = malloc(100 * sizeof(char));
            sprintf(address, "..\\Database\\Users\\%s.user.json", username->valuestring);
            FILE *user_file = fopen(address, "w");
            char *user_text = cJSON_PrintUnformatted(user);
            fputs(user_text, user_file);

            free(user_text);
            free(address);
            fclose(user_file);
            cJSON_Delete(user);

            char *message = (char *) malloc(100 * sizeof(char));
            sprintf(message, "User %s successfully signed up!", username->valuestring);
            cJSON_AddStringToObject(json_response, "type", "Successful");
            cJSON_AddStringToObject(json_response, "message", message);
            free(message);
        } else {
            cJSON_AddStringToObject(json_response, "type", "Error");
            cJSON_AddStringToObject(json_response, "message", "Entered username is not Unique!");
        }
    }

    char *response = cJSON_PrintUnformatted(json_response);
    cJSON_Delete(json_response);
    printf("Response: %s\n", response);
    return response;
}

char *log_in(cJSON *request) {
    if (cJSON_GetObjectSize(request) != 3) {
        return bad_request_format();
    }

    cJSON *json_response = cJSON_CreateObject();

    cJSON *username = cJSON_GetObjectItem(request, "username");
    CHECK(username)
    cJSON *password = cJSON_GetObjectItem(request, "password");
    CHECK(password)

    bool is_valid = authenticate(username->valuestring, password->valuestring);
    if (is_valid) {
        bool is_active = is_user_active(username->valuestring);
        if (!is_active) {
            char *token = create_token(username->valuestring);

            cJSON *active_user = cJSON_CreateObject();
            cJSON_AddStringToObject(active_user, "username", username->valuestring);
            cJSON_AddStringToObject(active_user, "token", token);

            char *address = malloc(100 * sizeof(char));
            sprintf(address, "..\\Database\\Active Users\\%s.active_user.json", username->valuestring);
            FILE *user_file = fopen(address, "w");
            char *user_text = cJSON_PrintUnformatted(active_user);
            fputs(user_text, user_file);

            free(address);
            free(user_text);
            fclose(user_file);
            cJSON_Delete(active_user);

            char *message = (char *) malloc(100 * sizeof(char));
            sprintf(message, "User %s successfully logged in!", username->valuestring);
            cJSON_AddStringToObject(json_response, "type", "Token");
            cJSON_AddStringToObject(json_response, "message", token);
            free(token);
            free(message);
        } else {
            cJSON_AddStringToObject(json_response, "type", "Error");
            cJSON_AddStringToObject(json_response, "message", "The user is already logged in!");
        }
    } else {
        cJSON_AddStringToObject(json_response, "type", "Error");
        cJSON_AddStringToObject(json_response, "message", "No match found!");
    }

    char *response = cJSON_PrintUnformatted(json_response);
    cJSON_Delete(json_response);
    printf("Response: %s\n", response);
    return response;
}

char *log_out(cJSON *request) {
    if (cJSON_GetObjectSize(request) != 2) {
        return bad_request_format();
    }

    cJSON *json_response = cJSON_CreateObject();

    cJSON *token = cJSON_GetObjectItem(request, "token");
    CHECK(token)

    bool is_active = false;

    DIR *users_dir;
    users_dir = opendir("..\\Database\\Active Users");

    struct dirent *entry;
    char *address = (char *) malloc(100 * sizeof(char));
    char *curt_user = (char *) malloc(100000 * sizeof(char));
    while ((entry = readdir(users_dir)) != NULL) {
        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0) {
            continue;
        }

        sprintf(address, "..\\Database\\Active Users\\%s", entry->d_name);
        FILE *curt_user_file = fopen(address, "r");

        fgets(curt_user, 99999, curt_user_file);
        fclose(curt_user_file);

        cJSON *curt_user_json = cJSON_Parse(curt_user);
        if (curt_user_json == NULL) {
            cJSON_Delete(curt_user_json);
            continue;
        }
        cJSON *curt_user_token = cJSON_GetObjectItem(curt_user_json, "token");
        if (strcmp(curt_user_token->valuestring, token->valuestring) == 0) {
            is_active = true;
            cJSON *curt_username = cJSON_GetObjectItem(curt_user_json, "username");
            remove(address);
            char *message = (char *) malloc(100 * sizeof(char));
            sprintf(message, "User %s successfully logged out!", curt_username->valuestring);
            cJSON_AddStringToObject(json_response, "type", "Successful");
            cJSON_AddStringToObject(json_response, "message", message);

            free(message);
            cJSON_Delete(curt_user_json);
            break;
        }
        cJSON_Delete(curt_user_json);
    }
    closedir(users_dir);
    free(curt_user);
    free(address);

    if (!is_active) {
        cJSON_AddStringToObject(json_response, "type", "Error");
        cJSON_AddStringToObject(json_response, "message", "User is not logged in!");
    }

    char *response = cJSON_PrintUnformatted(json_response);
    cJSON_Delete(json_response);
    printf("Response: %s\n", response);
    return response;
}

char *set_bio(cJSON *request) {
    if (cJSON_GetObjectSize(request) != 3) {
        return bad_request_format();
    }

    cJSON *json_response = cJSON_CreateObject();

    cJSON *token = cJSON_GetObjectItem(request, "token");
    CHECK(token)
    cJSON *bio = cJSON_GetObjectItem(request, "bio");
    CHECK(bio)

    if (strlen(bio->valuestring) <= BIO_MAX_LEN) {
        char *username = get_username_by_token(token->valuestring);
        if (username[0] != '\0') {
            cJSON *user = get_user_by_username(username);
            if (user != NULL) {
                cJSON *old_bio = cJSON_GetObjectItem(user, "bio");
                if (old_bio->valuestring[0] == '\0') {
                    cJSON_SetValuestring(old_bio, bio->valuestring);
                    update_user(user);
                    cJSON_AddStringToObject(json_response, "type", "Successful");
                    cJSON_AddStringToObject(json_response, "message", "Bio set successfully!");
                } else {
                    cJSON_AddStringToObject(json_response, "type", "Error");
                    cJSON_AddStringToObject(json_response, "message", "You can't set you bio twice!");
                }
                cJSON_Delete(user);
            } else {
                cJSON_AddStringToObject(json_response, "type", "Error");
                cJSON_AddStringToObject(json_response, "message", "Something went wrong!");
            }
        } else {
            cJSON_AddStringToObject(json_response, "type", "Error");
            cJSON_AddStringToObject(json_response, "message", "Invalid token!");
        }
        free(username);
    } else {
        cJSON_AddStringToObject(json_response, "type", "Error");
        cJSON_AddStringToObject(json_response, "message", "Bio is to big!");
    }

    char *response = cJSON_PrintUnformatted(json_response);
    cJSON_Delete(json_response);
    printf("Response: %s\n", response);

    return response;
}

char *change_pass(cJSON *request) {
    if (cJSON_GetObjectSize(request) != 4) {
        return bad_request_format();
    }

    cJSON *json_response = cJSON_CreateObject();

    cJSON *token = cJSON_GetObjectItem(request, "token");
    CHECK(token)
    cJSON *old_pass = cJSON_GetObjectItem(request, "old_pass");
    CHECK(old_pass)
    cJSON *new_pass = cJSON_GetObjectItem(request, "new_pass");
    CHECK(new_pass)

    unsigned int new_pass_len = strlen(new_pass->valuestring);
    if (new_pass_len <= PASS_MAX_LEN && new_pass_len >= PASS_MIN_LEN &&
        s_index(new_pass->valuestring, ' ') == -1) {
        if (strcmp(old_pass->valuestring, new_pass->valuestring) != 0) {
            char *username = get_username_by_token(token->valuestring);
            if (username[0] != '\0') {
                cJSON *user = get_user_by_username(username);
                if (user != NULL) {
                    cJSON *pass = cJSON_GetObjectItem(user, "password");
                    if (strcmp(pass->valuestring, old_pass->valuestring) == 0) {
                        cJSON_SetValuestring(pass, new_pass->valuestring);
                        update_user(user);
                        cJSON_AddStringToObject(json_response, "type", "Successful");
                        cJSON_AddStringToObject(json_response, "message", "Password changed successfully!");
                    } else {
                        cJSON_AddStringToObject(json_response, "type", "Error");
                        cJSON_AddStringToObject(json_response, "message", "Entered password is wrong!");
                    }
                    cJSON_Delete(user);
                } else {
                    cJSON_AddStringToObject(json_response, "type", "Error");
                    cJSON_AddStringToObject(json_response, "message", "Something went wrong!");
                }
            } else {
                cJSON_AddStringToObject(json_response, "type", "Error");
                cJSON_AddStringToObject(json_response, "message", "Invalid token!");
            }
            free(username);
        } else {
            cJSON_AddStringToObject(json_response, "type", "Error");
            cJSON_AddStringToObject(json_response, "message", "Entered new password is current password!");
        }
    } else {
        cJSON_AddStringToObject(json_response, "type", "Error");
        cJSON_AddStringToObject(json_response, "message", "Entered new password is not valid!");
    }

    char *response = cJSON_PrintUnformatted(json_response);
    cJSON_Delete(json_response);
    printf("Response: %s\n", response);

    return response;
}

char *search_user_by_username(cJSON *request, bool is_self) {
    if (is_self) {
        if (cJSON_GetObjectSize(request) != 2) {
            return bad_request_format();
        }
    } else {
        if (cJSON_GetObjectSize(request) != 3) {
            return bad_request_format();
        }
    }

    cJSON *json_response = cJSON_CreateObject();

    cJSON *token = cJSON_GetObjectItem(request, "token");
    CHECK(token)

    char *username = get_username_by_token(token->valuestring);
    if (username[0] == '\0') {
        cJSON_AddStringToObject(json_response, "type", "Error");
        cJSON_AddStringToObject(json_response, "message", "Invalid token!");
    } else {
        cJSON *searched_username;
        if (!is_self) {
            searched_username = cJSON_GetObjectItem(request, "username");
            if (searched_username == NULL) {
                free(username);
                cJSON_Delete(json_response);
                return bad_request_format();
            }
        } else {
            searched_username = cJSON_CreateString(username);
        }

        if (is_self || strcmp(username, searched_username->valuestring) != 0) {
            cJSON *searched_user = get_user_by_username(searched_username->valuestring);
            if (searched_user != NULL) {
                cJSON *profile = cJSON_CreateObject();
                cJSON_AddStringToObject(profile, "username", searched_username->valuestring);
                cJSON_AddStringToObject(profile, "bio", cJSON_GetObjectItem(searched_user, "bio")->valuestring);
                cJSON_AddNumberToObject(profile, "numberofFollowers",
                                        cJSON_GetArraySize(cJSON_GetObjectItem(searched_user, "followers")));
                cJSON_AddNumberToObject(profile, "numberofFollowings",
                                        cJSON_GetArraySize(cJSON_GetObjectItem(searched_user, "followings")));
                if (!is_self) {
                    cJSON_AddStringToObject(profile, "followStatus",
                                            is_followed(searched_username->valuestring, username) ? "Followed"
                                                                                                  : "Not Followed");
                }
                cJSON_AddItemToObject(profile, "allTweets", get_tweets_by_author(searched_username->valuestring));

                cJSON_AddStringToObject(json_response, "type", "Profile");
                cJSON_AddItemToObject(json_response, "message", profile);

                cJSON_Delete(searched_user);
            } else {
                cJSON_AddStringToObject(json_response, "type", "Error");
                cJSON_AddStringToObject(json_response, "message", "No match found!");
            }

        } else {
            cJSON_AddStringToObject(json_response, "type", "Error");
            cJSON_AddStringToObject(json_response, "message", "You can't search yourself!");
        }
        if (!is_self) {
            cJSON_Delete(searched_username);
        }
    }
    free(username);

    char *response = cJSON_PrintUnformatted(json_response);
    cJSON_Delete(json_response);
    printf("Response: %s\n", response);

    return response;
}

char *search_username_by_pattern(cJSON *request) {
    if (cJSON_GetObjectSize(request) != 3) {
        return bad_request_format();
    }

    cJSON *json_response = cJSON_CreateObject();

    cJSON *token = cJSON_GetObjectItem(request, "token");
    CHECK(token)
    cJSON *pattern = cJSON_GetObjectItem(request, "pattern");
    CHECK(pattern)

    char *username = get_username_by_token(token->valuestring);
    if (username[0] == '\0') {
        cJSON_AddStringToObject(json_response, "type", "Error");
        cJSON_AddStringToObject(json_response, "message", "Invalid token!");
    } else {
        cJSON *usernames_list = get_usernames_by_pattern(pattern->valuestring);

        cJSON_AddStringToObject(json_response, "type", "List");
        cJSON_AddItemToObject(json_response, "message", usernames_list);

        if (glob_username_list != NULL) {
            cJSON_Delete(glob_username_list);
        }
        glob_username_list = cJSON_CreateArray();
        cJSON *item = NULL;
        cJSON_ArrayForEach(item , usernames_list) {
            cJSON_AddItemToArray(glob_username_list, cJSON_CreateString(item->valuestring));
        }

        free(username);
    }

    char *response = cJSON_PrintUnformatted(json_response);
    cJSON_Delete(json_response);
    printf("Response: %s\n", response);

    return response;
}

char *advanced_user_search(cJSON *request) {
    if (cJSON_GetObjectSize(request) != 3) {
        return bad_request_format();
    }

    cJSON *json_response = cJSON_CreateObject();

    cJSON *token = cJSON_GetObjectItem(request, "token");
    CHECK(token)
    cJSON *username_id = cJSON_GetObjectItem(request, "id");
    CHECK(username_id);
    printf("id: %d\n", username_id->valueint);

    char *username = get_username_by_token(token->valuestring);
    if (username[0] == '\0') {
        cJSON_AddStringToObject(json_response, "type", "Error");
        cJSON_AddStringToObject(json_response, "message", "Invalid token!");
    } else {
        int list_size = cJSON_GetArraySize(glob_username_list);
        printf("%d\n", list_size);
        if (username_id->valueint < 0 || username_id->valueint >= list_size) {
            cJSON_AddStringToObject(json_response, "type", "Error");
            cJSON_AddStringToObject(json_response, "message", "Invalid id!");
        } else {
            cJSON *searched_username = cJSON_GetArrayItem(glob_username_list, username_id->valueint);
            printf("username: %s\n", searched_username->valuestring);
            if (strcmp(username, searched_username->valuestring) != 0) {
                cJSON *searched_user = get_user_by_username(searched_username->valuestring);
                if (searched_user != NULL) {
                    cJSON *profile = cJSON_CreateObject();
                    cJSON_AddStringToObject(profile, "username", searched_username->valuestring);
                    cJSON_AddStringToObject(profile, "bio", cJSON_GetObjectItem(searched_user, "bio")->valuestring);
                    cJSON_AddNumberToObject(profile, "numberofFollowers",
                                            cJSON_GetArraySize(cJSON_GetObjectItem(searched_user, "followers")));
                    cJSON_AddNumberToObject(profile, "numberofFollowings",
                                            cJSON_GetArraySize(cJSON_GetObjectItem(searched_user, "followings")));

                    cJSON_AddStringToObject(profile, "followStatus",
                                            is_followed(searched_username->valuestring, username) ? "Followed"
                                                                                                  : "Not Followed");

                    cJSON_AddItemToObject(profile, "allTweets", get_tweets_by_author(searched_username->valuestring));

                    cJSON_AddStringToObject(json_response, "type", "Profile");
                    cJSON_AddItemToObject(json_response, "message", profile);

                    cJSON_Delete(searched_user);
                } else {
                    cJSON_AddStringToObject(json_response, "type", "Error");
                    cJSON_AddStringToObject(json_response, "message", "No match found!");
                }

            } else {
                cJSON_AddStringToObject(json_response, "type", "Error");
                cJSON_AddStringToObject(json_response, "message", "You can't search yourself!");
            }
        }
    }
    free(username);

    char *response = cJSON_PrintUnformatted(json_response);
    cJSON_Delete(json_response);
    printf("Response: %s\n", response);

    return response;
}

char *search_tweet_by_tag(cJSON *request) {
    if (cJSON_GetObjectSize(request) != 3) {
        return bad_request_format();
    }

    cJSON *json_response = cJSON_CreateObject();

    cJSON *token = cJSON_GetObjectItem(request, "token");
    CHECK(token)
    cJSON *tag = cJSON_GetObjectItem(request, "tag");
    CHECK(tag)

    if (strlen(tag->valuestring) > TAG_MAX_LEN) {
        cJSON_AddStringToObject(json_response, "type", "Error");
        cJSON_AddStringToObject(json_response, "message", "Tag is too big!");
    } else {
        char *username = get_username_by_token(token->valuestring);
        if (username[0] == '\0') {
            cJSON_AddStringToObject(json_response, "type", "Error");
            cJSON_AddStringToObject(json_response, "message", "Invalid token!");
        } else {
            cJSON *tweets = get_tweets_by_tag(tag->valuestring);
            cJSON_AddStringToObject(json_response, "type", "List");
            cJSON_AddItemToObject(json_response, "message", tweets);
        }
        free(username);
    }

    char *response = cJSON_PrintUnformatted(json_response);
    cJSON_Delete(json_response);
    printf("Response: %s\n", response);

    return response;
}

char *follow(cJSON *request) {
    if (cJSON_GetObjectSize(request) != 3) {
        return bad_request_format();
    }

    cJSON *json_response = cJSON_CreateObject();

    cJSON *token = cJSON_GetObjectItem(request, "token");
    CHECK(token)
    cJSON *followed_username = cJSON_GetObjectItem(request, "username");
    CHECK(followed_username)

    char *following_username = get_username_by_token(token->valuestring);
    if (following_username[0] == '\0') {
        cJSON_AddStringToObject(json_response, "type", "Error");
        cJSON_AddStringToObject(json_response, "message", "Invalid token!");
    } else {
        if (!is_followed(followed_username->valuestring, following_username)) {
            cJSON *followed_user = get_user_by_username(followed_username->valuestring);
            if (followed_user != NULL) {
                cJSON *following_user = get_user_by_username(following_username);
                if (following_user != NULL) {
                    cJSON_AddItemToArray(cJSON_GetObjectItem(followed_user, "followers"),
                                         cJSON_CreateString(following_username));
                    cJSON_AddItemToArray(cJSON_GetObjectItem(following_user, "followings"),
                                         cJSON_CreateString(followed_username->valuestring));
                    update_user(following_user);
                    update_user(followed_user);

                    cJSON_AddStringToObject(json_response, "type", "Successful");
                    cJSON_AddStringToObject(json_response, "message", "User followed successfully!");

                    cJSON_Delete(following_user);
                } else {
                    cJSON_AddStringToObject(json_response, "type", "Error");
                    cJSON_AddStringToObject(json_response, "message", "Something went wrong!");
                }

                cJSON_Delete(followed_user);
            } else {
                cJSON_AddStringToObject(json_response, "type", "Error");
                cJSON_AddStringToObject(json_response, "message", "Invalid following_username!");
            }
        } else {
            cJSON_AddStringToObject(json_response, "type", "Error");
            cJSON_AddStringToObject(json_response, "message", "You already are following this user!");
        }
    }
    free(following_username);

    char *response = cJSON_PrintUnformatted(json_response);
    cJSON_Delete(json_response);
    printf("Response: %s\n", response);

    return response;
}

char *unfollow(cJSON *request) {
    if (cJSON_GetObjectSize(request) != 3) {
        return bad_request_format();
    }

    cJSON *json_response = cJSON_CreateObject();

    cJSON *token = cJSON_GetObjectItem(request, "token");
    CHECK(token)
    cJSON *followed_username = cJSON_GetObjectItem(request, "username");
    CHECK(followed_username)

    char *following_username = get_username_by_token(token->valuestring);
    if (following_username[0] == '\0') {
        cJSON_AddStringToObject(json_response, "type", "Error");
        cJSON_AddStringToObject(json_response, "message", "Invalid token!");
    } else {
        if (is_followed(followed_username->valuestring, following_username)) {
            cJSON *followed_user = get_user_by_username(followed_username->valuestring);
            if (followed_user != NULL) {
                cJSON *following_user = get_user_by_username(following_username);
                if (following_user != NULL) {
                    cJSON_DeleteStringFromArrayByElem(cJSON_GetObjectItem(following_user, "followings"),
                                                      followed_username->valuestring);
                    cJSON_DeleteStringFromArrayByElem(cJSON_GetObjectItem(followed_user, "followers"),
                                                      following_username);

                    update_user(following_user);
                    update_user(followed_user);

                    cJSON_AddStringToObject(json_response, "type", "Successful");
                    cJSON_AddStringToObject(json_response, "message", "User followed successfully!");

                    cJSON_Delete(following_user);
                } else {
                    cJSON_AddStringToObject(json_response, "type", "Error");
                    cJSON_AddStringToObject(json_response, "message", "Something went wrong!");
                }
                cJSON_Delete(followed_user);
            } else {
                cJSON_AddStringToObject(json_response, "type", "Error");
                cJSON_AddStringToObject(json_response, "message", "Invalid username!");
            }
        } else {
            cJSON_AddStringToObject(json_response, "type", "Error");
            cJSON_AddStringToObject(json_response, "message", "You are not following this user!");
        }
    }
    free(following_username);

    char *response = cJSON_PrintUnformatted(json_response);
    cJSON_Delete(json_response);
    printf("Response: %s\n", response);

    return response;
}

char *send_tweet(cJSON *request) {
    if (cJSON_GetObjectSize(request) != 3) {
        return bad_request_format();
    }

    cJSON *json_response = cJSON_CreateObject();

    cJSON *token = cJSON_GetObjectItem(request, "token");
    CHECK(token)
    cJSON *tweet = cJSON_GetObjectItem(request, "tweet");
    CHECK(tweet)

    if (strlen(tweet->valuestring) > TWEET_MAX_LEN) {
        cJSON_AddStringToObject(json_response, "type", "Error");
        cJSON_AddStringToObject(json_response, "message", "Tweet is too big!");
    } else {
        char *username = get_username_by_token(token->valuestring);
        if (username[0] == '\0') {
            cJSON_AddStringToObject(json_response, "type", "Error");
            cJSON_AddStringToObject(json_response, "message", "Invalid token!");
        } else {
            int id = get_last_tweet_id() + 1;
            increment_last_tweet_id();

            cJSON *tweet_json = cJSON_CreateObject();
            cJSON_AddNumberToObject(tweet_json, "id", id);
            cJSON_AddStringToObject(tweet_json, "author", username);
            cJSON_AddStringToObject(tweet_json, "source", "");
            cJSON_AddStringToObject(tweet_json, "content", tweet->valuestring);
            cJSON_AddItemToObject(tweet_json, "comments", cJSON_CreateArray());
            cJSON_AddItemToObject(tweet_json, "retweets", cJSON_CreateArray());
            cJSON_AddItemToObject(tweet_json, "likes", cJSON_CreateArray());

            char *address = malloc(100 * sizeof(char));
            sprintf(address, "..\\Database\\Tweets\\#%d.tweet.json", id);
            FILE *tweet_file = fopen(address, "w");
            fputs(cJSON_PrintUnformatted(tweet_json), tweet_file);

            cJSON_AddStringToObject(json_response, "type", "Successful");
            cJSON_AddStringToObject(json_response, "message", "Tweet successfully sent!");

            free(address);
            fclose(tweet_file);
            cJSON_Delete(tweet_json);
        }
        free(username);
    }

    char *response = cJSON_PrintUnformatted(json_response);
    cJSON_Delete(json_response);
    printf("Response: %s\n", response);

    return response;
}

char *retweet(cJSON *request)    {
    if (cJSON_GetObjectSize(request) != 3) {
        return bad_request_format();
    }

    cJSON *json_response = cJSON_CreateObject();

    cJSON *token = cJSON_GetObjectItem(request, "token");
    CHECK(token)
    cJSON *source_id = cJSON_GetObjectItem(request, "id");
    CHECK(source_id)

    char *username = get_username_by_token(token->valuestring);
    if (username[0] == '\0') {
        cJSON_AddStringToObject(json_response, "type", "Error");
        cJSON_AddStringToObject(json_response, "message", "Invalid token!");
    } else {
        cJSON *source_tweet = get_tweet_by_id(source_id->valueint);
        if (source_tweet == NULL) {
            cJSON_AddStringToObject(json_response, "type", "Error");
            cJSON_AddStringToObject(json_response, "message", "Invalid id!");
        } else {
            cJSON *source_tweet_source = cJSON_GetObjectItem(source_tweet, "source");
            if (source_tweet_source->valuestring[0] != '\0') {
                cJSON_AddStringToObject(json_response, "type", "Error");
                cJSON_AddStringToObject(json_response, "message", "You can't retweet a retweet!");
            } else {
                cJSON *source_username = cJSON_GetObjectItem(source_tweet, "author");
                if (strcmp(source_username->valuestring, username) == 0) {
                    cJSON_AddStringToObject(json_response, "type", "Error");
                    cJSON_AddStringToObject(json_response, "message", "You can't retweet your tweet!");
                } else {
                    cJSON *source_tweet_retweets = cJSON_GetObjectItem(source_tweet, "retweets");
                    bool is_retweeted = false;
                    cJSON *source_tweet_retweet_id = NULL;
                    cJSON_ArrayForEach(source_tweet_retweet_id, source_tweet_retweets) {
                        cJSON *retweet = get_tweet_by_id(source_tweet_retweet_id->valueint);
                        cJSON *retweet_author = cJSON_GetObjectItem(retweet, "author");
                        if (strcmp(retweet_author->valuestring, username) == 0) {
                            is_retweeted = true;
                            cJSON_Delete(retweet);
                            break;
                        }
                        cJSON_Delete(retweet);
                    }

                    if (is_retweeted) {
                        cJSON_AddStringToObject(json_response, "type", "Error");
                        cJSON_AddStringToObject(json_response, "message", "You can't retweet a tweet twice!");
                    } else {
                        cJSON *tweet_content = cJSON_GetObjectItem(source_tweet, "content");
                        cJSON *source_author = cJSON_GetObjectItem(source_tweet, "author");
                        int id = get_last_tweet_id() + 1;
                        increment_last_tweet_id();

                        cJSON *tweet_json = cJSON_CreateObject();
                        cJSON_AddNumberToObject(tweet_json, "id", id);
                        cJSON_AddStringToObject(tweet_json, "author", username);
                        cJSON_AddStringToObject(tweet_json, "source", source_author->valuestring);
                        cJSON_AddStringToObject(tweet_json, "content", tweet_content->valuestring);
                        cJSON_AddItemToObject(tweet_json, "comments", cJSON_CreateArray());
                        cJSON_AddItemToObject(tweet_json, "likes", cJSON_CreateArray());

                        char *address = malloc(100 * sizeof(char));
                        sprintf(address, "..\\Database\\Tweets\\#%d.tweet.json", id);
                        FILE *tweet_file = fopen(address, "w");
                        fputs(cJSON_PrintUnformatted(tweet_json), tweet_file);

                        cJSON_AddItemToArray(source_tweet_retweets, cJSON_CreateNumber(id));
                        update_tweet(source_tweet);

                        cJSON_AddStringToObject(json_response, "type", "Successful");
                        cJSON_AddStringToObject(json_response, "message", "Retweeted successfully!");

                        free(address);
                        fclose(tweet_file);
                        cJSON_Delete(tweet_json);
                    }
                }
            }
        }
        cJSON_Delete(source_tweet);
    }
    free(username);

    char *response = cJSON_PrintUnformatted(json_response);
    cJSON_Delete(json_response);
    printf("Response: %s\n", response);

    return response;
}

char *delete_tweet(cJSON *request) {
    if (cJSON_GetObjectSize(request) != 3) {
        return bad_request_format();
    }

    cJSON *json_response = cJSON_CreateObject();

    cJSON *token = cJSON_GetObjectItem(request, "token");
    CHECK(token)
    cJSON *id = cJSON_GetObjectItem(request, "id");
    CHECK(id)

    char *username = get_username_by_token(token->valuestring);
    if (username[0] == '\0') {
        cJSON_AddStringToObject(json_response, "type", "Error");
        cJSON_AddStringToObject(json_response, "message", "Invalid token!");
    } else {
        cJSON *tweet = get_tweet_by_id(id->valueint);
        if (tweet == NULL) {
            cJSON_AddStringToObject(json_response, "type", "Error");
            cJSON_AddStringToObject(json_response, "message", "Invalid id!");
        } else {
            cJSON *author = cJSON_GetObjectItem(tweet, "author");
            if (strcmp(username, author->valuestring) != 0) {
                cJSON_AddStringToObject(json_response, "type", "Error");
                cJSON_AddStringToObject(json_response, "message", "You can't delete other users' tweets!");
            } else {
                char *address = (char *) malloc(100 * sizeof(char));

                sprintf(address, "..\\Database\\Tweets\\#%d.tweet.json", id->valueint);
                remove(address);

                cJSON *source = cJSON_GetObjectItem(tweet, "source");
                if (source->valuestring[0] == '\0') {
                    cJSON *retweets = cJSON_GetObjectItem(tweet, "retweets");
                    cJSON *retweet_id = NULL;
                    cJSON_ArrayForEach(retweet_id, retweets) {
                        sprintf(address, "..\\Database\\Tweets\\#%d.tweet.json", retweet_id->valueint);
                        remove(address);
                    }
                } else {
                    remove_retweet_id(id->valueint);
                }

                free(address);

                cJSON_AddStringToObject(json_response, "type", "Successful");
                cJSON_AddStringToObject(json_response, "message", "Tweet deleted successfully");
            }

            cJSON_Delete(tweet);
        }
    }
    free(username);

    char *response = cJSON_PrintUnformatted(json_response);
    cJSON_Delete(json_response);
    printf("Response: %s\n", response);

    return response;
}

char *send_comment(cJSON *request) {
    if (cJSON_GetObjectSize(request) != 4) {
        return bad_request_format();
    }

    cJSON *json_response = cJSON_CreateObject();

    cJSON *token = cJSON_GetObjectItem(request, "token");
    CHECK(token);

    char *username = get_username_by_token(token->valuestring);
    if (username[0] != '\0') {
        cJSON *id = cJSON_GetObjectItem(request, "id");
        if (id == NULL) {
            free(username);
            cJSON_Delete(json_response);
            return bad_request_format();
        }

        cJSON *tweet = get_tweet_by_id(id->valueint);
        if (tweet != NULL) {
            cJSON *comment_content = cJSON_GetObjectItem(request, "comment");
            if (comment_content == NULL) {
                free(username);
                cJSON_Delete(json_response);
                cJSON_Delete(tweet);
                return bad_request_format();
            }
            cJSON *new_comment = cJSON_CreateObject();
            cJSON_AddStringToObject(new_comment, "name", username);
            cJSON_AddStringToObject(new_comment, "content", comment_content->valuestring);

            cJSON *comments = cJSON_GetObjectItem(tweet, "comments");
            cJSON_AddItemToArray(comments, new_comment);

            update_tweet(tweet);

            cJSON_AddStringToObject(json_response, "type", "Successful");
            cJSON_AddStringToObject(json_response, "message", "Comment sent successfully!");

            cJSON_Delete(tweet);
        } else {
            cJSON_AddStringToObject(json_response, "type", "Error");
            cJSON_AddStringToObject(json_response, "message", "Invalid id!");
        }
    } else {
        cJSON_AddStringToObject(json_response, "type", "Error");
        cJSON_AddStringToObject(json_response, "message", "Invalid token!");
    }
    free(username);

    char *response = cJSON_PrintUnformatted(json_response);
    cJSON_Delete(json_response);
    printf("Response: %s\n", response);

    return response;
}

char *refresh(cJSON *request) {
    if (cJSON_GetObjectSize(request) != 2) {
        return bad_request_format();
    }

    cJSON *json_response = cJSON_CreateObject();

    cJSON *token = cJSON_GetObjectItem(request, "token");
    CHECK(token)

    char *username = get_username_by_token(token->valuestring);
    if (username[0] == '\0') {
        cJSON_AddStringToObject(json_response, "type", "Error");
        cJSON_AddStringToObject(json_response, "message", "Invalid token!");
    } else {
        cJSON *user = get_user_by_username(username);
        if (user != NULL) {
            cJSON *seen_array = cJSON_GetObjectItem(user, "seen_tweets");

            cJSON *tweets_array = cJSON_CreateArray();
            int last_tweet_id = get_last_tweet_id();
            for (int i = 0; i <= last_tweet_id; i++) {
                cJSON *tweet = get_tweet_by_id(i);
                if (tweet != NULL) {
                    cJSON *author = cJSON_GetObjectItem(tweet, "author");
                    cJSON *tweet_id = cJSON_GetObjectItem(tweet, "id");
                    if (is_followed(author->valuestring, username) &&
                        !is_tweet_seen(tweet_id->valueint, username)) {
                        cJSON *source = cJSON_GetObjectItem(tweet, "source");
                        if (!is_followed(source->valuestring, username)) {
                            cJSON_AddItemToArray(seen_array, cJSON_CreateNumber(tweet_id->valuedouble));
                            cJSON_AddItemToArray(tweets_array, tweet);
                        }
                    } else {
                        cJSON_Delete(tweet);
                    }
                }
            }
            update_user(user);

            cJSON_AddStringToObject(json_response, "type", "List");
            cJSON_AddItemToObject(json_response, "message", tweets_array);

            cJSON_Delete(user);
        } else {
            cJSON_AddStringToObject(json_response, "type", "Error");
            cJSON_AddStringToObject(json_response, "message", "Something went wrong!");
        }
    }
    free(username);

    char *response = cJSON_PrintUnformatted(json_response);
    cJSON_Delete(json_response);
    printf("Response: %s\n", response);

    return response;
}

char *like_tweet(cJSON *request) {
    if (cJSON_GetObjectSize(request) != 3) {
        return bad_request_format();
    }

    cJSON *json_response = cJSON_CreateObject();

    cJSON *token = cJSON_GetObjectItem(request, "token");
    CHECK(token)
    cJSON *id = cJSON_GetObjectItem(request, "id");
    CHECK(id)

    char *username = get_username_by_token(token->valuestring);
    if (username[0] == '\0') {
        cJSON_AddStringToObject(json_response, "type", "Error");
        cJSON_AddStringToObject(json_response, "message", "Invalid token!");
    } else {
        cJSON *tweet = get_tweet_by_id(id->valueint);
        if (tweet == NULL) {
            cJSON_AddStringToObject(json_response, "type", "Error");
            cJSON_AddStringToObject(json_response, "message", "Invalid id!");
        } else {
            cJSON *author = cJSON_GetObjectItem(tweet, "author");
            if (strcmp(username, author->valuestring) == 0) {
                cJSON_AddStringToObject(json_response, "type", "Error");
                cJSON_AddStringToObject(json_response, "message", "You can't like your tweets!");
            } else {
                cJSON *likes = cJSON_GetObjectItem(tweet, "likes");

                bool is_liked = false;
                cJSON *like = NULL;
                cJSON_ArrayForEach(like, likes) {
                    if (strcmp(like->valuestring, username) == 0) {
                        is_liked = true;
                        break;
                    }
                }

                if (!is_liked) {
                    cJSON_AddItemToArray(likes, cJSON_CreateString(username));

                    char *address = malloc(1000 * sizeof(char));
                    sprintf(address, "..\\Database\\Tweets\\#%d.tweet.json", id->valueint);
                    FILE *tweet_file = fopen(address, "w");
                    char *tweet_text = cJSON_PrintUnformatted(tweet);
                    fputs(tweet_text, tweet_file);

                    fclose(tweet_file);
                    free(address);
                    free(tweet_text);

                    cJSON_AddStringToObject(json_response, "type", "Successful");
                    cJSON_AddStringToObject(json_response, "message", "Tweet liked successfully!");
                } else {
                    cJSON_AddStringToObject(json_response, "type", "Error");
                    cJSON_AddStringToObject(json_response, "message", "You can't like a tweet twice!");
                }
            }

            cJSON_Delete(tweet);
        }
    }
    free(username);

    char *response = cJSON_PrintUnformatted(json_response);
    cJSON_Delete(json_response);
    printf("Response: %s\n", response);

    return response;
}

char *bad_request_format() {
    cJSON *json_response = cJSON_CreateObject();
    cJSON_AddStringToObject(json_response, "type", "Error");
    cJSON_AddStringToObject(json_response, "message", "Bad request format!");

    char *response = cJSON_PrintUnformatted(json_response);
    cJSON_Delete(json_response);

    return response;
}
