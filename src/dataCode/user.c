#include "user.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define INITIAL_CAPACITY 10
#define USER_DATA_FILE "YskTANKycklaba/data/users.txt" // peut être a modifier z

//gcc \
  YskTANKycklaba/src/dataCode/user.c \
  -o user \
  -Iraylib/src \
  raylib/src/libraylib.a \
  -lm -lpthread -ldl -lrt -lX11 -lGL


// le tableau qui va stocker les deux users (on pourra essayer de faire avec trois)
User* users[2] = {NULL, NULL}; 

User* createUser(const char* newUsername) {
  if (strlen(newUsername) > 50) {
    printf("username trop long");
    return NULL;
  }
  
  User* newUser = malloc(sizeof(User));
  if (!newUser) {
    printf("erreur allocation mémoire\n");
    return NULL;
  }

  strcpy(newUser->username, newUsername);
  newUser->gamePlayed = 0;
  newUser->maxScore = 0;
  newUser->kill = 0;
  newUser->death = 0;
  return newUser;
}

User* getUser(const char* getUsername){
  User* userFound = NULL;
  int found = 0; // savoir si on a trouvé un user
  FILE* file = fopen(USER_DATA_FILE, "r");

  if (file == NULL) {
    printf("aucun fichier utilisateur trouvé");
    return NULL; 
  }

  char line[256]; // buffer pour une ligne entière
  int lineNumber = 0;

  while (fgets(line, sizeof(line), file) && (!found)) {
     
    lineNumber++;
    line[strcspn(line, "\n")] = '\0';  // enlève le \n éventuel

    char* token = strtok(line, ";");
    if (strcmp(token, getUsername) == 0) {
      found = 1;
      char username[50];
      strncpy(username, token, sizeof(username)-1);
      username[sizeof(username)-1] = '\0';

      token = strtok(NULL, ";");
      if (token != NULL) {
        int maxScore = atoi(token);
        token = strtok(NULL, ";");
        if (token != NULL) {
          int gamePlayed = atoi(token);
          token = strtok(NULL, ";");
          if (token !=NULL) {
            int kill = atoi(token); 
            token = strtok(NULL, ";");
            if (token != NULL) {
              int death = atoi(token);

              // tout est correct => créer l'utilisateur
              userFound = createUser(username);
              userFound->maxScore = maxScore;
              userFound->gamePlayed = gamePlayed;
              userFound->kill = kill;
              

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

User* getOrCreateUser(const char* getUsername) {
  User* userFound = NULL;
  int found = 0; // savoir si on a trouvé un user
  FILE* file = fopen(USER_DATA_FILE, "r");

  if (file == NULL) {
    printf("aucun fichier utilisateur trouvé");
    return NULL; 
  }

  char line[256]; // buffer pour une ligne entière
  int lineNumber = 0;

  while (fgets(line, sizeof(line), file) && (!found)) {
     
    lineNumber++;
    line[strcspn(line, "\n")] = '\0';  // enlève le \n éventuel

    char* token = strtok(line, ";");
    if (strcmp(token, getUsername) == 0) {
      found = 1;
      char username[50];
      strncpy(username, token, sizeof(username)-1);
      username[sizeof(username)-1] = '\0';

      token = strtok(NULL, ";");
      if (token != NULL) {
        int maxScore = atoi(token);
        token = strtok(NULL, ";");
        if (token != NULL) {
          int gamePlayed = atoi(token);
          token = strtok(NULL, ";");
          if (token !=NULL) {
            int kill = atoi(token); 
            token = strtok(NULL, ";");
            if (token != NULL) {
              int death = atoi(token);

              // tout est correct => créer l'utilisateur
              userFound = createUser(username);
              userFound->maxScore = maxScore;
              userFound->gamePlayed = gamePlayed;
              userFound->kill = kill;
              

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
  return userFound;
}

void updateUserStats(User* u, int newMaxScore) {
  u->maxScore = newMaxScore; 
}

void incrementGame(User* u) {
  u->gamePlayed++;
}

void incrementDeath(User* u) {
  u->death++;
}

void incrementKill(User* u) {
  u->kill++;
}

//save les deux users dans le txt
int saveUsers(User* u1, User* u2) {
  FILE* file = fopen(USER_DATA_FILE, "r");
  char* tempFile = "YskTANKycklaba/data/users.tmp";
  FILE* temp = fopen(tempFile, "w");
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
  int u2Written = 0;
  char originalLine[256];

  char line[256];

  while (fgets(line, sizeof(line), file)) {
    line[strcspn(line, "\n")] = '\0';
    strcpy(originalLine, line);
    char* token = strtok(line, ";");
    if (token == NULL) continue;

    if (strcmp(token, u1->username) == 0) {
      fprintf(temp, "%s;%d;%d;%d;%d\n", u1->username, u1->maxScore, u1->gamePlayed, u1->kill, u1->death);
      u1Written = 1;
    } else if (strcmp(token, u2->username) == 0) {
      fprintf(temp, "%s;%d;%d;%d;%d\n", u2->username, u2->maxScore, u2->gamePlayed, u2->kill, u2->death);
      u2Written = 1;
    } else {
      fputs(originalLine, temp);
      fprintf(temp, "\n");
    }
  }

  // Ajouter les utilisateurs qui n’existaient pas
  if (!u1Written)
    fprintf(temp, "%s;%d;%d;%d;%d\n", u1->username, u1->maxScore, u1->gamePlayed, u1->kill, u1->death);

  if (!u2Written)
    fprintf(temp, "%s;%d;%d;%d;%d\n", u2->username, u2->maxScore, u2->gamePlayed, u2->kill, u2->death);

  fclose(file);
  fclose(temp);

  remove(USER_DATA_FILE);
  rename(tempFile, USER_DATA_FILE);

  return 1;
}

float getKdRatio(User* u) {
  if (u->death == 0) {
    return (float)u->kill;
  } else {
    return (float)u->kill / u->death;
  }
}

/*
int main() {
  // Récupération ou création de deux utilisateurs
  User* u1 = getOrCreateUser("Alice");
  User* u2 = getOrCreateUser("Bob");

  if (!u1 || !u2) {
    printf("Erreur lors de la récupération des utilisateurs.\n");
    return 1;
  }

  // Simuler des parties
  incrementGame(u1);
  incrementGame(u1);
  incrementGame(u1);
  updateUserStats(u1, 10);

  incrementGame(u2);
  updateUserStats(u2, 120);

  // Sauvegarder les modifications
  if (saveUsers(u1, u2)) {
    printf("Sauvegarde réussie !\n");
  } else {
    printf("Erreur lors de la sauvegarde.\n");
  }

  // Affichage pour vérifier
  printf("=== État actuel des utilisateurs ===\n");
  printf("%s : Score Max = %d, Parties jouées = %d, kd ratio = %.2f\n", u1->username, u1->maxScore, u1->gamePlayed, getKdRatio(u1));
  printf("%s : Score Max = %d, Parties jouées = %d, kd ratio = %.2f\n", u2->username, u2->maxScore, u2->gamePlayed, getKdRatio(u2));

  // Libération mémoire
  free(u1);
  free(u2);

  return 0;
}
*/
