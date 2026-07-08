#ifndef BOOK_H
#define BOOK_H

#define MAX_BOOKS 500
#define FILENAME "books.dat"
#define LINE_WIDTH 92

typedef struct {
    int bookID;
    char title[100];
    char author[100];
    char publisher[100];
    char category[50];
    int quantity;
    float price;
} Book;

extern Book library[MAX_BOOKS];
extern int bookCount;

void addBook(void);
void displayBooks(void);
void searchBook(void);
void updateBook(void);
void deleteBook(void);

void issueBook(void);
void returnBook(void);

void statistics(void);

void saveBooks(void);
void loadBooks(void);

int findBookByID(int id);
void clearInputBuffer(void);
void printDivider(char ch, int width);
void printTableHeader(void);
void printBookRow(Book *b);
int strCmpCI(const char *s1, const char *s2);

#endif
