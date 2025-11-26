#include "user.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define INITIAL_CAPACITY 10

User* users = NULL; // le tableau dynamique qui va stocké les users
int userCount = 0;
int capacity = 0;

int createUser(User u) {
  if (userCount >= capacity) {
    if (capacity == 0 ) {
      capacity = INITIAL_CAPACITY;
      users = malloc(sizeof(User) * capacity); 
    } else {
      capacity = capacity * 2;
      users = realloc(users, sizeof(User) * capacity); 
    }
  }

  if (users == NULL) {
    printf("erreur d'allocation mémoire \n");
    return 0; // c'est ciao
  }

  users[userCount+1] = u; 

  return 1; // succès
}

int loadUsers() {

}

int saveUsers() {

}


User* getUser(const char* username) {
  
}

void updateUserStats(User* u, int newMaxScore) {
  u->maxScore = newMaxScore; 
}

void incrementGame(User* u) {
  u->gamePlayed++
}