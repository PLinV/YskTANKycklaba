#include "user.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define INITIAL_CAPACITY 10
#define USER_DATA_FILE "data/users.txt" // peut être a modifier

/* gcc \
  YskTANKycklaba/src/dataCode/user.c \
  -o user \
  -Iraylib/src \
  raylib/src/libraylib.a \
  -lm -lpthread -ldl -lrt -lX11 -lGL */

// le tableau qui va stocker les deux users (on pourra essayer de faire avec
// trois)
User *users[2] = {NULL, NULL};

User *createUser(const char *newUsername) {
  if (strlen(newUsername) > 50) {
    printf("username trop long");
    return NULL;
  }

  User *newUser = malloc(sizeof(User));
  if (!newUser) {
    printf("erreur allocation mémoire\n");
    return NULL;
  }

  strcpy(newUser->username, newUsername);
  newUser->gamePlayed = 0;
  newUser->maxScore = 0;
  newUser->kill = 0;
  newUser->death = 0;
  newUser->currentScore = 0;
  newUser->currentKills = 0;
  newUser->currentDeaths = 0;
  return newUser;
}

User *getUser(const char *getUsername) {
  User *userFound = NULL;
  int found = 0; // savoir si on a trouvé un user
  FILE *file = fopen(USER_DATA_FILE, "r");

  if (file == NULL) {
    printf("aucun fichier utilisateur trouvé");
    return NULL;
  }

  char line[256]; // buffer pour une ligne entière
  int lineNumber = 0;

  while (fgets(line, sizeof(line), file) && (!found)) {

    lineNumber++;
    line[strcspn(line, "\n")] = '\0'; // enlève le \n éventuel

    char *token = strtok(line, ";");
    if (!token)
      continue;

    if (strcmp(token, getUsername) == 0) {
      found = 1;
      char username[50];
      strncpy(username, token, sizeof(username) - 1);
      username[sizeof(username) - 1] = '\0';

      token = strtok(NULL, ";");
      if (token != NULL) {
        int maxScore = atoi(token);
        token = strtok(NULL, ";");
        if (token != NULL) {
          int gamePlayed = atoi(token);
          token = strtok(NULL, ";");
          if (token != NULL) {
            int kill = atoi(token);
            token = strtok(NULL, ";");
            if (token != NULL) {
              int death = atoi(token);

              // tout est correct => créer l'utilisateur
              userFound = createUser(username);
              userFound->maxScore = maxScore;
              userFound->gamePlayed = gamePlayed;
              userFound->kill = kill;
              userFound->death = death;

            } else {
              printf("ligne %d : champ death manquant.\n", lineNumber);
            }
          } else {
            printf("ligne %d : champ kill manquant.\n", lineNumber);
          }
        } else {
          printf("ligne %d : champ gamePlayed manquant.\n", lineNumber);
        }
      } else {
        printf("ligne %d : champ maxScore manquant.\n", lineNumber);
      }
    }
  }

  if (!found) {
    return NULL;
  }
  fclose(file);
  return userFound;
}

User *getOrCreateUser(const char *getUsername) {
  User *userFound = NULL;
  int found = 0; // savoir si on a trouvé un user
  FILE *file = fopen(USER_DATA_FILE, "r");

  if (file == NULL) { // test si le fichier existe
    printf("aucun fichier utilisateur trouvé");
    return NULL;
  }

  char line[256]; // buffer pour une ligne entière
  int lineNumber = 0;

  while (fgets(line, sizeof(line), file) && (!found)) { // tant qu'on a pas trouvé ou que on a pas fini le fichier

    lineNumber++;
    line[strcspn(line, "\n")] = '\0'; // enlève le \n éventuel

    char *token = strtok(line, ";");
    if (!token)
      continue;

    if (strcmp(token, getUsername) == 0) { // on a trouvé le username
      found = 1;
      char username[50];
      strncpy(username, token, sizeof(username) - 1);
      username[sizeof(username) - 1] = '\0';

      token = strtok(NULL, ";");
      if (token != NULL) {
        int maxScore = atoi(token); // on récupère le maxScore
        token = strtok(NULL, ";");
        if (token != NULL) {
          int gamePlayed = atoi(token); // on récupère le gamePlayed
          token = strtok(NULL, ";");
          if (token != NULL) {
            int kill = atoi(token); // on récupère le kill
            token = strtok(NULL, ";");
            if (token != NULL) {
              int death = atoi(token); // on récupère le death

              // tout est correct => créer l'utilisateur on ajoute les informations
              userFound = createUser(username);
              userFound->maxScore = maxScore;
              userFound->gamePlayed = gamePlayed;
              userFound->kill = kill;
              userFound->death = death;
              
            } else {
              printf("ligne %d : champ death manquant.\n", lineNumber);
            }
          } else {
            printf("ligne %d : champ kill manquant.\n", lineNumber);
          }
        } else {
          printf("ligne %d : champ gamePlayed manquant.\n", lineNumber);
        }
      } else {
        printf("ligne %d : champ maxScore manquant.\n", lineNumber);
      }
    }
  }

  if (!found) {
    userFound = createUser(getUsername);
  }
  fclose(file);
  return userFound; // retourne l'utilisateur trouvé ou créé. 
}

void updateUserStats(User *u, int newMaxScore) {
  if (newMaxScore > u->maxScore) {
    u->maxScore = newMaxScore;
  }
}

void incrementGame(User *u) { u->gamePlayed++; }

void incrementDeath(User *u, int amount) { u->death += amount; }

void incrementKill(User *u, int amount) { u->kill += amount; }

// save l'user dans le txt
int saveUser(User *u1) {
  FILE *file = fopen(USER_DATA_FILE, "r");
  char *tempFile = "data/users.tmp";
  FILE *temp = fopen(tempFile, "w"); // on crée un fichier temporaire pour pas écraser
                                     // les données avant d'avoir tout lu
  if (!temp) {
    printf("Erreur : impossible d'écrire le fichier temporaire.\n");
    return 0;
  }
  if (!file) {
    printf("Erreur : impossible de lire le fichier source.\n");
    fclose(temp);
    return 0;
  }

  int u1Written = 0;
  char originalLine[256]; // pour stocker la ligne originale
  char line[256];

  while (fgets(line, sizeof(line), file)) {
    line[strcspn(line, "\n")] = '\0'; 
    strcpy(originalLine, line);
    char *token = strtok(line, ";");
    if (token == NULL)
      continue;

    if (strcmp(token, u1->username) == 0) { // on regarde si le username est le même 
      fprintf(temp, "%s;%d;%d;%d;%d\n", u1->username, u1->maxScore, // si oui on écrit les nouvelles données
              u1->gamePlayed, u1->kill, u1->death);
      u1Written = 1;
    } else {
      fputs(originalLine, temp); // sinon on réécrit la ligne originale
      fprintf(temp, "\n");
    }
  }

  // Ajouter les utilisateurs qui n’existaient pas
  if (!u1Written)
    fprintf(temp, "%s;%d;%d;%d;%d\n", u1->username, u1->maxScore,
            u1->gamePlayed, u1->kill, u1->death);

  fclose(file);
  fclose(temp);

  remove(USER_DATA_FILE); // supprimer l'ancien fichier 
  rename(tempFile, USER_DATA_FILE); // renommer le fichier temporaire

  return 1;
}

float getKdRatio(User *u) {
  if (u->death == 0) {
    return (float)u->kill;
  } else {
    return (float)u->kill / u->death;
  }
}

// Retrieve all usernames from the user data file
// Returns the count of users, and fills the char*** usernames pointer
int GetAllUsers(char ***usernames) {
  FILE *file = fopen(USER_DATA_FILE, "r");
  if (!file) {
    return 0; // No file, no users
  }

  int count = 0;
  int capacity = 10;
  char **list = malloc(capacity * sizeof(char *));
  if (!list) {
    fclose(file);
    return 0;
  }

  char line[256];
  if (!fgets(line, sizeof(line), file)) {
    fclose(file);
    *usernames = NULL;
    return 0;
  }
  // Reset file pointer to beginning if we want to read the first line as data,
  // OR if the first line is header, we should skip it.
  // Assuming the file has a header based on other functions, so we consume it.
  // Actually, users.txt usually has a header "username;maxScore...".
  // If the file was just cleared by user, it might be empty (size 0).
  // If it's empty, fgets above handles it.
  // If it has header, we should probably skip it if it's treated as header,
  // BUT existing code in this function didn't seem to explicitly skip header??
  // Looking at GetAllUsers in previous view_file:
  /*
  while (fgets(line, sizeof(line), file)) {
    // ...
    char *token = strtok(line, ";");
    // ...
  }
  */
  // It seems it treats the header as a user if it's not careful.
  // However, "username" (header) won't crash, just be added as a user?
  // Let's stick to preventing crash on empty file first.

  rewind(file); // Go back to start to let the loop handle everything including
                // header if logic allows, OR if it was empty we returned above.

  while (fgets(line, sizeof(line), file)) {
    line[strcspn(line, "\n")] = '\0';
    if (strlen(line) == 0)
      continue;

    char *token = strtok(line, ";");
    if (token == NULL)
      continue;

    // Store username
    if (count >= capacity) {
      capacity *= 2;
      char **temp = realloc(list, capacity * sizeof(char *));
      if (!temp) {
        // Handle cleanup if needed, for now just return what we have
        break;
      }
      list = temp;
    }

    list[count] = malloc((strlen(token) + 1) * sizeof(char));
    if (list[count]) {
      strcpy(list[count], token);
      count++;
    }
  }

  fclose(file);
  *usernames = list;
  return count;
}
