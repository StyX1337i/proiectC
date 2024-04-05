#ifndef USER_H
#define USER_H

typedef struct {
    char name[50];
    char surname[50];
} User;

void displayMenu();
void saveUserToFile(User newUser);
int userExists(User checkUser);

#endif
