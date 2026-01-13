#ifndef USER_H
#define USER_H

/**
 * @struct User
 * @brief représente l'état d'un joueurs et stock la data associé (score, kill,
 * death, ect...)
 */
typedef struct {
  char username[50]; // limite de 50 character
  int maxScore;      // >= 0
  int gamePlayed;    // >= 0
  int kill;          // >= 0
  int death;         // >= 0
} User;

// permet une instance user et de le renvoyer
User *createUser(const char *newUsername);

// on récupére un user via le username et on le renvoie
User *getUser(const char *getUsername);

// on récup le user via son username dans la db si il n'existe pas on le créer
// renvoie le User créer ou trouver
User *getOrCreateUser(const char *username);

// si le score max du user est battu on met le nouveau score au user
void updateUserStats(User *u, int newScore);

// ajoute 1 a gamePlayed du user
void incrementGame(User *u);

// ajoute 1 a death du user
void incrementDeath(User *u);

// ajoute 1 a kill du user
void incrementKill(User *u);

// renvoie le kdratio du user
float getKdRatio(User *u);

// écrit toutes les structs User en mémoire dans le fichier
int saveUsers(User *u1, User *u2);

// Récupère une liste de tous les pseudos existants
int GetAllUsers(char ***usernames);

extern User *users[2]; // tableau des deux users

#endif