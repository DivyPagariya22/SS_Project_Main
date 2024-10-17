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
	int id;
	printf("Enter Customer Id to view Transactions: ");
	scanf("%d", &id);


	FILE *file = fopen("database/transaction.dat", "rb+");
	if (file == NULL) {
		printf("Error! Can't open file.\n");
		exit(1);
	}

	Transaction trans;
	// fseek(file, 0, SEEK_SET);
	int i = 20; // Limit to store Transaction
	while (i && fread(&trans, sizeof(trans), 1, file)) {
		if (trans.from == id) {
			i--;
			if (trans.to == id) {
				if (trans.amount < 0) printf("Withdrawn (%.4f) \n", trans.amount);
				else printf("Deposited (%.4f) \n", trans.amount);
			} else {
				if (trans.amount < 0) printf("Transfered (%.4f) to %s \n", trans.amount, trans.to_username);
				else printf("Recived (%.4f) from %s \n", trans.amount, trans.to_username);
			}
		}
	}

	fclose(file);
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


void employee_menu(int sock, User user) {
	while (1) {
		printf("\nEmployee Menu:\n");
		printf("1. Add New Customer\n");
		printf("2. Modify Customer Details\n");
		printf("3. Process Loan Applications\n");
		printf("4. Approve/Reject Loans\n");
		printf("5. View Assigned Loan Applications\n");
		printf("6. View Customer Transactions\n");
		printf("7. Change Password\n");
		printf("8. Logout\n");
		printf("9. Exit\n");

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
			//process_loan_applications(sock, employee);
			break;
		case 4:
			//pprove_reject_loans(sock, employee);
			break;
		case 5:
			//view_assigned_loan_applications(sock, employee);
			break;
		case 6:
			view_customer_transactions(sock);
			break;
		case 7:
			change_password_employee(sock, user);
			break;
		case 8:
			logout_employee(sock);
			return;  // Logout and return to login screen or previous menu
		case 9:
			printf("Exiting...\n");
			exit(0);  // Exit the program
		default:
			printf("Invalid choice. Please try again.\n");
		}
	}

}