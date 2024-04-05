#ifndef LIBRARY_H
#define LIBRARY_H

#include "accounts.h"

// Functia pentru stergerea cartilor <= 0
void remove_zero_or_negative_copies(const char* filename);
// Funcția pentru căutarea cărților
void searchBooks(const char* keyword);
// Funcția pentru împrumut de cărți
void borrowBook(const char* username, const char* username1, const char* title, int numCopies);
// Functia pentru a returna carti
void returnBooks(const char* username, const char* username1, const char* bookTitle, const char* bookAuthor, int numCopies);
// Functia pentru a vizualiza cartile care trebuiesc returnate
void printUserBooks(const char* username, const char* surname);
// Functia pentru donare de carti
void donateBook(const char* bookTitle, const char* bookAuthor, int numCopies);
// Funcția pentru vizualizarea tuturor cărților din bibliotecă
void viewAllBooks();

#endif
