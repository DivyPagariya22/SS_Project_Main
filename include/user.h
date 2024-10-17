#ifndef USER_H
#define USER_H


typedef struct {
    int customerID;
    char customerName[50];
    char customerFeedback[2000];
    int review;
} Feedback;

typedef struct {
    int user_id;
    char username[50];
    int employee_id;
    float amount;
    char status[20];
    char type[20];
} Loan;

// User struct to hold user information
typedef struct {
    int id;                  // Unique identifier for the user
    char username[50];      // Username of the user
    char password[50];      // Password of the user
    int role;               // Role of the user (1: customer, 2: employee, 3: manager, 4: admin)
    float account_balance;   // Account balance for the user
} User;

typedef struct {
    User user;
    char action[50];
    float amount;
    int transfer_id;
    Feedback feedback;
    Loan loan;
} Request;

typedef struct {
    int from;
    int to;
    float amount;
    char from_username[50];
    char to_username[50];
} Transaction;







// Transaction struct (placeholder for future use)
// typedef struct {
//     int transaction_id;     // Unique identifier for the transaction
//     int user_id;            // ID of the user associated with the transaction
//     float amount;           // Amount of the transaction
//     char type[20];          // Type of transaction (e.g., deposit, withdrawal)
//     char date[20];          // Date of the transaction
// } Transaction;

#endif // USER_H
