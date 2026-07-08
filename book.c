#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "book.h"

Book library[MAX_BOOKS];
int bookCount = 0;

void printTableHeader() {
    printf("%-6s %-24s %-20s %-12s %-5s %-9s\n", "ID", "Title", "Author", "Category", "Qty", "Price");
}

void printBookRow(Book b) {
    printf("%-6d %-24s %-20s %-12s %-5d %.2f\n", 
           b.bookID, b.title, b.author, b.category, b.quantity, b.price);
}

int findBookByID(int id) {
    for (int i = 0; i < bookCount; i++) {
        if (library[i].bookID == id) {
            return i;
        }
    }
    return -1;
}

void saveBooks() {
    FILE *fp = fopen(FILENAME, "wb");
    if (fp != NULL) {
        fwrite(&bookCount, sizeof(int), 1, fp);
        fwrite(library, sizeof(Book), bookCount, fp);
        fclose(fp);
    }
}

void loadBooks() {
    FILE *fp = fopen(FILENAME, "rb");
    if (fp != NULL) {
        fread(&bookCount, sizeof(int), 1, fp);
        fread(library, sizeof(Book), bookCount, fp);
        fclose(fp);
    } else {
        bookCount = 0; 
    }
}

void addBook() {
    Book b;
    
    if (bookCount >= MAX_BOOKS) {
        printf("Library is full!\n");
        return;
    }

    printf("\n--- ADD NEW BOOK ---\n");
    
    printf("Enter Book ID: ");
    scanf("%d", &b.bookID);

    if (findBookByID(b.bookID) != -1) {
        printf("Error: Book ID already exists!\n");
        return;
    }

    printf("Enter Title: ");
    scanf(" %[^\n]", b.title); 

    printf("Enter Author: ");
    scanf(" %[^\n]", b.author);

    printf("Enter Publisher: ");
    scanf(" %[^\n]", b.publisher);

    printf("Enter Category: ");
    scanf(" %[^\n]", b.category);

    printf("Enter Quantity: ");
    scanf("%d", &b.quantity);

    printf("Enter Price: ");
    scanf("%f", &b.price);

    library[bookCount] = b;
    bookCount++;
    
    saveBooks();
    printf("Book added successfully!\n");
}

void displayBooks() {
    printf("\n--- ALL BOOKS ---\n");
    if (bookCount == 0) {
        printf("No books available.\n");
        return;
    }
    
    printTableHeader();
    for (int i = 0; i < bookCount; i++) {
        printBookRow(library[i]);
    }
}

void searchBook() {
    int choice, id, found = 0;
    char keyword[100];

    printf("\n--- SEARCH BOOK ---\n");
    printf("1. Search by ID\n");
    printf("2. Search by Title\n");
    printf("Enter choice: ");
    scanf("%d", &choice);

    if (choice == 1) {
        printf("Enter Book ID: ");
        scanf("%d", &id);
        
        int i = findBookByID(id);
        if (i != -1) {
            printTableHeader();
            printBookRow(library[i]);
        } else {
            printf("Book not found.\n");
        }
        
    } else if (choice == 2) {
        printf("Enter Title Keyword: ");
        scanf(" %[^\n]", keyword);
        
        printTableHeader();
        for (int i = 0; i < bookCount; i++) {
            if (strstr(library[i].title, keyword) != NULL) {
                printBookRow(library[i]);
                found = 1;
            }
        }
        if (found == 0) {
            printf("No matching books found.\n");
        }
    } else {
        printf("Invalid choice!\n");
    }
}

void updateBook() {
    int id;
    
    printf("\n--- UPDATE BOOK ---\n");
    printf("Enter Book ID to update: ");
    scanf("%d", &id);

    int idx = findBookByID(id);
    if (idx == -1) {
        printf("Book not found.\n");
        return;
    }

    printf("Enter New Title: ");
    scanf(" %[^\n]", library[idx].title);
    
    printf("Enter New Author: ");
    scanf(" %[^\n]", library[idx].author);
    
    printf("Enter New Publisher: ");
    scanf(" %[^\n]", library[idx].publisher);
    
    printf("Enter New Category: ");
    scanf(" %[^\n]", library[idx].category);
    
    printf("Enter New Quantity: ");
    scanf("%d", &library[idx].quantity);
    
    printf("Enter New Price: ");
    scanf("%f", &library[idx].price);

    saveBooks();
    printf("Book updated successfully!\n");
}

void deleteBook() {
    int id;
    
    printf("\n--- DELETE BOOK ---\n");
    printf("Enter Book ID to delete: ");
    scanf("%d", &id);

    int idx = findBookByID(id);
    if (idx == -1) {
        printf("Book not found.\n");
        return;
    }

    for (int i = idx; i < bookCount - 1; i++) {
        library[i] = library[i + 1];
    }
    
    bookCount--;
    saveBooks();
    printf("Book deleted successfully!\n");
}

void issueBook() {
    int id;
    
    printf("\n--- ISSUE BOOK ---\n");
    printf("Enter Book ID: ");
    scanf("%d", &id);

    int idx = findBookByID(id);
    if (idx != -1 && library[idx].quantity > 0) {
        library[idx].quantity--;
        saveBooks();
        printf("Book issued successfully! Remaining copies: %d\n", library[idx].quantity);
    } else {
        printf("Error: Book not found or out of stock.\n");
    }
}

void returnBook() {
    int id;
    
    printf("\n--- RETURN BOOK ---\n");
    printf("Enter Book ID: ");
    scanf("%d", &id);

    int idx = findBookByID(id);
    if (idx != -1) {
        library[idx].quantity++;
        saveBooks();
        printf("Book returned successfully! Total copies: %d\n", library[idx].quantity);
    } else {
        printf("Error: Book not found.\n");
    }
}

void statistics() {
    float totalValue = 0.0;
    int totalCopies = 0;

    for (int i = 0; i < bookCount; i++) {
        totalCopies += library[i].quantity;
        totalValue += (library[i].quantity * library[i].price);
    }

    printf("\n--- LIBRARY STATISTICS ---\n");
    printf("Total Unique Books : %d\n", bookCount);
    printf("Total Copies       : %d\n", totalCopies);
    printf("Total Value        : Rs. %.2f\n", totalValue);
}
