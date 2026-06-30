/*
 * ================================================================
 * book.h  —  Header file for Book Record Management System
 * ================================================================
 * WHAT GOES IN A HEADER FILE?
 *   - struct/typedef definitions shared across multiple .c files
 *   - #define constants
 *   - extern declarations (variable exists in another .c file)
 *   - Function prototypes (tells compiler about functions in book.c)
 *
 * HOW TO USE:
 *   Both main.c and book.c include this file with:  #include "book.h"
 * ================================================================
 */

#ifndef BOOK_H   /* Include guard — prevents double-inclusion */
#define BOOK_H   /* If already included, skip everything below */


/* ================================================================
 * CONSTANTS  (#define creates a compile-time text substitution)
 * ================================================================ */
#define MAX_BOOKS  500          /* Max books the array can hold      */
#define FILENAME   "books.dat"  /* Binary file for persistent storage*/
#define LINE_WIDTH 92           /* Width of separator lines          */


/* ================================================================
 * BOOK STRUCTURE
 * ================================================================
 * 'typedef struct' creates a new type called 'Book'.
 * After this, we can write:  Book b;
 * instead of:                struct Book b;
 *
 * Each field stores one piece of data per book record.
 * ================================================================ */
typedef struct {
    int   bookID;           /* Unique numeric ID — must be > 0       */
    char  title[100];       /* Book title                            */
    char  author[100];      /* Author full name                      */
    char  publisher[100];   /* Publisher name                        */
    char  category[50];     /* Genre e.g. Science, Fiction, Maths    */
    int   quantity;         /* Copies in library  (must be >= 0)     */
    float price;            /* Price per copy Rs. (must be >= 0.0)   */
} Book;


/* ================================================================
 * GLOBAL VARIABLE DECLARATIONS  (extern)
 * ================================================================
 * 'extern' tells the compiler:
 *   "These variables exist. Don't allocate memory here —
 *    find them in book.c when linking."
 *
 * The actual memory is DEFINED (allocated) in book.c.
 * ================================================================ */
extern Book library[MAX_BOOKS]; /* In-memory array of all books      */
extern int  bookCount;          /* How many books are stored now     */


/* ================================================================
 * FUNCTION PROTOTYPES
 * ================================================================
 * A prototype tells the compiler a function's name, return type,
 * and parameter types before the function is actually defined.
 * This allows main.c to call functions defined in book.c.
 * ================================================================ */

/* --- Core CRUD operations --- */
void addBook(void);       /* Add a new book record (C = Create)      */
void displayBooks(void);  /* Show all books as a table   (R = Read)  */
void searchBook(void);    /* Find book by ID or title    (R = Read)  */
void updateBook(void);    /* Edit fields of a book       (U = Update)*/
void deleteBook(void);    /* Remove a book record        (D = Delete)*/

/* --- Issue / Return --- */
void issueBook(void);     /* Issue one copy to student (quantity--)  */
void returnBook(void);    /* Receive returned copy    (quantity++)   */

/* --- Reports --- */
void statistics(void);    /* Print totals, unique titles, value      */

/* --- File I/O --- */
void saveBooks(void);     /* Write library[] to books.dat (binary)   */
void loadBooks(void);     /* Read books.dat into library[] (binary)  */

/* --- Utility helpers (used internally by the functions above) --- */
int  findBookByID(int id);              /* Returns index or -1        */
void clearInputBuffer(void);            /* Flush stdin after scanf    */
void printDivider(char ch, int width);  /* Print a separator line     */
void printTableHeader(void);            /* Print column headings      */
void printBookRow(Book *b);             /* Print one book as a row    */
int  strCmpCI(const char *s1,           /* Case-insensitive strcmp    */
              const char *s2);          /* (replaces strcasecmp)      */


#endif /* BOOK_H */
