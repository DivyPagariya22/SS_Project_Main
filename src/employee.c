#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>
#include "user.h"
#include "login.h"
#include "globals.h"


void logout_employee(int sock) {
	printf("\n");
	Request req;
	strcpy(req.action, "EMPLOYEE_LOGOUT");
	send(sock, &req, sizeof(Request), 0);
}

void add_new_customer(int sock) {
	printf("Adding a new Customer ...\n");
	Request req;
	char username[50];
	char password[50];
	printf("Enter Customer username: ");
	scanf("%s", username);
	printf("Enter Customer password: ");
	scanf("%s", password);
	User user;
	strcpy(user.username, username);
	strcpy(user.password, password);
	strcpy(req.action, "ADD_CUSTOMER");
	user.role = 1;
	req.user = user;
	send(sock, &req, sizeof(Request), 0);
}

void modify_customer_details(int sock) {
	int id;
	printf("Enter Customer Id: ");
	scanf("%d", &id);

	Request req;
	User user;

	strcpy(req.action, "GET_USER");
	req.user.id = id;
	send(sock, &req, sizeof(req), 0);

	ssize_t bytes_received = recv(sock, &user, sizeof(user), 0);
	if (bytes_received < 0) {
		perror("Receive failed");
		return; // Handle error
	} else if (bytes_received == 0) {
		printf("Connection closed by peer\n");
		return; // Handle connection closure
	}


	strcpy(req.action, "MODIFY_CUSTOMER");

	char choice[5];
	user.id = id;

	printf("Modify username? (yes/no): ");
	scanf("%s", choice);
	if (strcmp(choice, "yes") == 0) {
		printf("Enter new username: ");
		scanf("%s", user.username);
	}

	printf("Modify password? (yes/no): ");
	scanf("%s", choice);
	if (strcmp(choice, "yes") == 0) {
		printf("Enter new password: ");
		scanf("%s", user.password);
	}

	req.user = user;
	ssize_t bytes = send(sock, &req, sizeof(Request), 0);
	if (bytes < 0) {
		printf("Modification Failed \n");
	} else {
		printf("Modified Succesfully !!\n");
	}

}

void view_customer_transactions(int sock) {


	int customer_id;
	printf("Enter Customer ID to view Transactions: ");
	scanf("%d", &customer_id);

	Request req;
	strcpy(req.action, "GET_TRANSACTIONS");
	req.user.id = customer_id;



	// Send request to server
	ssize_t bytes = send(sock, &req, sizeof(Request), 0);
	if (bytes < 0) {
		printf("Failed to request transactions.\n");
		return;
	}

	// Receive the number of transactions
	int transaction_count = 0;
	ssize_t bytes_received = recv(sock, &transaction_count, sizeof(int), 0);
	if (bytes_received <= 0) {
		printf("Failed to receive transaction count.\n");
		return;
	}

	// If no transactions found, inform the user
	if (transaction_count == 0) {
		printf("No transactions found for this customer.\n");
		return;
	}

	// Allocate memory to receive the transactions
	Transaction* transaction_array = (Transaction*)malloc(transaction_count * sizeof(Transaction));
	if (transaction_array == NULL) {
		printf("Memory allocation failed.\n");
		return;
	}

	// Receive the transaction array
	ssize_t total_size = transaction_count * sizeof(Transaction);
	bytes_received = recv(sock, transaction_array, total_size, 0);
	if (bytes_received <= 0) {
		printf("Failed to receive transaction data.\n");
		free(transaction_array);
		return;
	}

	// Display the list of transactions
	printf("\nCustomer Transactions:\n");
	for (int i = 0; i < transaction_count; i++) {
		Transaction trans = transaction_array[i];
		if (trans.from == customer_id) {
			if (trans.to == customer_id) {
				if (trans.amount < 0) printf("Withdrawn (%.4f) \n", trans.amount);
				else printf("Deposited (%.4f) \n", trans.amount);
			} else {
				if (trans.amount < 0) printf("Transfered (%.4f) to %s \n", trans.amount, trans.to_username);
				else printf("Recived (%.4f) from %s \n", trans.amount, trans.to_username);
			}
		}
	}

	// Free the allocated memory
	free(transaction_array);



	// int id;
	// printf("Enter Customer Id to view Transactions: ");
	// scanf("%d", &id);


	// FILE *file = fopen("database/transaction.dat", "rb+");
	// if (file == NULL) {
	// 	printf("Error! Can't open file.\n");
	// 	exit(1);
	// }

	// Transaction trans;
	// // fseek(file, 0, SEEK_SET);
	// int i = 20; // Limit to store Transaction
	// while (i && fread(&trans, sizeof(trans), 1, file)) {
	// 	if (trans.from == id) {
	// 		i--;
	// 		if (trans.to == id) {
	// 			if (trans.amount < 0) printf("Withdrawn (%.4f) \n", trans.amount);
	// 			else printf("Deposited (%.4f) \n", trans.amount);
	// 		} else {
	// 			if (trans.amount < 0) printf("Transfered (%.4f) to %s \n", trans.amount, trans.to_username);
	// 			else printf("Recived (%.4f) from %s \n", trans.amount, trans.to_username);
	// 		}
	// 	}
	// }

	// fclose(file);
}

void change_password_employee(int sock, User user) {
	char new_password[50];
	printf("Enter New password: ");
	scanf("%s", new_password);
	strcpy(user.password, new_password);
	Request req;
	req.user = user;
	strcpy(req.action, "CHANGE_PASSWORD");

	ssize_t bytes = send(sock, &req, sizeof(Request), 0);
	if (bytes < 0) {
		printf("Password was not changed\n");
	} else {
		printf("Succesfull changed !!\n");
	}
}


void view_assigned_loan_applications(int sock, User user) {
	Request req;


	strcpy(req.action, "GET_ASSIGNED_LOANS");
	req.user = user;


	ssize_t bytes = send(sock, &req, sizeof(Request), 0);
	if (bytes < 0) {
		printf("Failed to send request to get assigned loans.\n");
		return;
	}

	printf("Request sent to get assigned loan applications.\n");


	int loan_count = 0;
	ssize_t bytes_received = recv(sock, &loan_count, sizeof(int), 0);
	if (bytes_received <= 0) {
		perror("Failed to receive the count of assigned loans.");
		return;
	}


	Loan* loan_array = (Loan*)malloc(loan_count * sizeof(Loan));
	if (loan_array == NULL) {
		printf("Memory allocation failed for loan array.\n");
		return;
	}


	ssize_t total_size = loan_count * sizeof(Loan);
	bytes_received = recv(sock, loan_array, total_size, 0);
	if (bytes_received <= 0) {
		perror("Failed to receive the loan array.");
		free(loan_array);
		return;
	}


	printf("Assigned Loan Applications:\n");
	for (int i = 0; i < loan_count; i++) {
		printf("%d. User ID: %d, Username: %s, Loan Type: %s, Amount: %.2f, Status: %s\n",
		       i + 1, loan_array[i].user_id, loan_array[i].username, loan_array[i].type,
		       loan_array[i].amount, loan_array[i].status);
	}


	free(loan_array);
}


void approve_reject_loans(int sock, User employee) {
	Request req;
	strcpy(req.action, "GET_ASSIGNED_LOANS");
	req.user = employee;

	ssize_t bytes = send(sock, &req, sizeof(Request), 0);
	if (bytes < 0) {
		printf("Failed to request assigned loans.\n");
		return;
	}

	int loan_count = 0;
	ssize_t bytes_received = recv(sock, &loan_count, sizeof(int), 0);
	if (bytes_received <= 0) {
		printf("Failed to receive assigned loan count.\n");
		return;
	}

	if (loan_count == 0) {
		printf("No assigned loans found for this employee.\n");
		return;
	}

	Loan* loan_array = (Loan*)malloc(loan_count * sizeof(Loan));
	if (loan_array == NULL) {
		printf("Memory allocation failed.\n");
		return;
	}

	ssize_t total_size = loan_count * sizeof(Loan);
	bytes_received = recv(sock, loan_array, total_size, 0);
	if (bytes_received <= 0) {
		printf("Failed to receive loan data.\n");
		free(loan_array);
		return;
	}

	printf("\nAssigned Loan Applications:\n");
	for (int i = 0; i < loan_count; i++) {
		printf("%d. Loan ID: %d, Customer ID: %d, Amount: %.2f, Type: %s, Status: %s\n",
		       i + 1, loan_array[i].user_id, loan_array[i].user_id,
		       loan_array[i].amount, loan_array[i].type, loan_array[i].status);
	}

	int index;
	printf("Enter the index of the loan to review (1 - %d): ", loan_count);
	scanf("%d", &index);

	if (index < 1 || index > loan_count) {
		printf("Invalid loan index.\n");
		free(loan_array);
		return;
	}

	char decision[10];
	printf("Approve or Reject the loan (approve/reject)? ");
	scanf("%s", decision);

	Loan selected_loan = loan_array[index - 1];
	if (strcmp(decision, "approve") == 0) {
		strcpy(selected_loan.status, "Approved");
	} else if (strcmp(decision, "reject") == 0) {
		strcpy(selected_loan.status, "Rejected");
	} else {
		printf("Invalid decision. Please choose 'approve' or 'reject'.\n");
		free(loan_array);
		return;
	}

	strcpy(req.action, "UPDATE_LOAN");
	req.loan = selected_loan;
	bytes = send(sock, &req, sizeof(Request), 0);
	if (bytes < 0) {
		printf("Failed to send updated loan to the server.\n");
	} else {
		printf("Loan status updated successfully.\n");
	}

	free(loan_array);
}




void employee_menu(int sock, User user) {
	while (1) {
		printf("\nEmployee Menu:\n");
		printf("1. Add New Customer\n");
		printf("2. Modify Customer Details\n");
		printf("3. Approve/Reject Loans\n");
		printf("4. View Assigned Loan Applications\n");
		printf("5. View Customer Transactions\n");
		printf("6. Change Password\n");
		printf("7. Logout\n");
		printf("8. Exit\n");

		int choice;
		printf("Enter your choice: ");
		scanf("%d", &choice);

		switch (choice) {
		case 1:
			add_new_customer(sock);
			break;
		case 2:
			modify_customer_details(sock);
			break;
		case 3:
			approve_reject_loans(sock, user);
			break;
		case 4:
			view_assigned_loan_applications(sock, user);
			break;
		case 5:
			view_customer_transactions(sock);
			break;
		case 6:
			change_password_employee(sock, user);
			break;
		case 7:
			logout_employee(sock);
			return;  // Logout and return to login screen or previous menu
		case 8:
			printf("Exiting...\n");
			exit(0);  // Exit the program
		default:
			printf("Invalid choice. Please try again.\n");
		}
	}

}