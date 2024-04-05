#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "accounts.h"
#include "library.h"

int main(int argc, char* argv[]) {
    char name[50];
    char surname[50];
    strcpy(name, argv[1]);
    strcpy(surname, argv[2]);

    User newUser;
    strcpy(newUser.name, name);
    strcpy(newUser.surname, surname);

    if (userExists(newUser)) {
        printf("User found.\n");
    }
    else {
        printf("User does not exist. Account created.\n");
        saveUserToFile(newUser);
    }
    // Meniu principal
    int choice;
    char input[100];
    char bookTitle[100];
    char bookAuthor[100];
    int nrCopies;
    char bookName[30], bookAuth[30];
    int nrCpy;
    do {
        displayMenu();
        fgets(input, sizeof(input), stdin); // Read user input
        sscanf(input, "%d", &choice); // Convert user input to integer
        switch (choice) {
        case 1:
            system("cls");
            printf("Enter title to search: ");
            fgets(input, sizeof(input), stdin);
            input[strcspn(input, "\n")] = 0; // Remove newline character
            searchBooks(input); // Search by keyword
            break;
        case 2:
            system("cls");
            printf("Enter author to search: ");
            fgets(input, sizeof(input), stdin);
            input[strcspn(input, "\n")] = 0; // Remove newline character
            searchBooks(input); // Search by author
            break;
        case 3:
            system("cls");
            viewAllBooks();
            break;
        case 4:
            system("cls");
            printf("Enter the title of the book you want to borrow: ");
            fgets(input, sizeof(input), stdin);
            input[strcspn(input, "\n")] = '\0'; // Remove newline character

            printf("Enter the number of copies you want to borrow: ");
            int numCopies;
            scanf("%d", &numCopies);
            getchar(); // Consume newline character left in input buffer
            borrowBook(newUser.name, newUser.surname, input, numCopies);
            remove_zero_or_negative_copies("books.txt");
            break;
        case 5:
            system("cls");
            printf("Enter the title of the book you want to return: ");
            fgets(bookTitle, sizeof(bookTitle), stdin);
            bookTitle[strcspn(bookTitle, "\n")] = 0; // Remove newline character

            printf("Enter the author of the book you want to return: ");
            fgets(bookAuthor, sizeof(bookAuthor), stdin);
            bookAuthor[strcspn(bookAuthor, "\n")] = 0; // Remove newline character

            printf("Enter the number of copies you want to return: ");
            scanf("%d", &nrCopies);
            getchar();
            returnBooks(newUser.name, newUser.surname, bookTitle, bookAuthor, nrCopies);
            break;
        case 6:
            system("cls");
            printUserBooks(newUser.name, newUser.surname);
            break;
        case 7:
            system("cls");
            printf("Enter the title of the book you want to donate: ");
            scanf("%s", bookName);
            printf("Enter the name of the author of the book: ");
            scanf("%s", bookAuth);
            printf("Enter the number of books you want to donate: ");
            scanf("%d", &nrCpy);
            getchar();
            donateBook(bookName, bookAuth, nrCpy);
            printf("Donation successfull, thank you!");
            break;
        case 8:
            printf("Exiting...\n");
            break;
        default:
            printf("Invalid choice. Please enter a number between 1 and 6.\n");
        }
    } while (choice != 8);

    return 0;

}
