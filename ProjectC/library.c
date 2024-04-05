#include <stdio.h>
#include <string.h>
#include "library.h"

#define MAX_BOOKS 20
#define MAX_TITLE_LENGTH 100
#define MAX_AUTHOR_LENGTH 100

typedef struct {
    char title[MAX_TITLE_LENGTH];
    char author[MAX_AUTHOR_LENGTH];
    int availableCopies;
} Book;

void remove_zero_or_negative_copies(const char* filename) {
    FILE* file = fopen(filename, "r");
    if (file == NULL) {
        fprintf(stderr, "Error opening file %s\n", filename);
        return;
    }

    FILE* temp_file = tmpfile();
    if (temp_file == NULL) {
        fclose(file);
        fprintf(stderr, "Error creating temporary file\n");
        return;
    }

    Book book;
    while (fscanf(file, "%s %s %d", book.title, book.author, &book.availableCopies) == 3) {
        if (book.availableCopies > 0) {
            fprintf(temp_file, "%s %s %d\n", book.title, book.author, book.availableCopies);
        }
    }

    fclose(file);
    fseek(temp_file, 0, SEEK_SET);

    file = fopen(filename, "w");
    if (file == NULL) {
        fclose(temp_file);
        fprintf(stderr, "Error opening file %s for writing\n", filename);
        return;
    }

    char line[MAX_TITLE_LENGTH + MAX_AUTHOR_LENGTH + 20]; // Max line length considering all fields
    while (fgets(line, sizeof(line), temp_file) != NULL) {
        fputs(line, file);
    }

    fclose(file);
    fclose(temp_file);
}

void searchBooks(const char* keyword) {
    FILE* file = fopen("books.txt", "r");
    if (file == NULL) {
        printf("Error opening file.\n");
        return;
    }

    int found = 0;

    char title[MAX_TITLE_LENGTH], author[MAX_AUTHOR_LENGTH];
    int availableCopies;
    while (fscanf(file, "%s %s %d", title, author, &availableCopies) != EOF) {
        if (strstr(title, keyword) != NULL || strstr(author, keyword) != NULL) {
            printf("Title: %s, Author: %s, Available Copies: %d\n", title, author, availableCopies);
            found = 1; // Setăm flag-ul că am găsit cel puțin o carte
        }
    }

    fclose(file);

    if (!found) {
        printf("No books found matching the keyword '%s'\n", keyword);
    }
}

void borrowBook(const char* username, const char* username1, const char* title, int numCopies) {
    FILE* inFile = fopen("books.txt", "r");
    if (inFile == NULL) {
        printf("Error opening file.\n");
        return;
    }

    FILE* borrowedFile = fopen("borrowed_books.txt", "a");
    if (borrowedFile == NULL) {
        printf("Error opening file.\n");
        fclose(inFile);
        return;
    }

    FILE* tempFile = fopen("temp.txt", "w");
    if (tempFile == NULL) {
        printf("Error opening file.\n");
        fclose(inFile);
        fclose(borrowedFile);
        return;
    }

    char line[500];
    int found = 0;
    char auth[30];
    while (fgets(line, sizeof(line), inFile)) {
        char bookTitle[MAX_TITLE_LENGTH], author[MAX_AUTHOR_LENGTH];
        int availableCopies;

        sscanf(line, "%s %s %d", bookTitle, author, &availableCopies);

        if (strcmp(bookTitle, title) == 0) {
            availableCopies -= numCopies;
            if (availableCopies < 0) {
                availableCopies = 0; // Ensure available copies doesn't go below 0
            }
            found = 1;

            if (numCopies > availableCopies) {
                printf("Error: Cannot borrow more books than available.\n");
                fclose(borrowedFile);
                fclose(inFile);
                fclose(tempFile);
                remove("temp.txt");
                return;
            }

            strcpy(auth, author);
            fprintf(borrowedFile, "%s", line); // Write borrowed book to separate file
            fprintf(tempFile, "%s %s %d\n", bookTitle, author, availableCopies); // Write updated book info to temp file
        }
        else {
            fprintf(tempFile, "%s", line); // Write unchanged book info to temp file
        }
    }

    fclose(inFile);
    fclose(borrowedFile);
    fclose(tempFile);

    remove("books.txt");
    rename("temp.txt", "books.txt");

    if (!found) {
        printf("Book '%s' not found in the library.\n", title);
        return;
    }

    FILE* usersFile = fopen("borrowedbooks.txt", "a");
    if (usersFile == NULL) {
        printf("Error opening file.\n");
        return;
    }

    fprintf(usersFile, "%s %s %s %s %d\n", username, username1, title, auth, numCopies);

    fclose(usersFile);

    printf("Book '%s' successfully borrowed by %s.\n", title, username);
}



void returnBooks(const char* username, const char* username1, const char* bookTitle, const char* bookAuthor, int numCopies) {
    // Open the borrowed books file for reading
    FILE* borrowedFile = fopen("borrowedbooks.txt", "r");
    if (borrowedFile == NULL) {
        printf("Error opening borrowedbooks.txt file.\n");
        return;
    }

    // Open the books file for reading and writing
    FILE* booksFile = fopen("books.txt", "r+");
    if (booksFile == NULL) {
        printf("Error opening books.txt file.\n");
        fclose(borrowedFile);
        return;
    }

    // Open a temporary file for writing
    FILE* tempFile = fopen("temp.txt", "w");
    if (tempFile == NULL) {
        printf("Error opening temp.txt file.\n");
        fclose(booksFile);
        fclose(borrowedFile);
        return;
    }

    char line[500];
    int found = 0;

    // Iterate through each line in the borrowed books file
    while (fgets(line, sizeof(line), borrowedFile)) {
        char name[100], surname[100], bTitle[100], bAuthor[100];
        int copies;

        // Parse the line to extract book information
        if (sscanf(line, "%s %s %s %s %d", name, surname, bTitle, bAuthor, &copies) == 5) {
            // Check if the book matches the one being returned by the user
            if (strcmp(name, username) == 0 && strcmp(surname, username1) == 0 &&
                strcmp(bTitle, bookTitle) == 0 && strcmp(bAuthor, bookAuthor) == 0) {
                // Update the available copies of the book
                found = 1;

                if (numCopies > copies) {
                    printf("Error: Cannot return more copies than borrowed.\n");
                    fclose(borrowedFile);
                    fclose(booksFile);
                    fclose(tempFile);
                    remove("temp.txt");
                    return;
                }

                copies -= numCopies;

                // Check if the remaining copies are negative
                if (copies <= 0) {
                    // Skip writing to temp file, effectively removing the line
                    continue;
                }

                // Write the updated book information to the temporary file
                fprintf(tempFile, "%s %s %s %s %d\n", name, surname, bTitle, bAuthor, copies);
            }
            else {
                // Write unchanged borrowed book information to the temporary file
                fprintf(tempFile, "%s", line);
            }
        }
    }


    // Close the files
    fclose(borrowedFile);
    fclose(tempFile);

    // Remove the original borrowed books file and rename the temporary file
    remove("borrowedbooks.txt");
    rename("temp.txt", "borrowedbooks.txt");

    if (found) {
        fseek(booksFile, 0, SEEK_SET); // Move the file pointer to the beginning
        FILE* tempFile = fopen("temp.txt", "w"); // Open a temporary file for writing
        if (tempFile == NULL) {
            printf("Error opening temp.txt file.\n");
            fclose(booksFile);
            fclose(borrowedFile);
            return;
        }
        char tempLine[500];
        while (fgets(tempLine, sizeof(tempLine), booksFile)) {
            char tempTitle[100], tempAuthor[100];
            int tempCopies;
            if (sscanf(tempLine, "%s %s %d", tempTitle, tempAuthor, &tempCopies) == 3) {
                if (strcmp(tempTitle, bookTitle) == 0 && strcmp(tempAuthor, bookAuthor) == 0) {
                    tempCopies += numCopies; // Increment available copies
                    fprintf(tempFile, "%s %s %d\n", tempTitle, tempAuthor, tempCopies); // Write updated line to temp file
                }
                else {
                    fprintf(tempFile, "%s", tempLine); // Write unchanged line to temp file
                }
            }
        }
        fclose(tempFile);
        fclose(booksFile);
        remove("books.txt"); // Remove the original books file
        rename("temp.txt", "books.txt"); // Rename the temp file to books.txt
    }
    else {
        printf("Book '%s' by '%s' not found in the library.\n", bookTitle, bookAuthor);
    }


    if (!found) {
        printf("No matching book found for the user.\n");
    }
    else {
        printf("Books returned successfully.\n");
    }
}

void printUserBooks(const char* username, const char* surname) {
    FILE* file = fopen("borrowedbooks.txt", "r");
    if (file == NULL) {
        printf("Error opening borrowedbooks.txt file.\n");
        return;
    }
    printf("User: %s %s\n", username, surname);
    char line[500];
    while (fgets(line, sizeof(line), file)) {
        char name[100], userSurname[100], bookTitle[100], bookAuthor[100];
        int numCopies;

        if (sscanf(line, "%s %s %s %s %d", name, userSurname, bookTitle, bookAuthor, &numCopies) == 5) {
            if (strcmp(name, username) == 0 && strcmp(userSurname, surname) == 0) {
                printf("Book: % s by % s, Copies : % d\n",bookTitle, bookAuthor, numCopies);
            }
        }
    }

    fclose(file);
}

void donateBook(const char* bookTitle, const char* bookAuthor, int numCopies) {
    FILE* file = fopen("books.txt", "r+");
    if (file == NULL) {
        printf("Error opening books.txt file.\n");
        return;
    }

    FILE* tempFile = fopen("temp.txt", "w");
    if (tempFile == NULL) {
        printf("Error opening temp.txt file.\n");
        fclose(file);
        return;
    }

    char line[500];
    int found = 0;
    while (fgets(line, sizeof(line), file)) {
        char tempTitle[100], tempAuthor[100];
        int tempCopies;
        if (sscanf(line, "%s %s %d", tempTitle, tempAuthor, &tempCopies) == 3) {
            if (strcmp(tempTitle, bookTitle) == 0 && strcmp(tempAuthor, bookAuthor) == 0) {
                fprintf(tempFile, "%s %s %d\n", tempTitle, tempAuthor, tempCopies + numCopies); // Update available copies
                found = 1;
            }
            else {
                fprintf(tempFile, "%s", line); // Copy unchanged line to temp file
            }
        }
    }

    if (!found) {
        // If the book is not found, append it to the end of the file
        fprintf(tempFile, "%s %s %d\n", bookTitle, bookAuthor, numCopies);
    }

    fclose(file);
    fclose(tempFile);

    remove("books.txt");
    rename("temp.txt", "books.txt");
}

void viewAllBooks() {
    FILE* file = fopen("books.txt", "r");
    if (file == NULL) {
        printf("Error opening file.\n");
        return;
    }

    char line[500];
    while (fgets(line, sizeof(line), file)) {
        printf("%s", line);
    }

    fclose(file);
}
