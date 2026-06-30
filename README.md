# Book Record Management System (BRMS)

## Project Details
| Field        | Detail                          |
|--------------|---------------------------------|
| Language     | C (ANSI C89 / C90 standard)     |
| Type         | Console-based, menu-driven      |
| Storage      | Binary file (`books.dat`)       |
| Course       | BCA — C Programming Project     |
| Author       | [Your Name]                     |
| Roll No.     | [Your Roll Number]              |

---

## Folder Structure
```
BookRecordSystem/
│
├── main.c       ← Entry point: main(), menu loop
├── book.c       ← All 10 function implementations
├── book.h       ← struct Book, constants, prototypes
├── Makefile     ← For easy compilation (Linux/macOS)
├── books.dat    ← Created automatically at runtime
└── README.md    ← This file
```

---

## How to Compile

### Linux / macOS (Terminal)
```bash
# Option 1 — Using Makefile
make

# Option 2 — Manual gcc
gcc -Wall -ansi main.c book.c -o BookSystem

# Run
./BookSystem
```

### Windows (Command Prompt)
```cmd
gcc -Wall -ansi main.c book.c -o BookSystem.exe
BookSystem.exe
```

---

## Features

| # | Feature         | Description                                      |
|---|-----------------|--------------------------------------------------|
| 1 | Add Book        | Add new book with ID, title, author, etc.        |
| 2 | Display Books   | View all books in a formatted table              |
| 3 | Search Book     | Find by ID (exact) or Title (keyword/partial)    |
| 4 | Update Book     | Edit any field; press Enter to keep old value    |
| 5 | Delete Book     | Remove a book after Y/N confirmation             |
| 6 | Issue Book      | Issue one copy (decrements quantity)             |
| 7 | Return Book     | Accept return of one copy (increments quantity)  |
| 8 | Statistics      | Totals, unique titles, inventory value           |
| 9 | Save Manually   | Explicit save (auto-save also happens always)    |
| 0 | Exit            | Saves and exits cleanly                          |

---

## Validation Rules

| Field       | Rule                                          |
|-------------|-----------------------------------------------|
| Book ID     | Must be a positive integer, must be unique    |
| Title       | Must not be empty                             |
| Author      | Must not be empty                             |
| Quantity    | Must be >= 0                                  |
| Price       | Must be >= 0.0                                |
| Issue       | Quantity must be > 0 before issuing           |

---

## Sample Test Session

```
Welcome to the Book Record Management System (BRMS)
No saved data found. Starting with an empty library.

  +======================================================+
  |         BOOK RECORD MANAGEMENT SYSTEM               |
  +======================================================+
  |   1.  Add Book                                       |
  ...
  Your choice: 1

  ADD NEW BOOK
  Enter Book ID        : 101
  Enter Title          : C Programming Language
  Enter Author         : Dennis Ritchie
  Enter Publisher      : Prentice Hall
  Enter Category       : Programming
  Enter Quantity       : 5
  Enter Price (Rs.)    : 450.00

  [SUCCESS] "C Programming Language" added. Library now has 1 record(s).

  Your choice: 8

  LIBRARY STATISTICS REPORT
  Total Book Records    : 1
  Unique Titles         : 1
  Total Copies in Stock : 5
  Total Inventory Value : Rs. 2250.00
  Most Expensive Book   : C Programming Language
  Most Expensive Price  : Rs. 450.00
```

---

## Black-Box Test Table

| Test | Function     | Input                      | Expected Output                          | Pass/Fail |
|------|-------------|----------------------------|------------------------------------------|-----------|
| T01  | addBook     | Valid ID=101, all fields   | "[SUCCESS] Book added..."                | -         |
| T02  | addBook     | Duplicate ID=101           | "[ERROR] Book ID 101 already used..."    | -         |
| T03  | addBook     | ID = -5                    | "[ERROR] Book ID must be positive"       | -         |
| T04  | addBook     | Empty title                | "[ERROR] Title cannot be empty"          | -         |
| T05  | addBook     | Quantity = -3              | "[ERROR] Quantity cannot be negative"    | -         |
| T06  | addBook     | Price = -100               | "[ERROR] Price cannot be negative"       | -         |
| T07  | displayBooks| Library has 3 books        | Table with 3 rows printed                | -         |
| T08  | displayBooks| Library is empty           | "No books in the system yet."            | -         |
| T09  | searchBook  | ID=101 (exists)            | Record displayed                         | -         |
| T10  | searchBook  | ID=999 (not found)         | "[NOT FOUND] No book with ID 999"        | -         |
| T11  | searchBook  | Keyword "prog"             | All books with "prog" in title shown     | -         |
| T12  | searchBook  | Keyword "xyz999"           | "No books match the keyword..."          | -         |
| T13  | updateBook  | ID=101, new title entered  | Title updated, confirmed                 | -         |
| T14  | updateBook  | ID=101, press Enter only   | No fields changed                        | -         |
| T15  | updateBook  | ID=999 (not found)         | "[NOT FOUND] No book with ID 999"        | -         |
| T16  | deleteBook  | ID=101, confirm Y          | "[SUCCESS] Book deleted..."              | -         |
| T17  | deleteBook  | ID=101, confirm N          | "Deletion cancelled. No changes made."   | -         |
| T18  | deleteBook  | ID=999 (not found)         | "[NOT FOUND] No book with ID 999"        | -         |
| T19  | issueBook   | ID=101, qty=5              | qty becomes 4, "[SUCCESS] Book issued"   | -         |
| T20  | issueBook   | ID=101, qty=0              | "[ERROR] OUT OF STOCK"                   | -         |
| T21  | returnBook  | ID=101, qty=4              | qty becomes 5, "[SUCCESS] Book returned" | -         |
| T22  | statistics  | 3 books, various prices    | Correct totals and inventory value       | -         |
| T23  | saveBooks   | After adding book          | books.dat exists and readable            | -         |
| T24  | loadBooks   | Restart program            | Previously added books still present     | -         |
| T25  | main        | Enter letter 'x' as choice | "[ERROR] Invalid input..."               | -         |

---

## Data Structures Used

| Structure   | Type               | Purpose                                  |
|-------------|--------------------|------------------------------------------|
| `Book`      | typedef struct     | Stores all fields for one book record    |
| `library[]` | Book array         | Holds all books in RAM (max 500)         |
| `bookCount` | int                | Tracks how many books are in array       |
| `books.dat` | Binary file        | Persistent storage across sessions       |

---

## Key C Concepts Demonstrated

| Concept          | Where Used                                        |
|------------------|---------------------------------------------------|
| Structures       | `typedef struct Book` in book.h                  |
| Arrays           | `Book library[MAX_BOOKS]` global array            |
| Pointers         | `Book *b` in printBookRow(), findBookByID()       |
| File Handling    | fopen/fclose/fread/fwrite in saveBooks/loadBooks  |
| Modular Code     | Separate main.c, book.c, book.h                  |
| String Functions | fgets, strcspn, strncpy, strstr, tolower          |
| Validation       | Input checks in every function                   |
| Extern keyword   | Global vars declared extern in book.h             |
| Static keyword   | displayMenu() static in main.c                   |
| Header Guards    | #ifndef BOOK_H in book.h                         |
