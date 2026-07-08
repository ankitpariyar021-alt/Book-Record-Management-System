#include <stdio.h>  
#include <stdlib.h>  
#include <string.h> 
#include <ctype.h>   
#include "book.h"   
Book library[MAX_BOOKS]; 
int  bookCount = 0;      
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
    return (int)tolower((unsigned char)*s1)
         - (int)tolower((unsigned char)*s2);
}

void clearInputBuffer(void) {
    int c;
    while ((c = getchar()) != '\n' && c != EOF)
        ; 
}
void printDivider(char ch, int width) {
    int i;
    for (i = 0; i < width; i++)
        putchar(ch);
    putchar('\n');
}
void printTableHeader(void) {
    printDivider('=', LINE_WIDTH);
    printf("  %-6s  %-24s  %-20s  %-12s  %-5s  %9s\n",
           "ID", "Title", "Author", "Category", "Qty", "Price(Rs)");
    printDivider('-', LINE_WIDTH);
}
void printBookRow(Book *b) {
    char shortTitle[25];  
    char shortAuthor[21];
    char shortCat[13];

    strncpy(shortTitle,  b->title,    24); shortTitle[24]  = '\0';
    strncpy(shortAuthor, b->author,   20); shortAuthor[20] = '\0';
    strncpy(shortCat,    b->category, 12); shortCat[12]    = '\0';

    printf("  %-6d  %-24s  %-20s  %-12s  %-5d  %9.2f\n",
           b->bookID, shortTitle, shortAuthor,
           shortCat, b->quantity, b->price);
}
int findBookByID(int id) {
    int i;
    for (i = 0; i < bookCount; i++) {
        if (library[i].bookID == id)
            return i; 
    }
    return -1;      
}
void saveBooks(void) {
    FILE *fp;

    fp = fopen(FILENAME, "wb"); 
    if (fp == NULL) {
        printf("\n  [ERROR] Cannot open '%s' for writing.\n", FILENAME);
        printf("Check that you have write permission "
               "in this folder.\n");
        return;
    }
    fwrite(&bookCount, sizeof(int), 1, fp);
    fwrite(library, sizeof(Book), (size_t)bookCount, fp);
    fclose(fp);
}
void loadBooks(void) {
    FILE  *fp;
    int    itemsRead;

    fp = fopen(FILENAME, "rb"); 
    if (fp == NULL) {
        bookCount = 0; 
        return;
    }
    fread(&bookCount, sizeof(int), 1, fp);
    if (bookCount < 0 || bookCount > MAX_BOOKS) {
        printf("\n  [WARNING] Data file appears corrupted. "
               "Starting with empty library.\n");
        bookCount = 0;
        fclose(fp);
        return;
    }
    itemsRead = (int)fread(library, sizeof(Book), (size_t)bookCount, fp);
    if (itemsRead != bookCount) {
        printf("\n  [WARNING] Expected %d records, read only %d. "
               "File may be incomplete.\n", bookCount, itemsRead);
        bookCount = itemsRead;
    }

    fclose(fp);
}
void addBook(void) {
    Book newBook;
    int  dupIdx;

    printf("\n");
    printDivider('=', LINE_WIDTH);
    printf("  ADD NEW BOOK\n");
    printDivider('=', LINE_WIDTH);
    if (bookCount >= MAX_BOOKS) {
        printf("  [ERROR] Library is full (%d/%d records). "
               "Cannot add more.\n", bookCount, MAX_BOOKS);
        return;
    }
    printf("  Enter Book ID: ");
    if (scanf("%d", &newBook.bookID) != 1) {
        printf("  [ERROR] Book ID must be a whole number.\n");
        clearInputBuffer();
        return;
    }
    clearInputBuffer(); 

    if (newBook.bookID <= 0) {
        printf("  [ERROR] Book ID must be a positive number (> 0).\n");
        return;
    }
    dupIdx = findBookByID(newBook.bookID);
    if (dupIdx != -1) {
        printf("  [ERROR] Book ID %d is already used by \"%s\".\n",
               newBook.bookID, library[dupIdx].title);
        printf("Every book must have a UNIQUE ID.\n");
        return;
    }
    printf("  Enter Title : ");
    fgets(newBook.title, sizeof(newBook.title), stdin);
    newBook.title[strcspn(newBook.title, "\n")] = '\0'; 
    if (strlen(newBook.title) == 0) {
        printf("  [ERROR] Title cannot be empty.\n");
        return;
    }
    printf("  Enter Author: ");
    fgets(newBook.author, sizeof(newBook.author), stdin);
    newBook.author[strcspn(newBook.author, "\n")] = '\0';
    if (strlen(newBook.author) == 0) {
        printf("  [ERROR] Author name cannot be empty.\n");
        return;
    }
    printf("  Enter Publisher: ");
    fgets(newBook.publisher, sizeof(newBook.publisher), stdin);
    newBook.publisher[strcspn(newBook.publisher, "\n")] = '\0';
    printf("  Enter Category : ");
    fgets(newBook.category, sizeof(newBook.category), stdin);
    newBook.category[strcspn(newBook.category, "\n")] = '\0';

    printf("  Enter Quantity: ");
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
    printf("  Enter Price (Rs.) : ");
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
    library[bookCount] = newBook;
    bookCount++;

    saveBooks(); 

    printf("\n  [SUCCESS] \"%s\" added. Library now has %d record(s).\n",
           newBook.title, bookCount);
    printDivider('-', LINE_WIDTH);
}
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
        printBookRow(&library[i]);
    }
    printDivider('=', LINE_WIDTH);
}

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
    printf("  2. Search by Title   (keyword, case-insensitive)\n");
    printf("  Enter choice: ");

    if (scanf("%d", &choice) != 1) {
        printf("  [ERROR] Invalid input.\n");
        clearInputBuffer();
        return;
    }
    clearInputBuffer();
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
        
    } else if (choice == 2) {
        printf("  Enter title keyword: ");
        fgets(keyword, sizeof(keyword), stdin);
        keyword[strcspn(keyword, "\n")] = '\0';

        if (strlen(keyword) == 0) {
            printf("  [ERROR] Keyword cannot be empty.\n");
            return;
        }

        for (j = 0; keyword[j] != '\0'; j++)
            lowerKey[j] = (char)tolower((unsigned char)keyword[j]);
        lowerKey[j] = '\0';

        printf("\n  Results matching \"%s\":\n", keyword);
        printTableHeader();

        found = 0;
        for (i = 0; i < bookCount; i++) {
            for (j = 0; library[i].title[j] != '\0'; j++)
                lowerTitle[j] = (char)tolower(
                                    (unsigned char)library[i].title[j]);
            lowerTitle[j] = '\0';
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
void updateBook(void) {
    int   idx, id;
    char  buf[150];
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

    printf("  Title      [%s]:\n  > ", library[idx].title);
    fgets(buf, sizeof(buf), stdin);
    buf[strcspn(buf, "\n")] = '\0';
    if (strlen(buf) > 0) {
        strncpy(library[idx].title, buf, sizeof(library[idx].title) - 1);
        library[idx].title[sizeof(library[idx].title) - 1] = '\0';
    }

    printf("  Author     [%s]:\n  > ", library[idx].author);
    fgets(buf, sizeof(buf), stdin);
    buf[strcspn(buf, "\n")] = '\0';
    if (strlen(buf) > 0) {
        strncpy(library[idx].author, buf, sizeof(library[idx].author) - 1);
        library[idx].author[sizeof(library[idx].author) - 1] = '\0';
    }
    printf("  Publisher  [%s]:\n  > ", library[idx].publisher);
    fgets(buf, sizeof(buf), stdin);
    buf[strcspn(buf, "\n")] = '\0';
    if (strlen(buf) > 0) {
        strncpy(library[idx].publisher, buf,
                sizeof(library[idx].publisher) - 1);
        library[idx].publisher[sizeof(library[idx].publisher) - 1] = '\0';
    }
    printf("  Category   [%s]:\n  > ", library[idx].category);
    fgets(buf, sizeof(buf), stdin);
    buf[strcspn(buf, "\n")] = '\0';
    if (strlen(buf) > 0) {
        strncpy(library[idx].category, buf,
                sizeof(library[idx].category) - 1);
        library[idx].category[sizeof(library[idx].category) - 1] = '\0';
    }
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
    for (i = idx; i < bookCount - 1; i++) {
        library[i] = library[i + 1];
    }
    bookCount--; 

    saveBooks();
    printf("\n  [SUCCESS] Book ID %d deleted. Remaining records: %d\n",
           id, bookCount);
    printDivider('-', LINE_WIDTH);
}
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

    library[idx].quantity--;
    saveBooks();

    printf("\n  [SUCCESS] Book issued. Copies remaining: %d\n",
           library[idx].quantity);
    printDivider('-', LINE_WIDTH);
}
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

    library[idx].quantity++;
    saveBooks();

    printf("\n  Book   : %s\n",     library[idx].title);
    printf("  [SUCCESS] Book returned. Updated stock: %d cop%s\n",
           library[idx].quantity,
           library[idx].quantity == 1 ? "y" : "ies");
    printDivider('-', LINE_WIDTH);
}
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
    for (i = 0; i < bookCount; i++) {
        isDuplicate = 0;
        for (j = 0; j < i; j++) {
            if (strCmpCI(library[i].title, library[j].title) == 0) {
                isDuplicate = 1;
                break;
            }
        }
        if (!isDuplicate)
            uniqueTitles++;
    }
    for (i = 0; i < bookCount; i++) {
        totalCopies += library[i].quantity;
        totalValue  += (float)library[i].quantity * library[i].price;

        if (library[i].price > maxPrice) {
            maxPrice = library[i].price;
            maxIdx   = i;
        }
    }
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
