#include <stdbool.h>
#ifndef LOGIN_H
#define LOGIN_H
#include "user.h"


// Function prototypes
bool login(const char* username, const char* password);
void add_user(const char* username, char *password, int role);
void printAllUser();
void modify_user(User user);
void deposit(User user, float amount);
User get_balance(User user);
void update_password(User user);
void create_feedback(Feedback feedback);
Feedback* review_customer_feedback(int* feedback_count);
void update_feedback_review(int customerID);
void addLoan(Loan loan);
Loan* review_customer_loans(int* loan_count);
void update_loan(Loan updated_loan);
Loan* get_assigned_loans(int employee_id, int* loan_count);
Transaction* get_transactions_for_customer(int customer_id, int* transaction_count);
#endif // LOGIN_H
