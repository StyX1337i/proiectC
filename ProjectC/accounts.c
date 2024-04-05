#include <stdio.h>
#include <string.h>
#include "accounts.h"

// Function to save user to file
void saveUserToFile(User newUser) {
    FILE* file = fopen("users.txt", "a");
    if (file == NULL) {
        printf("Error opening file.\n");
        return;
    }
    fprintf(file, "%s %s\n", newUser.name, newUser.surname);
    fclose(file);
}

void displayMenu() {
    printf("\nMain Menu:\n");
    printf("1. Search Books by title\n");
    printf("2. Search Books by author\n");
    printf("3. View Books\n");
    printf("4. Borrow book/s\n");
    printf("5. Return book/s\n");
    printf("6. Show user books\n");
    printf("7. Donate Book/s\n");
    printf("8. Exit\n");
    printf("Enter your choice: ");
}

// Function to check if user exists
int userExists(User checkUser) {
    FILE* file = fopen("users.txt", "r");
    if (file == NULL) {
        printf("Error opening file.\n");
        return 0;
    }

    char line[100];
    while (fgets(line, sizeof(line), file)) {
        char name[50], surname[50];
        sscanf(line, "%s %s", name, surname);
        if (strcmp(name, checkUser.name) == 0 && strcmp(surname, checkUser.surname) == 0) {
            fclose(file);
            return 1; // User exists
        }
    }

    fclose(file);
    return 0; // User does not exist
}

