#include <iostream>       
#include <vector>         
#include <string>         
#include <iomanip>        
using namespace std;

struct Date { int d, m, y; };

struct Customer {
    int id, loanCount = 0;
    string name, user, pass;
    double totalFine = 0, paidFine = 0;
};

struct Book {
    int id, year, loanDays = 7, custID = -1;  // Fixed 7 days loan period
    string title, author;
    bool onLoan = false;
    Date loanDate;
};

struct Payment {
    int id, custID, bookID, overdueDays;
    string custName, bookTitle;
    Date dueDate, returnDate;
    double amount, paidAmount;
    bool isPaid = false;
};

// ARRAY ADDITION 1: Array for fine rates
const double FINE_RATES[] = { 0, 5, 6, 7, 10 };  // Index 0: 0 days, 1: 1 day, 2: 2 days, 3: 3 days, 4: 4+ days
const int DAYS_IN_MONTH[] = { 31,28,31,30,31,30,31,31,30,31,30,31 };  // Array for days in each month
const string MONTH_NAMES[] = { "January", "February", "March", "April", "May", "June",
                             "July", "August", "September", "October", "November", "December" };

vector<Book> books;
vector<Customer> customers;
vector<Payment> payments;
int nextCustID = 1001;
int nextPayID = 10001;
string adminUser = "admin";
string adminPass = "123";

// Helper functions
Date getDate(string msg) {
    Date dt;
    cout << msg << " (DD MM YYYY): ";
    cin >> dt.d >> dt.m >> dt.y;
    return dt;
}

bool checkDate(Date dt) {
    if (dt.y < 2023 || dt.y > 2030) return false;
    if (dt.m < 1 || dt.m > 12) return false;

    // USING ARRAY: daysInMonth array
    int daysInMonth[] = { 31,28,31,30,31,30,31,31,30,31,30,31 };

    // Check for leap year if month is February
    if (dt.m == 2 && (dt.y % 4 == 0 && dt.y % 100 != 0 || dt.y % 400 == 0)) daysInMonth[1] = 29;

    return dt.d >= 1 && dt.d <= daysInMonth[dt.m - 1];
}

Date getDueDate(Date loan, int days) {
    Date due = loan;
    due.d += days;

    // USING ARRAY: DAYS_IN_MONTH array
    int daysIn[12];
    // Copy global array to local array
    for (int i = 0; i < 12; i++) {
        daysIn[i] = DAYS_IN_MONTH[i];
    }

    // Adjust for leap year if month is February
    if (due.m == 2 && (due.y % 4 == 0 && due.y % 100 != 0 || due.y % 400 == 0)) daysIn[1] = 29;

    // Handle day overflow to next month
    while (due.d > daysIn[due.m - 1]) {
        due.d -= daysIn[due.m - 1];
        due.m++;
        if (due.m > 12) {
            due.m = 1;
            due.y++;
        }
    }
    return due;
}

int getOverdueDays(Date loan, int days, Date ret) {
    Date due = getDueDate(loan, days);

    if (ret.y < due.y || (ret.y == due.y && ret.m < due.m) || (ret.y == due.y && ret.m == due.m && ret.d <= due.d))
        return 0;

    int overdue = 0;
    Date curr = due;

    // USING ARRAY: DAYS_IN_MONTH array
    int daysIn[12];
    for (int i = 0; i < 12; i++) {
        daysIn[i] = DAYS_IN_MONTH[i];
    }

    // Adjust for leap year
    if (curr.m == 2 && (curr.y % 4 == 0 && curr.y % 100 != 0 || curr.y % 400 == 0)) daysIn[1] = 29;

    // Count days until return date
    while (!(curr.y == ret.y && curr.m == ret.m && curr.d == ret.d)) {
        overdue++;
        curr.d++;

        // Handle month/year rollover
        if (curr.d > daysIn[curr.m - 1]) {
            curr.d = 1;
            curr.m++;
            if (curr.m > 12) {
                curr.m = 1;
                curr.y++;
                // Recheck leap year for new year
                if (curr.m == 2 && (curr.y % 4 == 0 && curr.y % 100 != 0 || curr.y % 400 == 0))
                    daysIn[1] = 29;
                else if (curr.m == 2)
                    daysIn[1] = 28;
            }
        }
    }
    return overdue;
}

double calcFine(int days) {
    // USING ARRAY: FINE_RATES array instead of if-else
    if (days <= 0) return FINE_RATES[0];    // 0 days: RM 0
    if (days == 1) return FINE_RATES[1];    // 1 day: RM 5
    if (days == 2) return FINE_RATES[2];    // 2 days: RM 6
    if (days == 3) return FINE_RATES[3];    // 3 days: RM 7
    return FINE_RATES[4] * days;           // 4+ days: RM 10 per day
}

// ARRAY ADDITION 2: Function to display fine rates using array
void displayFineRates() {
    cout << "\n=== FINE RATES ===\n";
    string descriptions[] = { "0 days (on time): RM ",
                           "1 day overdue: RM ",
                           "2 days overdue: RM ",
                           "3 days overdue: RM ",
                           "4+ days overdue: RM " };

    for (int i = 0; i < 5; i++) {
        if (i == 4) {
            cout << descriptions[i] << FINE_RATES[i] << " per day\n";
        }
        else {
            cout << descriptions[i] << FINE_RATES[i] << endl;
        }
    }
}

// ARRAY ADDITION 3: Function to display month name using array
string getMonthName(int month) {
    if (month >= 1 && month <= 12) {
        return MONTH_NAMES[month - 1];
    }
    return "Invalid Month";
}

// Book functions
void addBook() {
    Book b;
    b.loanDays = 7;  // Fixed 7-day loan period for all books
    cout << "\nADD NEW BOOK\nID: ";
    cin >> b.id;
    cin.ignore();
    cout << "Title: ";
    getline(cin, b.title);
    cout << "Author: ";
    getline(cin, b.author);
    cout << "Year: ";
    cin >> b.year;

    // All books have fixed 7-day loan period
    b.loanDays = 7;

    books.push_back(b);
    cout << "Book added with 7-day loan period!\n";
}

void viewBooks() {
    if (books.empty()) {
        cout << "No books.\n";
        return;
    }
    cout << "\nBOOK CATALOGUE\n";
    for (Book b : books) {
        // USING ARRAY: Get month name
        string monthName = getMonthName(b.loanDate.m);
        cout << "ID: " << b.id << " | " << b.title << " | " << b.author << " | " << b.year;
        cout << " | Loan Period: " << b.loanDays << " days";
        cout << " | " << (b.onLoan ? "Loaned" : "Available");
        if (b.onLoan) {
            cout << " (Since: " << b.loanDate.d << " " << monthName << " " << b.loanDate.y << ")";
        }
        cout << endl;
    }
}

void searchBook() {
    int ch; string s; int id;
    cout << "\nSEARCH BOOK\n1. By ID\n2. By Title\n3. By Author\nChoose: ";
    cin >> ch;
    cin.ignore();

    if (ch == 1) {
        cout << "Book ID: ";
        cin >> id;
        for (Book b : books) {
            if (b.id == id) {
                cout << "Found: " << b.title << " by " << b.author << " (" << b.year << ")\n";
                cout << "Loan Period: " << b.loanDays << " days\n";
                return;
            }
        }
        cout << "Not found.\n";
    }
    else if (ch == 2 || ch == 3) {
        cout << "Enter text: ";
        getline(cin, s);
        bool found = false;
        for (Book b : books) {
            if ((ch == 2 && b.title.find(s) != string::npos) ||
                (ch == 3 && b.author.find(s) != string::npos)) {
                cout << "ID: " << b.id << " | " << b.title << " | " << b.author;
                cout << " | Loan Period: " << b.loanDays << " days" << endl;
                found = true;
            }
        }
        if (!found) cout << "No books found.\n";
    }
}

void editBook() {
    int id;
    cout << "\nEDIT BOOK\nEnter Book ID: ";
    cin >> id;

    for (Book& b : books) {
        if (b.id == id) {
            if (b.onLoan) {
                cout << "Cannot edit a loaned book!\n";
                return;
            }

            cin.ignore();
            cout << "Current Title: " << b.title << endl;
            cout << "New Title (press Enter to skip): ";
            string newTitle;
            getline(cin, newTitle);
            if (!newTitle.empty()) b.title = newTitle;

            cout << "Current Author: " << b.author << endl;
            cout << "New Author (press Enter to skip): ";
            string newAuthor;
            getline(cin, newAuthor);
            if (!newAuthor.empty()) b.author = newAuthor;

            cout << "Current Year: " << b.year << endl;
            cout << "New Year (enter 0 to skip): ";
            int newYear;
            cin >> newYear;
            if (newYear != 0) b.year = newYear;

            cout << "Book updated successfully!\n";
            return;
        }
    }
    cout << "Book not found.\n";
}

void deleteBook() {
    int id;
    cout << "\nDELETE BOOK\nEnter Book ID: ";
    cin >> id;

    for (int i = 0; i < books.size(); i++) {
        if (books[i].id == id) {
            if (books[i].onLoan) {
                cout << "Cannot delete a loaned book!\n";
                return;
            }

            cout << "Are you sure you want to delete: " << books[i].title << "? (y/n): ";
            char confirm;
            cin >> confirm;

            if (confirm == 'y' || confirm == 'Y') {
                books.erase(books.begin() + i);
                cout << "Book deleted successfully!\n";
            }
            else {
                cout << "Deletion cancelled.\n";
            }
            return;
        }
    }
    cout << "Book not found.\n";
}

// Customer functions
void registerCust() {
    Customer c;
    c.id = nextCustID++;
    cin.ignore();
    cout << "\nREGISTRATION\nName: ";
    getline(cin, c.name);
    cout << "Username: ";
    getline(cin, c.user);

    for (Customer cust : customers) {
        if (cust.user == c.user) {
            cout << "Username exists!\n";
            return;
        }
    }

    cout << "Password: ";
    getline(cin, c.pass);
    customers.push_back(c);
    cout << "Registered! Your ID: " << c.id << endl;
}

void viewCust() {
    if (customers.empty()) {
        cout << "No customers.\n";
        return;
    }
    cout << "\nALL CUSTOMERS\n";
    for (Customer c : customers) {
        cout << "ID: " << c.id << " | " << c.name << " | Loans: " << c.loanCount;
        cout << " | Fine: RM " << fixed << setprecision(2) << (c.totalFine - c.paidFine) << endl;
    }
}

void payRemainingBalance(int custID) {
    cout << "\n===== PAY REMAINING BALANCE =====\n";

    bool hasUnpaid = false;
    vector<Payment*> unpaidPayments;

    for (Payment& p : payments) {
        if (p.custID == custID && !p.isPaid && p.paidAmount < p.amount) {
            hasUnpaid = true;
            unpaidPayments.push_back(&p);
        }
    }

    if (!hasUnpaid) {
        cout << "You have no pending payments with remaining balance.\n";
        return;
    }

    cout << "Your pending payments with remaining balance:\n";
    for (int i = 0; i < unpaidPayments.size(); i++) {
        Payment* p = unpaidPayments[i];
        double remaining = p->amount - p->paidAmount;
        cout << i + 1 << ". Payment ID: " << p->id << " | Book: " << p->bookTitle;
        cout << " | Total Due: RM " << fixed << setprecision(2) << p->amount;
        cout << " | Paid: RM " << fixed << setprecision(2) << p->paidAmount;
        cout << " | Remaining: RM " << fixed << setprecision(2) << remaining << endl;
    }

    int choice;
    cout << "\nSelect payment to pay (0 to cancel): ";
    cin >> choice;

    if (choice < 1 || choice > unpaidPayments.size()) {
        cout << "Cancelled.\n";
        return;
    }

    Payment* selectedPayment = unpaidPayments[choice - 1];
    double remaining = selectedPayment->amount - selectedPayment->paidAmount;

    cout << "\nPayment for: " << selectedPayment->bookTitle << endl;
    cout << "Remaining amount: RM " << fixed << setprecision(2) << remaining << endl;

    double payment;
    cout << "Enter payment amount: RM ";
    cin >> payment;

    if (payment <= 0) {
        cout << "Invalid amount!\n";
        return;
    }

    selectedPayment->paidAmount += payment;

    if (selectedPayment->paidAmount >= selectedPayment->amount) {
        selectedPayment->isPaid = true;
        double overpayment = selectedPayment->paidAmount - selectedPayment->amount;
        if (overpayment > 0) {
            cout << "Payment completed with balance: RM " << fixed << setprecision(2) << overpayment << endl;
        }
        else {
            cout << "Payment completed! All dues cleared.\n";
        }
    }
    else {
        remaining = selectedPayment->amount - selectedPayment->paidAmount;
        cout << "Partial payment accepted. Remaining balance: RM " << fixed << setprecision(2) << remaining << endl;
    }

    for (Customer& c : customers) {
        if (c.id == custID) {
            c.paidFine += payment;
            break;
        }
    }

    cout << "Payment recorded successfully!\n";
}

// Loan functions
void loanBook(int custID) {
    Customer* cust = nullptr;
    for (Customer& c : customers) if (c.id == custID) { cust = &c; break; }
    if (!cust) { cout << "Customer not found!\n"; return; }
    if (cust->loanCount >= 5) { cout << "Max 5 books!\n"; return; }

    cout << "\n=== LOAN BOOK ===\n";
    viewBooks();

    int bookID;
    cout << "\nEnter Book ID to loan: ";
    cin >> bookID;

    for (Book& b : books) {
        if (b.id == bookID) {
            if (b.onLoan) {
                cout << "Book is already on loan!\n";
                return;
            }

            // Get loan date with validation
            Date loanDate;
            do {
                loanDate = getDate("Enter loan date");
                if (!checkDate(loanDate)) {
                    cout << "Invalid date! Please enter a valid date (2023-2030).\n";
                }
            } while (!checkDate(loanDate));

            b.onLoan = true;
            b.custID = custID;
            b.loanDate = loanDate;
            // Book already has fixed 7-day loan period (set in struct)

            cust->loanCount++;

            Date dueDate = getDueDate(loanDate, b.loanDays);
            cout << "\nBook loaned successfully!\n";
            cout << "Book: " << b.title << endl;
            cout << "Loan Date: " << loanDate.d << " " << getMonthName(loanDate.m) << " " << loanDate.y << endl;
            cout << "Due Date: " << dueDate.d << " " << getMonthName(dueDate.m) << " " << dueDate.y << endl;
            cout << "Loan Period: " << b.loanDays << " days" << endl;
            return;
        }
    }

    cout << "Book not found!\n";
}

// Return book function
void returnBook(int custID) {
    cout << "\n=== RETURN BOOK ===\n";

    // Find customer's loaned books
    vector<Book*> customerBooks;
    for (Book& b : books) {
        if (b.onLoan && b.custID == custID) {
            customerBooks.push_back(&b);
        }
    }

    if (customerBooks.empty()) {
        cout << "You don't have any books on loan.\n";
        return;
    }

    cout << "Your loaned books:\n";
    for (Book* b : customerBooks) {
        Date dueDate = getDueDate(b->loanDate, b->loanDays);
        cout << "ID: " << b->id << " | " << b->title
            << " | Due: " << dueDate.d << " " << getMonthName(dueDate.m) << " " << dueDate.y << endl;
    }

    int bookID;
    cout << "\nEnter Book ID to return: ";
    cin >> bookID;

    for (Book& b : books) {
        if (b.id == bookID && b.onLoan && b.custID == custID) {
            // Get return date
            Date returnDate;
            do {
                returnDate = getDate("Enter return date");
                if (!checkDate(returnDate)) {
                    cout << "Invalid date! Please enter a valid date (2023-2030).\n";
                }
            } while (!checkDate(returnDate));

            // Calculate overdue and fine
            int overdueDays = getOverdueDays(b.loanDate, b.loanDays, returnDate);
            double fine = calcFine(overdueDays);

            // Create payment record
            Payment p;
            p.id = nextPayID++;
            p.custID = custID;
            p.bookID = b.id;
            p.dueDate = getDueDate(b.loanDate, b.loanDays);
            p.returnDate = returnDate;
            p.overdueDays = overdueDays;
            p.amount = fine;
            p.paidAmount = 0;
            p.isPaid = (fine == 0);
            p.bookTitle = b.title;

            // Find customer name
            for (Customer& c : customers) {
                if (c.id == custID) {
                    p.custName = c.name;
                    c.totalFine += fine;
                    c.loanCount--;
                    break;
                }
            }

            payments.push_back(p);

            // Reset book status
            b.onLoan = false;
            b.custID = -1;

            cout << "\nBook returned successfully!\n";
            cout << "Book: " << b.title << endl;
            cout << "Return Date: " << returnDate.d << " " << getMonthName(returnDate.m) << " " << returnDate.y << endl;
            cout << "Original Loan Period: " << b.loanDays << " days" << endl;

            if (overdueDays > 0) {
                cout << "Overdue: " << overdueDays << " days\n";
                cout << "Fine: RM " << fixed << setprecision(2) << fine << endl;
                cout << "Fine Rate Used: ";
                if (overdueDays == 1) cout << "RM " << FINE_RATES[1] << " (1 day overdue)";
                else if (overdueDays == 2) cout << "RM " << FINE_RATES[2] << " (2 days overdue)";
                else if (overdueDays == 3) cout << "RM " << FINE_RATES[3] << " (3 days overdue)";
                else cout << "RM " << FINE_RATES[4] << " per day (" << overdueDays << " days overdue)";
                cout << endl;
            }
            else {
                cout << "Returned on time. No fine incurred.\n";
            }

            return;
        }
    }

    cout << "Book not found or not loaned by you.\n";
}

// View fines function (removed from customer menu but kept for admin access)
void viewFines(int custID) {
    cout << "\n=== YOUR FINES ===\n";

    // Find customer
    for (Customer& c : customers) {
        if (c.id == custID) {
            double balance = c.totalFine - c.paidFine;
            cout << "Customer: " << c.name << endl;
            cout << "Total Fines Incurred: RM " << fixed << setprecision(2) << c.totalFine << endl;
            cout << "Total Paid: RM " << fixed << setprecision(2) << c.paidFine << endl;
            cout << "Current Balance: RM " << fixed << setprecision(2) << balance << endl;

            // Show payment details
            bool hasPayments = false;
            for (Payment& p : payments) {
                if (p.custID == custID) {
                    hasPayments = true;
                    cout << "\nPayment ID: " << p.id << " | Book: " << p.bookTitle;
                    cout << " | Amount: RM " << fixed << setprecision(2) << p.amount;
                    cout << " | Paid: RM " << fixed << setprecision(2) << p.paidAmount;
                    cout << " | Status: " << (p.isPaid ? "PAID" : "UNPAID") << endl;
                }
            }

            if (!hasPayments) {
                cout << "\nNo payment records found.\n";
            }
            return;
        }
    }

    cout << "Customer not found!\n";
}

// View payment history
void viewPaymentHistory(int custID) {
    cout << "\n=== PAYMENT HISTORY ===\n";

    bool found = false;
    for (Payment& p : payments) {
        if (p.custID == custID) {
            found = true;
            cout << "Payment ID: " << p.id << endl;
            cout << "Date: " << p.returnDate.d << " " << getMonthName(p.returnDate.m) << " " << p.returnDate.y << endl;
            cout << "Book: " << p.bookTitle << endl;
            cout << "Amount: RM " << fixed << setprecision(2) << p.amount << endl;
            cout << "Paid: RM " << fixed << setprecision(2) << p.paidAmount << endl;
            cout << "Status: " << (p.isPaid ? "PAID" : "UNPAID") << endl;
            cout << "-------------------\n";
        }
    }

    if (!found) {
        cout << "No payment records found.\n";
    }
}

// Login functions
bool adminLogin() {
    string user, pass;
    cin.ignore();
    cout << "\n=== ADMIN LOGIN ===\n";
    cout << "Username: ";
    getline(cin, user);
    cout << "Password: ";
    getline(cin, pass);

    if (user == adminUser && pass == adminPass) {
        cout << "Login successful!\n";
        return true;
    }

    cout << "Invalid credentials!\n";
    return false;
}

bool customerLogin(int& custID) {
    string user, pass;
    cin.ignore();
    cout << "\n=== CUSTOMER LOGIN ===\n";
    cout << "Username: ";
    getline(cin, user);
    cout << "Password: ";
    getline(cin, pass);

    for (Customer& c : customers) {
        if (c.user == user && c.pass == pass) {
            custID = c.id;
            cout << "Login successful! Welcome, " << c.name << "!\n";
            return true;
        }
    }

    cout << "Invalid credentials!\n";
    return false;
}

// Menu functions
void adminMenu() {
    int choice;
    do {
        cout << "\n=== ADMIN MENU ===\n";
        cout << "1. Add New Book\n";
        cout << "2. View All Books\n";
        cout << "3. Search Book\n";
        cout << "4. Edit Book\n";
        cout << "5. Delete Book\n";
        cout << "6. View All Customers\n";
        cout << "7. View Fine Rates\n";
        cout << "8. Logout\n";
        cout << "Enter choice: ";
        cin >> choice;

        switch (choice) {
        case 1: addBook(); break;
        case 2: viewBooks(); break;
        case 3: searchBook(); break;
        case 4: editBook(); break;
        case 5: deleteBook(); break;
        case 6: viewCust(); break;
        case 7: displayFineRates(); break;
        case 8: cout << "Logging out...\n"; break;
        default: cout << "Invalid choice!\n";
        }
    } while (choice != 8);
}

void customerMenu(int custID) {
    int choice;
    do {
        cout << "\n=== CUSTOMER MENU ===\n";
        cout << "1. View All Books\n";
        cout << "2. Search Book\n";
        cout << "3. Loan Book\n";
        cout << "4. Return Book\n";
        cout << "5. Pay Balance\n";
        cout << "6. View Payment History\n";
        cout << "7. Logout\n";
        cout << "Enter choice: ";
        cin >> choice;

        switch (choice) {
        case 1: viewBooks(); break;
        case 2: searchBook(); break;
        case 3: loanBook(custID); break;
        case 4: returnBook(custID); break;
        case 5: payRemainingBalance(custID); break;
        case 6: viewPaymentHistory(custID); break;
        case 7: cout << "Logging out...\n"; break;
        default: cout << "Invalid choice!\n";
        }
    } while (choice != 7);
}

// Main function
int main() {
    cout << "=========================================\n";
    cout << "   LIBRARY MANAGEMENT SYSTEM\n";
    cout << "=========================================\n";

    int mainChoice;
    do {
        cout << "\n=== MAIN MENU ===\n";
        cout << "1. Admin Login\n";
        cout << "2. Customer Login\n";
        cout << "3. Customer Registration\n";
        cout << "4. View Fine Rates\n";
        cout << "5. Exit\n";
        cout << "Enter choice: ";
        cin >> mainChoice;

        switch (mainChoice) {
        case 1:
            if (adminLogin()) adminMenu();
            break;
        case 2: {
            int custID;
            if (customerLogin(custID)) customerMenu(custID);
            break;
        }
        case 3:
            registerCust();
            break;
        case 4:
            displayFineRates();
            break;
        case 5:
            cout << "\nThank you for using the Library System!\n";
            break;
        default:
            cout << "Invalid choice!\n";
        }
    } while (mainChoice != 5);

    return 0;
}