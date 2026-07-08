#include <stdio.h>
#include "book.h"

void displayMenu() {
    printf("\n");
    printf("BOOK RECORD MANAGEMENT SYSTEM \n");
    printf(" BCA Project \n");
    printf(" 1.  Add Book \n");
    printf(" 2.  Display All Books \n");
    printf(" 3.  Search Book \n");
    printf(" 4.  Update Book \n");
    printf(" 5.  Delete Book \n");
    printf(" 6.  Issue Book \n");
    printf(" 7.  Return Book \n");
    printf(" 8.  Statistics  \n");
    printf(" 9.  Save Data Manually \n");
    printf(" 0.  Exit \n");
    printf("  Your choice: ");
}

int main() {
    int choice;

    printf("\n");
    printf("  Welcome to the Book Record Management System (BRMS)\n");

    loadBooks();

    if (bookCount > 0) {
        printf("  %d record(s) loaded from '%s'.\n", bookCount, FILENAME);
    } else {
        printf("  No saved data found. Starting with an empty library.\n");
    }

    do {
        displayMenu();
        
        scanf("%d", &choice);
        clearInputBuffer(); 

        switch (choice) {
            case 1:
                addBook();
                break;
            case 2:
                displayBooks();
                break;
            case 3:
                searchBook();
                break;
            case 4:
                updateBook();
                break;
            case 5:
                deleteBook();
                break;
            case 6:
                issueBook();
                break;
            case 7:
                returnBook();
                break;
            case 8:
                statistics();
                break;
            case 9:
                saveBooks();
                printf("\n  [SUCCESS] All data saved to '%s'.\n", FILENAME);
                break;
            case 0:
                saveBooks();
                printf("\n  Data saved. Thank you for using BRMS!\n");
                printf("  Goodbye.\n\n");
                break;
            default:
                printf("\n  [ERROR] Invalid option. Please enter 0-9.\n");
                break;
        }

    } while (choice != 0);

    return 0;
}
