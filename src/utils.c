#include <fcntl.h>
#include <unistd.h>
#include <sys/types.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "login.h"
#include "user.h"
#include "globals.h"

void print_user(User user) {
    printf("\n");
    printf("User ID: %d\n", user.id);
    printf("Username: %s\n", user.username);
    printf("Password: %s\n", user.password);
    printf("Role: %d\n", user.role);
    printf("Account Balance: %.2f\n", user.account_balance);
    printf("\n");
}


FILE *openDataFile(char *flag) {
    FILE *file = fopen("database/data.dat", flag);

    if (file == NULL) {
        printf("Error! Can't open file.\n");
        exit(1);
    }
    return file;
}



void printAllUser() {

    FILE *file = fopen("database/data.dat", "rb");

    if (file == NULL) {
        printf("Error! Can't open file.\n");
        exit(1);
    }

    if (fseek(file, 0, SEEK_SET) != 0) {
        perror("Seek to start failed");
        fclose(file);
        return;
    }


    User user;
    while (fread(&user, sizeof(User), 1, file)) {
        // printf("Testing ALL User \n");
        printf("%d %s %s %d %f\n", user.id, user.username, user.password, user.role, user.account_balance);
    }

    fclose(file);

}



// Check Login Details
bool check_credentials(const char* username, const char* password) {

    // if (strcmp(username, "admin") == 0 && strcmp(password, "123") == 0) {
    //     strcpy(current_user.username, username);
    //     strcpy(current_user.password, password);
    //     strcpy(current_user.status, "active");
    //     current_user.id = 1;
    //     current_user.role = 4;
    //     current_user.account_balance = 0.0;
    //      printf("Logged In as %s \n", current_user.username);
    //     return true;
    // }

    FILE *file = fopen("database/data.dat", "rb");

    if (file == NULL) {
        printf("Error! Can't open file.\n");
        exit(1);
    }

    User user;
    while (fread(&user, sizeof(User), 1, file)) {
        if (strcmp(username, user.username) == 0 && strcmp(password, user.password) == 0 && user.account_balance != -1) {
            current_user = user;
            printf("Logged In as %s \n", current_user.username);
            fclose(file);
            return true;
        }
    }
    fclose(file);
    return false;
}


bool login(const char* username, const char* password) {

    if (check_credentials(username, password)) {
        printf("Login successful!\n");
        return true;
    } else {
        current_user.id = -1;
        printf("Invalid username: %s or password: %s .\n", username, password);
        return false;
    }
}


// Logout
// void logout() {
//     printf("Logging out Current User %s...\n", current_user.username);
//     printf("\n");
// }




void add_user(const char* username, char* password, int role) {
    printf("%s \n", username);
    FILE *file = openDataFile("rb+");

    fseek(file, -sizeof(User), SEEK_END);
    User user;
    fread(&user, sizeof(User), 1, file);
    int next_id = user.id + 1;

    User new_user;
    strcpy(new_user.username, username);
    strcpy(new_user.password, password);
    new_user.role = role;
    new_user.id = next_id;
    new_user.account_balance = 0.0;

    fseek(file, 0, SEEK_END);

    fwrite(&new_user, sizeof(User), 1, file);

    // fseek(file, 0, SEEK_SET);
    // while (fread(&user, sizeof(User), 1, file)) {
    //     printf("%d %s %s %d %f \n", user.id, user.username, user.password, user.role, user.account_balance);
    // }
    fclose(file);

    printf("User added successfully!\n");
}



void modify_user(User user) {
    FILE *file = openDataFile("rb+");


    fseek(file, (user.id - 1) * sizeof(User), SEEK_SET);

    // fread(&user, sizeof(User), 1, file);


    if (feof(file)) {
        printf("User not Found \n");
        return;
    }


    if (fwrite(&user, sizeof(User), 1, file) != 1) {
        printf("Error writing user to file.\n");
        fclose(file);
        return;
    }

    fflush(file); // Make sure changes are written to disk

    fclose(file);
}



void deposit(User user, float amount) {

    FILE *file = openDataFile("rb+");
    fseek(file, (user.id - 1) * sizeof(User), SEEK_SET);
    fread(&user, sizeof(User), 1, file);
    fseek(file, -sizeof(User), SEEK_CUR);
    if (amount < 0) {
        if (user.account_balance < (-1.00 * amount)) {
            printf("Error doing transaction due to insuffienct funds.\n");
            fclose(file);
            return;
        }
    }
    float new_amount = amount + user.account_balance;


    user.account_balance = new_amount;
    if (fwrite(&user, sizeof(User), 1, file) != 1) {
        printf("Error writing user to file.\n");
        fclose(file);
        return;
    }

    fflush(file); // Make sure changes are written to disk

    fclose(file);

}

User get_balance(User user) {
    FILE *file = openDataFile("rb");
    fseek(file, (user.id - 1) * sizeof(User), SEEK_SET);
    fread(&user, sizeof(User), 1, file);
    fclose(file);
    return user;
}


void update_password(User user) {
    FILE *file = openDataFile("rb+");
    fseek(file, (user.id - 1) * sizeof(User), SEEK_SET);
    if (fwrite(&user, sizeof(User), 1, file) != 1) {
        printf("Error writing user to file.\n");
        fclose(file);
        return;
    }

    fflush(file); // Make sure changes are written to disk

    fclose(file);
}


void create_feedback(Feedback feedback) {
    // printf("%s \n", feedback.customerFeedback);
    // printf("%s \n", feedback.customerName);
    // printf("%d %d \n", feedback.review, feedback.customerID);

    FILE *file = fopen("database/feedback.dat", "rb+");
    fseek(file, 0, SEEK_END);

    Feedback fb = feedback;
    if (fwrite(&fb, sizeof(fb), 1, file) != 1) {
        printf("Error writing feedback to file.\n");
        fclose(file);
        return;
    }

    fflush(file); // Make sure changes are written to disk

    fclose(file);

}


// Feedback* review_customer_feedback(int* feedback_count) {


//     FILE *file = fopen("database/feedback.dat", "rb");
//     if (file == NULL) {
//         printf("Error opening feedback file.\n");
//         *feedback_count = 0;
//         return NULL;
//     }

//     // Get the number of feedback entries
//     fseek(file, 0, SEEK_END);
//     long file_size = ftell(file);
//     *feedback_count = file_size / sizeof(Feedback);
//     rewind(file);

//     if (*feedback_count == 0) {
//         printf("No feedback available.\n");
//         fclose(file);
//         return NULL;
//     }

//     // Allocate memory for the feedback array
//     Feedback* feedback_array = (Feedback*)malloc((*feedback_count) * sizeof(Feedback));
//     if (feedback_array == NULL) {
//         printf("Memory allocation failed.\n");
//         fclose(file);
//         return NULL;
//     }


//     // Read the feedback entries into the array
//     if (fread(feedback_array, sizeof(Feedback), (*feedback_count), file) != (*feedback_count)) {
//         printf("Error reading feedback from file.\n");
//         free(feedback_array);
//         fclose(file);
//         return NULL;
//     }

//     // feedback_array[*feedback_count].customerID = -1;

//     fclose(file);
//     return feedback_array;
// }


Feedback* review_customer_feedback(int* feedback_count) {
    FILE *file = fopen("database/feedback.dat", "rb");
    if (file == NULL) {
        printf("Error opening feedback file.\n");
        *feedback_count = 0;
        return NULL;
    }

    // Get the number of feedback entries
    fseek(file, 0, SEEK_END);
    long file_size = ftell(file);
    *feedback_count = file_size / sizeof(Feedback);
    rewind(file);

    if (*feedback_count == 0) {
        printf("No feedback available.\n");
        fclose(file);
        return NULL;
    }

    // Allocate memory for the feedback array
    Feedback* feedback_array = (Feedback*)malloc((*feedback_count) * sizeof(Feedback));
    if (feedback_array == NULL) {
        printf("Memory allocation failed.\n");
        fclose(file);
        return NULL;
    }

    // Read the feedback entries into the array
    if (fread(feedback_array, sizeof(Feedback), (*feedback_count), file) != (*feedback_count)) {
        printf("Error reading feedback from file.\n");
        // free(feedback_array);
        fclose(file);
        return NULL;
    }

    // Filter the feedback entries based on review == 0
    int new_feedback_count = 0;
    for (int i = 0; i < *feedback_count; i++) {
        if (feedback_array[i].review == 0) {
            new_feedback_count++;
        }
    }

    // Allocate memory for the filtered feedback array
    Feedback* filtered_feedback_array = (Feedback*)malloc(new_feedback_count * sizeof(Feedback));
    if (filtered_feedback_array == NULL) {
        printf("Memory allocation failed for filtered feedback array.\n");
        // free(feedback_array);
        fclose(file);
        return NULL;
    }

    // Copy the filtered entries to the new array
    int j = 0;
    for (int i = 0; i < *feedback_count; i++) {
        if (feedback_array[i].review == 0) {
            filtered_feedback_array[j++] = feedback_array[i];
        }
    }

    free(feedback_array); // Free the original array
    fclose(file);

    *feedback_count = new_feedback_count; // Update the count to the filtered count
    return filtered_feedback_array; // Return the filtered array
}


void update_feedback_review(int customerID) {
    FILE *file = fopen("database/feedback.dat", "rb+");
    if (file == NULL) {
        printf("Error opening feedback file.\n");
        return;
    }

    Feedback feedback;
    // int found = 0;


    while (fread(&feedback, sizeof(Feedback), 1, file)) {
        if (feedback.customerID == customerID) {
            feedback.review = 1;

            fseek(file, -sizeof(Feedback), SEEK_CUR);
            if (fwrite(&feedback, sizeof(Feedback), 1, file) != 1) {
                printf("Error writing feedback to file.\n");
                fclose(file);
                return;
            }
            // found = 1;
            // printf("Updated feedback for customer ID %d.\n", customerID);
            // break;
        }
    }

    // if (!found) {
    //     printf("No feedback found for customer ID %d.\n", customerID);
    // }

    fclose(file);
}


void addLoan(Loan loan) {
    // printf("Coming here \n");
    FILE *file = fopen("database/loans.dat", "ab");
    if (file == NULL) {
        printf("Error opening loans file.\n");
        return;
    }

    // Write the loan entry to the file
    if (fwrite(&loan, sizeof(Loan), 1, file) != 1) {
        printf("Error writing loan to file.\n");
        fclose(file);
        return;
    }

    fclose(file);
    printf("Loan added successfully!\n");
}


Loan* review_customer_loans(int* loan_count) {
    FILE *file = fopen("database/loans.dat", "rb");
    if (file == NULL) {
        printf("Error opening loans file.\n");
        *loan_count = 0;
        return NULL;
    }


    fseek(file, 0, SEEK_END);
    long file_size = ftell(file);
    *loan_count = file_size / sizeof(Loan);
    rewind(file);

    if (*loan_count == 0) {
        printf("No loans available.\n");
        fclose(file);
        return NULL;
    }


    Loan* loan_array = (Loan*)malloc((*loan_count) * sizeof(Loan));
    if (loan_array == NULL) {
        printf("Memory allocation failed.\n");
        fclose(file);
        return NULL;
    }

    if (fread(loan_array, sizeof(Loan), (*loan_count), file) != (*loan_count)) {
        printf("Error reading loans from file.\n");
        free(loan_array);
        fclose(file);
        return NULL;
    }

    int new_loan_count = 0;
    for (int i = 0; i < *loan_count; i++) {
        if (strcmp(loan_array[i].status, "Processing") == 0) {
            new_loan_count++;
        }
    }

    Loan* filtered_loan_array = (Loan*)malloc(new_loan_count * sizeof(Loan));
    if (filtered_loan_array == NULL) {
        printf("Memory allocation failed for filtered loan array.\n");
        free(loan_array);
        fclose(file);
        return NULL;
    }

    int j = 0;
    for (int i = 0; i < *loan_count; i++) {
        if (strcmp(loan_array[i].status, "Processing") == 0) {
            filtered_loan_array[j++] = loan_array[i];
        }
    }

    free(loan_array);
    fclose(file);

    *loan_count = new_loan_count;
    return filtered_loan_array;
}


void update_loan(Loan updated_loan) {
    FILE *file = fopen("database/loans.dat", "rb+");
    if (file == NULL) {
        printf("Error opening loan file.\n");
        return;
    }

    Loan loan;
    int found = 0;


    while (fread(&loan, sizeof(Loan), 1, file)) {

        if (loan.user_id == updated_loan.user_id &&
                strcmp(loan.username, updated_loan.username) == 0 &&
                loan.amount == updated_loan.amount &&
                strcmp(loan.status, updated_loan.status) == 0 &&
                strcmp(loan.type, updated_loan.type) == 0) {


            loan.employee_id = updated_loan.employee_id;


            fseek(file, -sizeof(Loan), SEEK_CUR);
            if (fwrite(&loan, sizeof(Loan), 1, file) != 1) {
                printf("Error writing updated loan to file.\n");
                fclose(file);
                return;
            }

            found = 1;
            printf("Updated loan for user ID %d.\n", updated_loan.user_id);
            break;
        }
    }

    if (!found) {
        printf("No loan found for user ID %d.\n", updated_loan.user_id);
    }

    fclose(file);
}
