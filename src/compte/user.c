#include "user.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define INITIAL_CAPACITY 10
#define USER_DATA_FILE "data/users.txt" // peut être a modifier 


User** users = NULL; // le tableau dynamique qui va stocké les users
int userCount = 0;
int capacity = 0;

User* createUser(const char* new_username) {
  
  if (userCount >= capacity) {
    if (capacity == 0 ) {
      capacity = INITIAL_CAPACITY;
      users = malloc(sizeof(User*) * capacity); 
    } else {
      capacity = capacity * 2;
      users = realloc(users, sizeof(User*) * capacity); 
    }
  }

  if (users == NULL) {
    printf("erreur d'allocation mémoire \n");
    return NULL; // c'est ciao
  }
  
  User* new_user = malloc(sizeof(User));
  if (new_user == NULL) {
    printf("erreur d'allocation mémoire \n");
    return NULL;
  }

  strcpy(new_user->username, new_username);
  new_user->gamePlayed = 0;
  new_user->maxScore = 0;

  users[userCount] = new_user;
  userCount++;

  return new_user ; // succès
}

User* getUser(const char* getUsername) {
  int i =0;
  User* userResult = NULL;

  //Mr Fiorio serait fier de moi
  while ((i < userCount) && (userResult == NULL)) {
    if (strcmp(users[i]->username, getUsername) == 0) {
      userResult = users[i];
    }
    i++;
  }

  return userResult;
}

void updateUserStats(User* u, int newMaxScore) {
  u->maxScore = newMaxScore; 
}

void incrementGame(User* u) {
  u->gamePlayed++;
}

int loadUsers() {
  FILE* file = fopen(USER_DATA_FILE, "r");
  if (file == NULL) {
    printf("aucun fichier utilisateur trouvé");
    return 0; 
  }

  char line[256]; // buffer pour une ligne entière
  int lineNumber = 0;

  while (fgets(line, sizeof(line), file)) {
    lineNumber++;
    line[strcspn(line, "\n")] = '\0';  // enlève le \n éventuel

    char* token = strtok(line, ";");
    if (token != NULL) {
      char username[50];
      strncpy(username, token, sizeof(username)-1);
      username[sizeof(username)-1] = '\0';

      token = strtok(NULL, ";");
      if (token != NULL) {
        int maxScore = atoi(token);

        token = strtok(NULL, ";");
        if (token != NULL) {
          int gamePlayed = atoi(token);

          // tout est correct: créer l'utilisateur
          User* u = createUser(username);
          u->maxScore = maxScore;
          u->gamePlayed = gamePlayed;
        } else {
          printf("ligne %d : champ gamePlayed manquant.\n", lineNumber);
        }

      } else {
        printf("ligne %d : champ maxScore manquant.\n", lineNumber);
      }

    } else {
      printf("ligne %d vide ou mal formatée.\n", lineNumber);
    }
  }

  fclose(file);
  return 1;
}


int saveUsers() {

  return 1;
}