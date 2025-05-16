#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_USERS 100
#define MAX_SONGS 1000
#define MAX_NAME_LEN 100

typedef struct {
    char username[MAX_NAME_LEN];
    char songs[MAX_SONGS][MAX_NAME_LEN];
    int song_count;
} User;

User users[MAX_USERS];
int user_count = 0;

// Helper: Trim newline and comma stuff
void trim_newline(char *str) {
    str[strcspn(str, "\r\n")] = 0;
}

// Find or create a user
int get_user_index(const char *name) {
    for (int i = 0; i < user_count; i++) {
        if (strcmp(users[i].username, name) == 0)
            return i;
    }
    // New user
    strcpy(users[user_count].username, name);
    users[user_count].song_count = 0;
    return user_count++;
}

// Check if a user already has a song
int user_has_song(int user_idx, const char *song) {
    for (int i = 0; i < users[user_idx].song_count; i++) {
        if (strcmp(users[user_idx].songs[i], song) == 0)
            return 1;
    }
    return 0;
}

// Recommend songs based on similar users
void recommend_songs(const char *target_user) {
    int target_idx = get_user_index(target_user);
    if (target_idx >= user_count) {
        printf("User not found.\n");
        return;
    }

    printf("🎧 Recommendations for %s:\n", target_user);

    for (int i = 0; i < user_count; i++) {
        if (i == target_idx) continue;

        // Find shared songs
        int common = 0;
        for (int j = 0; j < users[i].song_count; j++) {
            if (user_has_song(target_idx, users[i].songs[j]))
                common++;
        }

        if (common > 0) {
            // Recommend their unique songs
            for (int j = 0; j < users[i].song_count; j++) {
                if (!user_has_song(target_idx, users[i].songs[j])) {
                    printf("- %s (liked by %s)\n", users[i].songs[j], users[i].username);
                }
            }
        }
    }
}

int main() {
    FILE *file = fopen("formatted_scrobbles.csv", "r");
    if (!file) {
        printf("Error opening file.\n");
        return 1;
    }

    char line[300];
    fgets(line, sizeof(line), file); // Skip header

    while (fgets(line, sizeof(line), file)) {
        char *username = strtok(line, ",");
        char *artist = strtok(NULL, ",");
        char *track = strtok(NULL, ",");

        if (!username || !artist || !track) continue;

        trim_newline(username);
        trim_newline(artist);
        trim_newline(track);

        char song[MAX_NAME_LEN];
        snprintf(song, MAX_NAME_LEN, "%s - %s", artist, track);

        int idx = get_user_index(username);
        if (!user_has_song(idx, song)) {
            strcpy(users[idx].songs[users[idx].song_count++], song);
        }
    }

    fclose(file);

    // Example recommendation
    char input_user[MAX_NAME_LEN];
    printf("Enter your Last.fm username: ");
    fgets(input_user, sizeof(input_user), stdin);
    trim_newline(input_user);

    recommend_songs(input_user);

    return 0;
}
