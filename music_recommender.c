#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_USERS 100
#define MAX_SONGS 1000000 // Reasonable limit
#define MAX_LINE 256

typedef struct {
    char name[100];
    int songIndices[MAX_SONGS];
    int songCount;
} User;

typedef struct {
    char title[100];
    int userIndices[MAX_USERS];
    int userCount;
} Song;

User users[MAX_USERS];
Song songs[MAX_SONGS];
int userCount = 0;
int songCount = 0;

// somewhat hash tables for fast lookup and no duplicates. but to be exact its just better linear search
typedef struct {
    char key[100];
    int index;
} HashEntry;

HashEntry userTable[MAX_USERS];
int userTableSize = 0;

HashEntry songTable[MAX_SONGS];
int songTableSize = 0;

int findUserIndex(const char* name) {
    for (int i = 0; i < userTableSize; i++) {
        if (strcmp(userTable[i].key, name) == 0) {
            return userTable[i].index;
        }
    }
    strcpy(users[userCount].name, name);
    users[userCount].songCount = 0;
    strcpy(userTable[userTableSize].key, name);
    userTable[userTableSize++].index = userCount;
    return userCount++;
}

int findSongIndex(const char* title) {
    for (int i = 0; i < songTableSize; i++) {
        if (strcmp(songTable[i].key, title) == 0) {
            return songTable[i].index;
        }
    }
    strcpy(songs[songCount].title, title);
    songs[songCount].userCount = 0;
    strcpy(songTable[songTableSize].key, title);
    songTable[songTableSize++].index = songCount;
    return songCount++;
}

void loadCSV(const char* filename) {
    FILE* file = fopen(filename, "r");
    if (!file) {
        printf("Error opening file.\n");
        return;
    }
    char line[MAX_LINE];
    fgets(line, MAX_LINE, file); // Skip header

    while (fgets(line, MAX_LINE, file)) {
        char* user = strtok(line, ",\n");
        char* song = strtok(NULL, "\n");
        if (user && song) {
            int uIndex = findUserIndex(user);
            int sIndex = findSongIndex(song);
            users[uIndex].songIndices[users[uIndex].songCount++] = sIndex;
            songs[sIndex].userIndices[songs[sIndex].userCount++] = uIndex;
        }
    }
    fclose(file);
}

int heardSongs[MAX_SONGS];

void recommendSongs(const char* username) {
    int uIndex = -1;
    for (int i = 0; i < userCount; i++) {
        if (strcmp(users[i].name, username) == 0) {
            uIndex = i;
            break;
        }
    }
    if (uIndex == -1) {
        printf("User not found.\n");
        return;
    }

    memset(heardSongs, 0, sizeof(int) * songCount);  // mark songs user alr heard.
    for (int i = 0; i < users[uIndex].songCount; i++) {
        heardSongs[users[uIndex].songIndices[i]] = 1;
    }

    int songScores[MAX_SONGS] = {0};
    // main scroing loop. time complexcity is O(UxAxB)
    for (int i = 0; i < users[uIndex].songCount; i++) {//u=Checking each song you heard
        int songIdx = users[uIndex].songIndices[i];
        for (int j = 0; j < songs[songIdx].userCount; j++) {//a=For each song, check similar users
            int otherUserIdx = songs[songIdx].userIndices[j];
            if (otherUserIdx == uIndex) continue;
            for (int k = 0; k < users[otherUserIdx].songCount; k++) {//b=For each similar user, check their songs
                int recSong = users[otherUserIdx].songIndices[k];
                if (!heardSongs[recSong]) {
                    songScores[recSong]++;
                }
            }
        }
    }

    printf("\nRecommended songs for %s:\n", username);
    int any = 0;
    for (int i = 0; i < 20; i++) {
        if (songScores[i] > 0) {
            printf("- %s (score: %d)\n", songs[i].title, songScores[i]);
            any = 1;
        }
    }
    if (!any) {
        printf("No new recommendations.\n");
    }
}

int main() {
    loadCSV("formatted_scrobbles.csv");

    printf("\nLoaded Users:\n");
    for (int i = 0; i < userCount; i++) {
        printf("User %d: %s\n", i + 1, users[i].name);
    }

    char username[100];
    printf("\nEnter username: ");
    fgets(username, 100, stdin);
    username[strcspn(username, "\n")] = 0; // Remove newline
    recommendSongs(username);

    return 0;
}
