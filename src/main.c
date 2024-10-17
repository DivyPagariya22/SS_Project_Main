#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <stdio.h>
#include <arpa/inet.h>
#include <pthread.h>

#include <sys/socket.h>
#include <netinet/in.h>
#include "banking.h"
#include "login.h"
#include "admin.h"
#include "user.h"
#include "globals.h"

User current_user;

// User fetchAllUsers() {
//     FILE *file = fopen("database/data.dat", "rb");

//     if (file == NULL) {
//         printf("Error! Can't open file.\n");
//         exit(1);
//     }

//     User *user;
//     User temp;
//     int i = 0;
//     while (fread(&temp, sizeof(User), 1, file)) {
//         user[i++] = temp;
//     }

//     user[i].id = -1;

//     return user;
// }

void addTransaction(Transaction trans) {
    FILE *file = fopen("database/transaction.dat", "rb+");
    if (file == NULL) {
        printf("Error! Can't open file.\n");
        exit(1);
    }

    fseek(file, 0, SEEK_END);
    if (fwrite(&trans, sizeof(trans), 1, file) != 1) {
        printf("Error writing transaction to file.\n");
        fclose(file);
        return;
    }

    fflush(file);

    fclose(file);
}


User getUser(int id) {
    FILE *file = fopen("database/data.dat", "rb");
    User user;
    fseek(file, (id - 1) * sizeof(User), SEEK_SET);
    fread(&user, sizeof(User), 1, file);
    fclose(file);
    return user;
}

void admin_functions(Request req, int client_sock) {
    if (strcmp(req.action, "ADD_EMPLOYEE") == 0) {
        // Handle adding a user
        add_user(req.user.username, req.user.password, req.user.role);
        printf("User %s added.\n", req.user.username);
        return;
    }

    if (strcmp(req.action, "MODIFY_EMPLOYEE") == 0) {
        modify_user(req.user);
        printf("User %s modified.\n", req.user.username);
    }

    if (strcmp(req.action, "GET_USER") == 0) {
        User user = getUser(req.user.id);
        send(client_sock, &user, sizeof(User), 0);
    }

    // if(strcmp(req.action, "GET_ALL_USER") == 0) {
    //     User *user;
    //     user = fetchAllUsers();
    //     send(client_sock, &user, sizeof(user), 0);
    // }


}

void customer_functions(Request req, int client_sock) {
    if (strcmp(req.action, "GET_BALANCE") == 0) {
        User user = get_balance(req.user);
        send(client_sock, &user, sizeof(user), 0);
    }

    if (strcmp(req.action, "DEPOSIT") == 0) {
        deposit(req.user, req.amount);
        User user = req.user;
        // Add Transaction in database
        Transaction trans;
        trans.to = user.id;
        trans.from = user.id;
        trans.amount = req.amount;
        strcpy(trans.from_username, user.username);
        strcpy(trans.to_username, user.username);

        addTransaction(trans);
    }

    if (strcmp(req.action, "TRANSFER_FUNDS") == 0) {
        User destination = getUser(req.transfer_id);
        User source = getUser(req.user.id);
        char error[50];
        if (source.account_balance < req.amount) {
            printf("Error doing transaction due to insuffienct funds.\n");
            strcpy(error, "insuffienct funds");
            send(client_sock, error, sizeof(error), 0);
            return;
        }
        deposit(destination, req.amount);
        deposit(req.user, -1.00 * req.amount);

        // Add Transaction in database
        Transaction trans;
        trans.to = destination.id;
        trans.from = source.id;
        trans.amount = -1 * req.amount;
        strcpy(trans.from_username, source.username);
        strcpy(trans.to_username, destination.username);

        addTransaction(trans);

        trans.to = source.id;
        trans.from = destination.id;
        trans.amount = req.amount;
        strcpy(trans.from_username, destination.username);
        strcpy(trans.to_username, source.username);

        addTransaction(trans);

        strcpy(error, "no error");
        send(client_sock, error, sizeof(error), 0);
    }

    if (strcmp(req.action, "CHANGE_PASSWORD") == 0) {
        char new_password[50];
        strcpy(new_password, req.user.password);
        User user = getUser(req.user.id);
        strcpy(user.password, new_password);
        update_password(user);
    }

    if (strcmp(req.action, "ADD_FEEDBACK") == 0) {
        create_feedback(req.feedback);
    }

    if (strcmp(req.action, "APPLY_LOAN") == 0) {
        addLoan(req.loan);
    }
}

void employee_functions(Request req, int client_sock) {
    if (strcmp(req.action, "ADD_CUSTOMER") == 0) {
        // Handle adding a user
        add_user(req.user.username, req.user.password, req.user.role);
        printf("User %s added.\n", req.user.username);
        return;
    }

    if (strcmp(req.action, "GET_USER") == 0) {
        User user = getUser(req.user.id);
        send(client_sock, &user, sizeof(user), 0);
    }

    if (strcmp(req.action, "MODIFY_CUSTOMER") == 0) {
        modify_user(req.user);
        printf("Customer Modified \n");
    }


    if (strcmp(req.action, "CHANGE_PASSWORD") == 0) {
        char new_password[50];
        strcpy(new_password, req.user.password);
        User user = getUser(req.user.id);
        strcpy(user.password, new_password);
        update_password(user);
    }
}

void manager_function(Request req, int client_sock) {
    if (strcmp(req.action, "DEACTIVATE_ACC") == 0) {
        User user = getUser(req.user.id);
        user.account_balance = -1.0;
        // printf("Account Balance : %f \n", user.account_balance);
        modify_user(user);
    }

    if (strcmp(req.action, "ACTIVATE_ACC") == 0) {
        User user = getUser(req.user.id);
        if (user.account_balance == -1) user.account_balance = 0.0;
        // printf("Account Balance : %f \n", user.account_balance);
        modify_user(user);
    }

    if (strcmp(req.action, "CHANGE_PASSWORD") == 0) {
        char new_password[50];
        strcpy(new_password, req.user.password);
        User user = getUser(req.user.id);
        strcpy(user.password, new_password);
        update_password(user);
    }

    if (strcmp(req.action, "GET_ALL_FEEDBACK") == 0) {
        int feedback_count = 0;
        Feedback* feedback_array = review_customer_feedback(&feedback_count);

        int total_entries = feedback_count; // Include the terminator
        ssize_t bytes_sent = send(client_sock, &total_entries, sizeof(int), 0);

        if (bytes_sent < 0) {
            perror("Failed to send feedback count");
            return;
        }


        ssize_t total_size = total_entries * sizeof(Feedback);
        bytes_sent = send(client_sock, feedback_array, total_size, 0);
        if (bytes_sent < 0) {
            perror("Failed to send feedback array");
        } else {
            printf("Sent %d feedback entries including terminator to the client.\n", feedback_count);
        }
    }

    if(strcmp(req.action, "CLEAR_FEEDBACK") == 0) {
        int id = req.user.id;
        update_feedback_review(id);
    }


    if (strcmp(req.action, "GET_ALL_LOANS") == 0) {
        int loan_count = 0;
        Loan* loan_array = review_customer_loans(&loan_count); // Replace this with the actual function to retrieve loans

        int total_entries = loan_count; // Count of loans
        ssize_t bytes_sent = send(client_sock, &total_entries, sizeof(int), 0);
        if (bytes_sent < 0) {
            perror("Failed to send loan count");
            return;
        }

        ssize_t total_size = total_entries * sizeof(Loan);
        bytes_sent = send(client_sock, loan_array, total_size, 0);
        if (bytes_sent < 0) {
            perror("Failed to send loan array");
        } else {
            printf("Sent %d loan entries to the client.\n", loan_count);
        }

        // Free the allocated loan_array after sending
        free(loan_array);
    }


    if(strcmp(req.action, "UPDATE_LOAN") == 0) {
        update_loan(req.loan);
    }
}

// int main() {

//     // printAllUser();
//     // return 0;

// User new_user;
// new_user.id = 1; // or whatever logic you have for ID
// strncpy(new_user.username, "admin", sizeof(new_user.username) - 1);
// strncpy(new_user.password, "123", sizeof(new_user.password) - 1);
// new_user.role = 4; // For admin
// new_user.account_balance = 0.0;

//     // FILE *file = fopen("database/data.dat", "ab");
//     // if (file == NULL) {
//     //     printf("Error! Can't open file.\n");
//     //     exit(1);
//     // }

//     // fwrite(&new_user, sizeof(User), 1, file);
//     // fclose(file);


//     // file = fopen("database/data.dat", "rb");
//     // if (file == NULL) {
//     //     printf("Error! Can't open file.\n");
//     //     exit(1);
//     // }

//     // while(fread(&new_user, sizeof(User), 1, file)) {
//     //     printf("ID: %d, Name: %s, Role: %s\n", new_user.id, new_user.username, new_user.password);
//     // }

//     // fclose(file);

//     // return 0;

//     while (1) {
//         char username[50];
//         char password[50];

//         printf("Enter username: ");
//         scanf("%s", username);
//         printf("Enter password: ");
//         scanf("%s", password);

//         if (login(username, password)) {
//             // printf("%s \n", current_user.username);
//             admin_menu();
//         } else {
//             printf("Login Failed...\n");
//         }
//     }


//     return 0;
// }



#define PORT 8080

void* handle_client(void* socket_desc) {
    int client_sock = *(int*)socket_desc;
    free(socket_desc);  // Free the dynamically allocated memory for socket descriptor

    // Handle client login and menu

    while (1) {
        char username[50];
        char password[50];

        // Receive username and password from client
        recv(client_sock, username, sizeof(username), 0);
        recv(client_sock, password, sizeof(password), 0);

        // User new_user;
        // new_user.id = 1;
        // strcpy(new_user.username, "admin");
        // strcpy(new_user.password, "123");
        // new_user.role = 4;
        // new_user.account_balance = 0.0;

        // break;

        // Check credentials
        if (login(username, password)) {
            send(client_sock, &current_user, sizeof(current_user), 0);
            // Now handle actions
            Request req;
            while (1) {
                ssize_t bytes_received = recv(client_sock, &req, sizeof(Request), 0);

                if (bytes_received < 0) {
                    perror("Receive failed");
                    return; // Handle error
                } else if (bytes_received == 0) {
                    printf("Connection closed by peer\n");
                    return; // Handle connection closure
                }

                if (current_user.role == 4) {
                    if (strcmp(req.action, "ADMIN_LOGOUT") == 0) {
                        current_user.id = -1;
                        break;
                    }
                    admin_functions(req, client_sock);
                } else if (current_user.role == 1) {
                    if (strcmp(req.action, "CUSTOMER_LOGOUT") == 0) {
                        current_user.id = -1;
                        break;
                    }
                    customer_functions(req, client_sock);
                } else if (current_user.role == 2) {
                    if (strcmp(req.action, "EMPLOYEE_LOGOUT") == 0) {
                        current_user.id = -1;
                        break;
                    }
                    employee_functions(req, client_sock);
                } else {
                    if (strcmp(req.action, "MANAGER_LOGOUT") == 0) {
                        current_user.id = -1;
                        break;
                    }
                    manager_function(req, client_sock);
                }

                // You can add more actions here (e.g., REMOVE_USER, VIEW_USER, etc.)
            }
        } else {
            // Send failure response
            // current_user.id = -1;
            send(client_sock, &current_user, sizeof(current_user), 0);
        }
    }


    close(client_sock); // Close the socket
    pthread_exit(NULL); // Exit thread
}

int main() {
    int server_sock, client_sock, *new_sock;
    struct sockaddr_in server_addr, client_addr;
    socklen_t client_len = sizeof(client_addr);

    // Create socket
    server_sock = socket(AF_INET, SOCK_STREAM, 0);
    if (server_sock == -1) {
        perror("Could not create socket");
        exit(EXIT_FAILURE);
    }

    // Prepare the sockaddr_in structure
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(PORT);

    // Bind the socket
    if (bind(server_sock, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        perror("Bind failed");
        close(server_sock);
        exit(EXIT_FAILURE);
    }

    // Listen for incoming connections
    listen(server_sock, 5);

    printf("Server is listening on port %d\n", PORT);

    // Accept and handle incoming clients in separate threads
    while ((client_sock = accept(server_sock, (struct sockaddr*)&client_addr, &client_len))) {
        printf("Connection accepted\n");

        // Allocate memory for the client socket descriptor
        new_sock = malloc(sizeof(int));
        *new_sock = client_sock;

        pthread_t client_thread;
        // Create a thread to handle the new client
        if (pthread_create(&client_thread, NULL, handle_client, (void*)new_sock) < 0) {
            perror("Could not create thread");
            free(new_sock);
            close(client_sock);
        }

        // Detach the thread to allow it to clean up automatically after finishing
        pthread_detach(client_thread);
    }

    if (client_sock < 0) {
        perror("Accept failed");
        close(server_sock);
        exit(EXIT_FAILURE);
    }

    close(server_sock);
    return 0;
}

