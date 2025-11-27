#ifndef USER_H
#define USER_H

typedef struct {
  char username[50]; 
  int maxScore;
  int gamePlayed;
} User;

// lit chaque ligne du fichier, crée les structs User en mémoire
int loadUsers();

// écrit toutes les structs User en mémoire dans le fichier
int saveUsers();

// on récup le user via son username
User* getUser(const char* username); 

// ajoute un nouvel utilisateur dans la liste users
User* createUser(const char* new_username);

// si le score max du user est battu on met le nouveau score
void updateUserStats(User* u, int newScore);

// ajoute 1 a gamePlayed a chaque fin de partie
void incrementGame(User* u);

extern User** users;
extern int userCount;

#endif