/*
 * ================================================================
 * main.c  —  Entry point for Book Record Management System
 * ================================================================
 * Author  : [Your Name]
 * Roll No : [Your Roll Number]
 * Course  : BCA — C Programming Project
 *
 * THIS FILE CONTAINS:
 *   displayMenu() — prints the main navigation menu
 *   main()        — loads data, runs the loop, exits cleanly
 *
 * PROGRAM FLOW:
 *   1. loadBooks()    → reads books.dat into library[]
 *   2. Loop begins    → displayMenu() → read choice
 *   3. switch/case    → calls the appropriate function
 *   4. choice == 0    → saveBooks() and exit
 *
 * TO COMPILE:
 *   gcc -Wall -ansi main.c book.c -o BookSystem
 *
 * TO RUN:
 *   ./BookSystem          (Linux / macOS terminal)
 *   BookSystem.exe        (Windows Command Prompt)
 * ================================================================
 */

#include <stdio.h>   /* printf, scanf                               */
#include "book.h"    /* struct Book, all prototypes, FILENAME const */


/* ================================================================
 * displayMenu()
 * =============
 * Prints the main menu to the screen.
 * Declared 'static' because it is only used inside this file
 * (main.c). 'static' gives it internal linkage — it won't
 * conflict with any identically named function in book.c.
 * ================================================================ */
static void displayMenu(void) {
    printf("\n");
    printf("  +======================================================+\n");
    printf("  |         BOOK RECORD MANAGEMENT SYSTEM               |\n");
    printf("  |                   BCA Project                       |\n");
    printf("  +======================================================+\n");
    printf("  |                                                      |\n");
    printf("  |   1.  Add Book                                       |\n");
    printf("  |   2.  Display All Books                              |\n");
    printf("  |   3.  Search Book                                    |\n");
    printf("  |   4.  Update Book                                    |\n");
    printf("  |   5.  Delete Book                                    |\n");
    printf("  |   6.  Issue Book                                     |\n");
    printf("  |   7.  Return Book                                    |\n");
    printf("  |   8.  Statistics                                     |\n");
    printf("  |   9.  Save Data Manually                             |\n");
    printf("  |   0.  Exit                                           |\n");
    printf("  |                                                      |\n");
    printf("  +======================================================+\n");
    printf("  Your choice: ");
}


/* ================================================================
 * main()
 * ======
 * Every C program starts execution from main().
 * 'int main(void)' is the ANSI C standard signature.
 * It returns an int to the operating system:
 *   return 0  → program finished successfully
 *   return 1  → program finished with an error
 * ================================================================ */
int main(void) {
    int choice; /* Stores the user's menu selection */

    /* Welcome banner */
    printf("\n");
    printf("  ======================================================\n");
    printf("  Welcome to the Book Record Management System (BRMS)\n");
    printf("  ======================================================\n");

    /* Load existing data from disk.
     * If books.dat doesn't exist (first run), loadBooks() silently
     * sets bookCount = 0 and returns — no error.                   */
    loadBooks();

    if (bookCount > 0)
        printf("  %d record(s) loaded from '%s'.\n",
               bookCount, FILENAME);
    else
        printf("  No saved data found. Starting with an empty library.\n");


    /* ==============================================================
     * MAIN MENU LOOP
     * ==============
     * 'do...while' ensures the menu is shown AT LEAST ONCE.
     * The loop continues as long as choice != 0 (Exit).
     * ============================================================== */
    do {
        displayMenu(); /* Print the menu and prompt */

        /* Read the user's choice.
         * scanf("%d") stores the integer in 'choice' and returns
         * the number of items successfully read (should be 1).    */
        if (scanf("%d", &choice) != 1) {
            /* Non-integer typed (e.g. letters): handle gracefully */
            printf("\n  [ERROR] Invalid input. "
                   "Please type a number between 0 and 9.\n");
            clearInputBuffer(); /* flush the bad input */
            choice = -1;        /* force loop to continue */
            continue;
        }
        clearInputBuffer(); /* Flush the '\n' left by scanf */

        /* Dispatch to the correct function based on choice */
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
                /* Manual save — data is already auto-saved after
                 * every operation, but this gives user peace of mind */
                saveBooks();
                printf("\n  [SUCCESS] All data saved to '%s'.\n",
                       FILENAME);
                break;

            case 0:
                /* Exit — save one final time then leave the loop */
                saveBooks();
                printf("\n  Data saved. Thank you for using BRMS!\n");
                printf("  Goodbye.\n\n");
                break;

            default:
                printf("\n  [ERROR] '%d' is not a valid option. "
                       "Please enter 0–9.\n", choice);
                break;
        }

    } while (choice != 0); /* Keep looping until user chooses Exit */

    return 0; /* Signal to OS that program exited cleanly */
}

/* ================================================================
 * End of main.c
 * ================================================================ */
