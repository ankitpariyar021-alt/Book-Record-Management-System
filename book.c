/*
 * ================================================================
 * book.c  —  All function implementations
 * ================================================================
 * Author  : [Your Name]
 * Roll No : [Your Roll Number]
 * Course  : BCA — C Programming Project
 * Subject : Book Record Management System
 *
 * SECTIONS IN THIS FILE:
 *   1. Global variable definitions
 *   2. Utility / helper functions
 *   3. File I/O  (saveBooks, loadBooks)
 *   4. Core CRUD (addBook, displayBooks, searchBook,
 *                  updateBook, deleteBook)
 *   5. Issue / Return
 *   6. Statistics
 *
 * Compile:  gcc -Wall -ansi main.c book.c -o BookSystem
 * ================================================================
 */

#include <stdio.h>   /* printf, scanf, fopen, fread, fwrite, fgets  */
#include <stdlib.h>  /* atoi(), atof()                               */
#include <string.h>  /* strcpy, strncpy, strstr, strcspn, strlen     */
#include <ctype.h>   /* tolower()                                    */
#include "book.h"    /* Our own header: struct Book, prototypes      */


/* ================================================================
 * SECTION 1 — GLOBAL VARIABLE DEFINITIONS
 * ================================================================
 * 'extern' in book.h says "these exist somewhere".
 * HERE is where the actual memory is allocated.
 * ================================================================ */

Book library[MAX_BOOKS]; /* Array that holds all book records in RAM */
int  bookCount = 0;      /* Number of books currently in library[]   */


/* ================================================================
 * SECTION 2 — UTILITY / HELPER FUNCTIONS
 * ================================================================ */

/*
 * strCmpCI()  —  Case-Insensitive String Compare
 * -----------------------------------------------
 * WHY: POSIX has strcasecmp() but it is NOT in ANSI C standard.
 *      Writing our own keeps the code portable and -ansi compliant.
 *
 * HOW: Compare characters one by one after converting both to
 *      lowercase with tolower().
 *
 * RETURNS: 0 if strings are equal (case-ignored)
 *          negative if s1 < s2
 *          positive if s1 > s2
 *
 * EXAMPLE: strCmpCI("hello", "HELLO") returns 0
 */
int strCmpCI(const char *s1, const char *s2) {
    unsigned char c1, c2;

    while (*s1 != '\0' && *s2 != '\0') {
        c1 = (unsigned char)tolower((unsigned char)*s1);
        c2 = (unsigned char)tolower((unsigned char)*s2);
        if (c1 != c2)
            return (int)c1 - (int)c2;
        s1++;
        s2++;
    }
    /* Handle strings of different lengths */
    return (int)tolower((unsigned char)*s1)
         - (int)tolower((unsigned char)*s2);
}

/*
 * clearInputBuffer()
 * ------------------
 * THE PROBLEM this solves:
 *   scanf("%d", &x) reads the number but leaves '\n' (Enter key)
 *   sitting in the input buffer. The next fgets() call then picks
 *   up that '\n' and returns immediately with an empty string.
 *
 * THE SOLUTION:
 *   Read and throw away characters until we consume the '\n'.
 *
 * CALL THIS after every scanf() that reads a number or single char.
 */
void clearInputBuffer(void) {
    int c;
    while ((c = getchar()) != '\n' && c != EOF)
        ; /* Deliberately empty — just consume characters */
}

/*
 * printDivider()
 * --------------
 * Prints 'width' copies of character 'ch', then a newline.
 *
 * EXAMPLE: printDivider('=', 10) outputs "==========\n"
 *          printDivider('-', 10) outputs "----------\n"
 */
void printDivider(char ch, int width) {
    int i;
    for (i = 0; i < width; i++)
        putchar(ch);
    putchar('\n');
}

/*
 * printTableHeader()
 * ------------------
 * Prints the column headings row for the book display table.
 * The column widths MUST match those in printBookRow() exactly,
 * otherwise the table will look misaligned.
 */
void printTableHeader(void) {
    printDivider('=', LINE_WIDTH);
    printf("  %-6s  %-24s  %-20s  %-12s  %-5s  %9s\n",
           "ID", "Title", "Author", "Category", "Qty", "Price(Rs)");
    printDivider('-', LINE_WIDTH);
}

/*
 * printBookRow()
 * --------------
 * Prints ONE book record as a single formatted table row.
 *
 * PARAMETER:  Book *b  — pointer to a Book struct
 *   WHY pointer? Passing a struct by value copies all ~360 bytes.
 *   Passing a pointer copies just 4-8 bytes (the address).
 *   Since we only READ the data here (not modify), 'const' would
 *   also be correct, but pointer alone is fine for a BCA project.
 *
 * Long strings are VISUALLY truncated for the table column —
 * the actual data stored in the struct is NOT changed.
 */
void printBookRow(Book *b) {
    char shortTitle[25];   /* Display-only truncated copies */
    char shortAuthor[21];
    char shortCat[13];

    strncpy(shortTitle,  b->title,    24); shortTitle[24]  = '\0';
    strncpy(shortAuthor, b->author,   20); shortAuthor[20] = '\0';
    strncpy(shortCat,    b->category, 12); shortCat[12]    = '\0';

    printf("  %-6d  %-24s  %-20s  %-12s  %-5d  %9.2f\n",
           b->bookID, shortTitle, shortAuthor,
           shortCat, b->quantity, b->price);
}

/*
 * findBookByID()
 * --------------
 * Linear search: checks every element of library[] in order.
 *
 * WHY linear search?
 *   The array is NOT sorted by ID (insertion order), so we can't
 *   use binary search. Linear search is O(n). For 500 books it's
 *   instant on any modern machine.
 *
 * RETURNS: index 0..bookCount-1 if found, -1 if not found.
 *
 * IMPORTANT — we return the INDEX, not a pointer. This lets the
 * caller do  library[index].quantity--  to modify in place.
 */
int findBookByID(int id) {
    int i;
    for (i = 0; i < bookCount; i++) {
        if (library[i].bookID == id)
            return i;   /* Found — return position in array */
    }
    return -1;          /* Not found */
}


/* ================================================================
 * SECTION 3 — FILE I/O FUNCTIONS
 * ================================================================
 * WHY BINARY FILES?
 *   - fwrite() copies raw struct bytes directly to disk.
 *     No formatting, no conversion — very fast.
 *   - fread() reads them back identically.
 *   - Works correctly with int, float, and char arrays.
 *   - Text files would require parsing logic (sscanf etc.).
 *
 * FILE FORMAT of books.dat:
 *   Bytes 0-3       : int bookCount (number of records)
 *   Bytes 4 onwards : bookCount × sizeof(Book) bytes
 * ================================================================ */

/*
 * saveBooks()
 * -----------
 * Writes library[] to books.dat in binary mode ("wb").
 * Called silently after every operation that modifies data.
 * This ensures data is never lost even if the program crashes.
 */
void saveBooks(void) {
    FILE *fp;

    fp = fopen(FILENAME, "wb"); /* "wb" = write binary, overwrite if exists */
    if (fp == NULL) {
        printf("\n  [ERROR] Cannot open '%s' for writing.\n", FILENAME);
        printf("          Check that you have write permission "
               "in this folder.\n");
        return;
    }

    /* Step 1: Write the count so loadBooks() knows how many to read */
    fwrite(&bookCount, sizeof(int), 1, fp);

    /* Step 2: Write all Book structs in a single call */
    fwrite(library, sizeof(Book), (size_t)bookCount, fp);

    fclose(fp);
    /* Silent save — no message. It's called automatically. */
}

/*
 * loadBooks()
 * -----------
 * Reads books.dat into library[] at program startup.
 * If the file does not exist (first-ever run), starts fresh
 * with bookCount = 0 — no error, perfectly normal.
 */
void loadBooks(void) {
    FILE  *fp;
    int    itemsRead;

    fp = fopen(FILENAME, "rb"); /* "rb" = read binary */
    if (fp == NULL) {
        bookCount = 0; /* First run — file doesn't exist yet */
        return;
    }

    /* Read the saved count */
    fread(&bookCount, sizeof(int), 1, fp);

    /* Safety check: corrupted count could overflow the array */
    if (bookCount < 0 || bookCount > MAX_BOOKS) {
        printf("\n  [WARNING] Data file appears corrupted. "
               "Starting with empty library.\n");
        bookCount = 0;
        fclose(fp);
        return;
    }

    /* Read all book records */
    itemsRead = (int)fread(library, sizeof(Book), (size_t)bookCount, fp);

    /* Verify we actually got as many records as expected */
    if (itemsRead != bookCount) {
        printf("\n  [WARNING] Expected %d records, read only %d. "
               "File may be incomplete.\n", bookCount, itemsRead);
        bookCount = itemsRead;
    }

    fclose(fp);
}


/* ================================================================
 * SECTION 4 — CORE CRUD FUNCTIONS
 * ================================================================ */

/*
 * addBook()
 * ---------
 * Collects 7 fields from the user, validates every field,
 * then appends the new book to library[] and saves.
 *
 * KEY CONCEPTS DEMONSTRATED:
 *   - struct variable (newBook) declared locally on the stack
 *   - fgets() for safe string input (prevents buffer overflow)
 *   - strcspn() to strip the trailing '\n' from fgets output
 *   - Struct assignment: library[bookCount] = newBook;
 *     (copies ALL fields in one statement)
 */
void addBook(void) {
    Book newBook;
    int  dupIdx;

    printf("\n");
    printDivider('=', LINE_WIDTH);
    printf("  ADD NEW BOOK\n");
    printDivider('=', LINE_WIDTH);

    /* Guard: is there still space in the array? */
    if (bookCount >= MAX_BOOKS) {
        printf("  [ERROR] Library is full (%d/%d records). "
               "Cannot add more.\n", bookCount, MAX_BOOKS);
        return;
    }

    /* ---- Book ID ---- */
    printf("  Enter Book ID        : ");
    if (scanf("%d", &newBook.bookID) != 1) {
        printf("  [ERROR] Book ID must be a whole number.\n");
        clearInputBuffer();
        return;
    }
    clearInputBuffer(); /* IMPORTANT: flush '\n' before fgets calls below */

    if (newBook.bookID <= 0) {
        printf("  [ERROR] Book ID must be a positive number (> 0).\n");
        return;
    }
    dupIdx = findBookByID(newBook.bookID);
    if (dupIdx != -1) {
        printf("  [ERROR] Book ID %d is already used by \"%s\".\n",
               newBook.bookID, library[dupIdx].title);
        printf("          Every book must have a UNIQUE ID.\n");
        return;
    }

    /* ---- Title ---- */
    printf("  Enter Title          : ");
    fgets(newBook.title, sizeof(newBook.title), stdin);
    newBook.title[strcspn(newBook.title, "\n")] = '\0'; /* Remove '\n' */
    if (strlen(newBook.title) == 0) {
        printf("  [ERROR] Title cannot be empty.\n");
        return;
    }

    /* ---- Author ---- */
    printf("  Enter Author         : ");
    fgets(newBook.author, sizeof(newBook.author), stdin);
    newBook.author[strcspn(newBook.author, "\n")] = '\0';
    if (strlen(newBook.author) == 0) {
        printf("  [ERROR] Author name cannot be empty.\n");
        return;
    }

    /* ---- Publisher ---- */
    printf("  Enter Publisher      : ");
    fgets(newBook.publisher, sizeof(newBook.publisher), stdin);
    newBook.publisher[strcspn(newBook.publisher, "\n")] = '\0';

    /* ---- Category ---- */
    printf("  Enter Category       : ");
    fgets(newBook.category, sizeof(newBook.category), stdin);
    newBook.category[strcspn(newBook.category, "\n")] = '\0';

    /* ---- Quantity ---- */
    printf("  Enter Quantity       : ");
    if (scanf("%d", &newBook.quantity) != 1) {
        printf("  [ERROR] Quantity must be a whole number.\n");
        clearInputBuffer();
        return;
    }
    clearInputBuffer();
    if (newBook.quantity < 0) {
        printf("  [ERROR] Quantity cannot be negative.\n");
        return;
    }

    /* ---- Price ---- */
    printf("  Enter Price (Rs.)    : ");
    if (scanf("%f", &newBook.price) != 1) {
        printf("  [ERROR] Price must be a number (e.g. 250 or 199.50).\n");
        clearInputBuffer();
        return;
    }
    clearInputBuffer();
    if (newBook.price < 0.0f) {
        printf("  [ERROR] Price cannot be negative.\n");
        return;
    }

    /* ---- Append to array ---- */
    /*
     * Struct assignment copies ALL fields in one go.
     * Equivalent to individually copying each field.
     */
    library[bookCount] = newBook;
    bookCount++;

    saveBooks(); /* Persist to disk immediately */

    printf("\n  [SUCCESS] \"%s\" added. Library now has %d record(s).\n",
           newBook.title, bookCount);
    printDivider('-', LINE_WIDTH);
}


/*
 * displayBooks()
 * --------------
 * Prints every record in library[] as a neat table.
 * Uses the pointer-based printBookRow() to avoid struct copying.
 * If no books exist, prints a helpful message instead.
 */
void displayBooks(void) {
    int i;

    printf("\n");
    printDivider('=', LINE_WIDTH);
    printf("  ALL BOOKS  [Total records: %d]\n", bookCount);
    printTableHeader();

    if (bookCount == 0) {
        printf("  No books in the system yet. "
               "Choose option 1 to add books.\n");
        printDivider('=', LINE_WIDTH);
        return;
    }

    for (i = 0; i < bookCount; i++) {
        printBookRow(&library[i]); /* &library[i] = address of i-th Book */
    }
    printDivider('=', LINE_WIDTH);
}


/*
 * searchBook()
 * ------------
 * Provides two search modes:
 *
 *   Mode 1 — By ID  : exact integer match using findBookByID()
 *   Mode 2 — By Title: partial, case-insensitive keyword match
 *
 * TITLE SEARCH TECHNIQUE:
 *   We build a lowercase copy of the keyword and a lowercase copy
 *   of each title, then use strstr() to check if the keyword
 *   appears anywhere inside the title.
 *   Example: keyword "prog" matches "C Programming Language"
 */
void searchBook(void) {
    int  choice, id, found, i, j;
    char keyword[100];
    char lowerKey[100];
    char lowerTitle[100];

    printf("\n");
    printDivider('=', LINE_WIDTH);
    printf("  SEARCH BOOK\n");
    printDivider('-', LINE_WIDTH);
    printf("  1. Search by Book ID  (exact match)\n");
    printf("  2. Search by Title    (keyword, case-insensitive)\n");
    printf("  Enter choice: ");

    if (scanf("%d", &choice) != 1) {
        printf("  [ERROR] Invalid input.\n");
        clearInputBuffer();
        return;
    }
    clearInputBuffer();

    /* ---- Mode 1: Search by ID ---- */
    if (choice == 1) {
        printf("  Enter Book ID: ");
        if (scanf("%d", &id) != 1) {
            printf("  [ERROR] Invalid ID.\n");
            clearInputBuffer();
            return;
        }
        clearInputBuffer();

        i = findBookByID(id);
        if (i == -1) {
            printf("\n  [NOT FOUND] No book with ID %d exists.\n", id);
        } else {
            printf("\n  Record found:\n");
            printTableHeader();
            printBookRow(&library[i]);
            printDivider('-', LINE_WIDTH);
            printf("  Full details:\n");
            printf("    Publisher  : %s\n", library[i].publisher);
            printf("    Category   : %s\n", library[i].category);
            printDivider('=', LINE_WIDTH);
        }

    /* ---- Mode 2: Search by Title keyword ---- */
    } else if (choice == 2) {
        printf("  Enter title keyword: ");
        fgets(keyword, sizeof(keyword), stdin);
        keyword[strcspn(keyword, "\n")] = '\0';

        if (strlen(keyword) == 0) {
            printf("  [ERROR] Keyword cannot be empty.\n");
            return;
        }

        /* Convert keyword to all-lowercase */
        for (j = 0; keyword[j] != '\0'; j++)
            lowerKey[j] = (char)tolower((unsigned char)keyword[j]);
        lowerKey[j] = '\0';

        printf("\n  Results matching \"%s\":\n", keyword);
        printTableHeader();

        found = 0;
        for (i = 0; i < bookCount; i++) {
            /* Convert this title to all-lowercase */
            for (j = 0; library[i].title[j] != '\0'; j++)
                lowerTitle[j] = (char)tolower(
                                    (unsigned char)library[i].title[j]);
            lowerTitle[j] = '\0';

            /* strstr returns non-NULL if lowerKey found in lowerTitle */
            if (strstr(lowerTitle, lowerKey) != NULL) {
                printBookRow(&library[i]);
                found = 1;
            }
        }

        if (!found)
            printf("  No books match the keyword \"%s\".\n", keyword);
        printDivider('=', LINE_WIDTH);

    } else {
        printf("  [ERROR] Invalid choice. Enter 1 or 2.\n");
    }
}


/*
 * updateBook()
 * ------------
 * Lets the user modify any or all fields of an existing book.
 *
 * UX DESIGN:
 *   - Shows current value in square brackets for each field.
 *   - If the user just presses Enter (empty input), the old
 *     value is kept unchanged.
 *   - This way the user only needs to retype fields they want
 *     to change.
 *
 * TECHNIQUE for "press Enter to keep":
 *   fgets() reads into a temporary buffer 'buf'.
 *   After stripping '\n', if strlen(buf) == 0, user pressed Enter.
 *   Only copy to library[idx] if user actually typed something.
 */
void updateBook(void) {
    int   idx, id;
    char  buf[150]; /* Temporary buffer for reading new values */
    int   newInt;
    float newFloat;

    printf("\n");
    printDivider('=', LINE_WIDTH);
    printf("  UPDATE BOOK\n");
    printDivider('-', LINE_WIDTH);

    printf("  Enter Book ID to update: ");
    if (scanf("%d", &id) != 1) {
        printf("  [ERROR] Invalid ID.\n");
        clearInputBuffer();
        return;
    }
    clearInputBuffer();

    idx = findBookByID(id);
    if (idx == -1) {
        printf("  [NOT FOUND] No book with ID %d.\n", id);
        return;
    }

    printf("\n  Current record:\n");
    printTableHeader();
    printBookRow(&library[idx]);
    printDivider('-', LINE_WIDTH);
    printf("  Tip: Press Enter to keep the current value.\n\n");

    /* ---- Title ---- */
    printf("  Title      [%s]:\n  > ", library[idx].title);
    fgets(buf, sizeof(buf), stdin);
    buf[strcspn(buf, "\n")] = '\0';
    if (strlen(buf) > 0) {
        strncpy(library[idx].title, buf, sizeof(library[idx].title) - 1);
        library[idx].title[sizeof(library[idx].title) - 1] = '\0';
    }

    /* ---- Author ---- */
    printf("  Author     [%s]:\n  > ", library[idx].author);
    fgets(buf, sizeof(buf), stdin);
    buf[strcspn(buf, "\n")] = '\0';
    if (strlen(buf) > 0) {
        strncpy(library[idx].author, buf, sizeof(library[idx].author) - 1);
        library[idx].author[sizeof(library[idx].author) - 1] = '\0';
    }

    /* ---- Publisher ---- */
    printf("  Publisher  [%s]:\n  > ", library[idx].publisher);
    fgets(buf, sizeof(buf), stdin);
    buf[strcspn(buf, "\n")] = '\0';
    if (strlen(buf) > 0) {
        strncpy(library[idx].publisher, buf,
                sizeof(library[idx].publisher) - 1);
        library[idx].publisher[sizeof(library[idx].publisher) - 1] = '\0';
    }

    /* ---- Category ---- */
    printf("  Category   [%s]:\n  > ", library[idx].category);
    fgets(buf, sizeof(buf), stdin);
    buf[strcspn(buf, "\n")] = '\0';
    if (strlen(buf) > 0) {
        strncpy(library[idx].category, buf,
                sizeof(library[idx].category) - 1);
        library[idx].category[sizeof(library[idx].category) - 1] = '\0';
    }

    /* ---- Quantity ---- */
    printf("  Quantity   [%d]:\n  > ", library[idx].quantity);
    fgets(buf, sizeof(buf), stdin);
    buf[strcspn(buf, "\n")] = '\0';
    if (strlen(buf) > 0) {
        newInt = atoi(buf);
        if (newInt < 0)
            printf("  [WARNING] Negative value rejected. "
                   "Quantity kept as %d.\n", library[idx].quantity);
        else
            library[idx].quantity = newInt;
    }

    /* ---- Price ---- */
    printf("  Price      [%.2f]:\n  > ", library[idx].price);
    fgets(buf, sizeof(buf), stdin);
    buf[strcspn(buf, "\n")] = '\0';
    if (strlen(buf) > 0) {
        newFloat = (float)atof(buf);
        if (newFloat < 0.0f)
            printf("  [WARNING] Negative value rejected. "
                   "Price kept as %.2f.\n", library[idx].price);
        else
            library[idx].price = newFloat;
    }

    saveBooks();
    printf("\n  [SUCCESS] Book ID %d updated successfully.\n", id);
    printDivider('-', LINE_WIDTH);
}


/*
 * deleteBook()
 * ------------
 * Permanently removes a book record after user confirmation.
 *
 * ARRAY SHIFTING TECHNIQUE (closing the gap):
 * ============================================
 * Suppose we delete index 2 from a 5-element array:
 *
 *   Before: [A][B][C][D][E]   (C is at index 2, to be deleted)
 *
 *   Loop iteration 1:  library[2] = library[3]  →  [A][B][D][D][E]
 *   Loop iteration 2:  library[3] = library[4]  →  [A][B][D][E][E]
 *
 *   bookCount-- makes the last slot invisible: [A][B][D][E]
 *
 * The struct assignment (library[i] = library[i+1]) copies ALL
 * fields of the struct in one statement.
 */
void deleteBook(void) {
    int  idx, id, i;
    char confirm;

    printf("\n");
    printDivider('=', LINE_WIDTH);
    printf("  DELETE BOOK\n");
    printDivider('-', LINE_WIDTH);

    printf("  Enter Book ID to delete: ");
    if (scanf("%d", &id) != 1) {
        printf("  [ERROR] Invalid ID.\n");
        clearInputBuffer();
        return;
    }
    clearInputBuffer();

    idx = findBookByID(id);
    if (idx == -1) {
        printf("  [NOT FOUND] No book with ID %d.\n", id);
        return;
    }

    /* Show the record so the user knows exactly what will be lost */
    printf("\n  Record to delete:\n");
    printTableHeader();
    printBookRow(&library[idx]);
    printDivider('-', LINE_WIDTH);

    printf("  This action CANNOT be undone. Confirm delete? (Y/N): ");
    scanf("%c", &confirm);
    clearInputBuffer();

    if (confirm != 'Y' && confirm != 'y') {
        printf("  Deletion cancelled. No changes made.\n");
        return;
    }

    /* Shift all elements after idx one position to the left */
    for (i = idx; i < bookCount - 1; i++) {
        library[i] = library[i + 1]; /* Struct copy */
    }
    bookCount--; /* The last element is now "unreachable" */

    saveBooks();
    printf("\n  [SUCCESS] Book ID %d deleted. Remaining records: %d\n",
           id, bookCount);
    printDivider('-', LINE_WIDTH);
}


/* ================================================================
 * SECTION 5 — ISSUE / RETURN FUNCTIONS
 * ================================================================ */

/*
 * issueBook()
 * -----------
 * Issues ONE copy of a book to a student.
 * Checks stock before issuing — cannot issue if quantity == 0.
 */
void issueBook(void) {
    int idx, id;

    printf("\n");
    printDivider('=', LINE_WIDTH);
    printf("  ISSUE BOOK\n");
    printDivider('-', LINE_WIDTH);

    printf("  Enter Book ID to issue: ");
    if (scanf("%d", &id) != 1) {
        printf("  [ERROR] Invalid ID.\n");
        clearInputBuffer();
        return;
    }
    clearInputBuffer();

    idx = findBookByID(id);
    if (idx == -1) {
        printf("  [NOT FOUND] No book with ID %d.\n", id);
        return;
    }

    printf("\n  Book   : %s\n",    library[idx].title);
    printf("  Author : %s\n",      library[idx].author);
    printf("  Stock  : %d cop%s currently available\n",
           library[idx].quantity,
           library[idx].quantity == 1 ? "y" : "ies");

    if (library[idx].quantity <= 0) {
        printf("\n  [ERROR] OUT OF STOCK. "
               "All copies of this book are issued out.\n");
        return;
    }

    library[idx].quantity--; /* Decrement stock by 1 */
    saveBooks();

    printf("\n  [SUCCESS] Book issued. Copies remaining: %d\n",
           library[idx].quantity);
    printDivider('-', LINE_WIDTH);
}


/*
 * returnBook()
 * ------------
 * Records the return of ONE copy of a book by a student.
 * Simply increments quantity by 1.
 */
void returnBook(void) {
    int idx, id;

    printf("\n");
    printDivider('=', LINE_WIDTH);
    printf("  RETURN BOOK\n");
    printDivider('-', LINE_WIDTH);

    printf("  Enter Book ID to return: ");
    if (scanf("%d", &id) != 1) {
        printf("  [ERROR] Invalid ID.\n");
        clearInputBuffer();
        return;
    }
    clearInputBuffer();

    idx = findBookByID(id);
    if (idx == -1) {
        printf("  [NOT FOUND] No book with ID %d.\n", id);
        return;
    }

    library[idx].quantity++; /* Increment stock by 1 */
    saveBooks();

    printf("\n  Book   : %s\n",     library[idx].title);
    printf("  [SUCCESS] Book returned. Updated stock: %d cop%s\n",
           library[idx].quantity,
           library[idx].quantity == 1 ? "y" : "ies");
    printDivider('-', LINE_WIDTH);
}


/* ================================================================
 * SECTION 6 — STATISTICS FUNCTION
 * ================================================================ */

/*
 * statistics()
 * ------------
 * Scans library[] to compute and display a summary report.
 *
 * METRICS CALCULATED:
 *   1. Total records    = bookCount
 *   2. Unique titles    = titles that don't repeat (nested loop)
 *   3. Total copies     = sum of all quantity values
 *   4. Inventory value  = sum of (quantity_i × price_i) for all i
 *   5. Most expensive   = record with the highest price
 *
 * UNIQUE TITLE ALGORITHM:
 *   For each book i, compare its title against all books 0..i-1.
 *   If no earlier book has the same title, it's a new unique title.
 *   Time complexity: O(n²) — acceptable for n ≤ 500.
 */
void statistics(void) {
    int   i, j;
    int   uniqueTitles = 0;
    int   totalCopies  = 0;
    int   isDuplicate;
    float totalValue   = 0.0f;
    float maxPrice     = -1.0f;
    int   maxIdx       = -1;

    printf("\n");
    printDivider('=', LINE_WIDTH);
    printf("  LIBRARY STATISTICS REPORT\n");
    printDivider('=', LINE_WIDTH);

    if (bookCount == 0) {
        printf("  No records in the system yet.\n");
        printDivider('=', LINE_WIDTH);
        return;
    }

    /* -- Count unique titles -- */
    for (i = 0; i < bookCount; i++) {
        isDuplicate = 0;
        for (j = 0; j < i; j++) {
            if (strCmpCI(library[i].title, library[j].title) == 0) {
                isDuplicate = 1;
                break; /* No need to check further */
            }
        }
        if (!isDuplicate)
            uniqueTitles++;
    }

    /* -- Sum copies, compute value, track most expensive -- */
    for (i = 0; i < bookCount; i++) {
        totalCopies += library[i].quantity;
        totalValue  += (float)library[i].quantity * library[i].price;

        if (library[i].price > maxPrice) {
            maxPrice = library[i].price;
            maxIdx   = i;
        }
    }

    /* -- Print the report -- */
    printDivider('-', LINE_WIDTH);
    printf("  Total Book Records    : %d\n",        bookCount);
    printf("  Unique Titles         : %d\n",        uniqueTitles);
    printf("  Total Copies in Stock : %d\n",        totalCopies);
    printf("  Total Inventory Value : Rs. %.2f\n",  totalValue);
    if (maxIdx >= 0) {
        printf("  Most Expensive Book   : %s\n",    library[maxIdx].title);
        printf("  Most Expensive Price  : Rs. %.2f\n", maxPrice);
    }
    printDivider('=', LINE_WIDTH);
}

/* ================================================================
 * End of book.c
 * ================================================================ */
