#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>
#include "user.h"
#include "login.h"
#include "globals.h"


void delete_feedback(Feedback* feedback_array, int* total_entries, int index_to_delete) {
	if (index_to_delete < 0 || index_to_delete >= *total_entries) {
		printf("Invalid index for deletion.\n");
		return; // Prevent segmentation fault by exiting if the index is invalid
	}

	// If the user confirms deletion, remove the feedback at the specified index


	// Optionally clear the last element
	feedback_array[*total_entries - 1].customerID = -1; // Mark it as deleted
	(*total_entries)--; // Decrease the count after deletion
}


void deactivate_customer_account(int sock) {
	Request req;
	int id;
	printf("Enter the Customer ID to deactivate its account: ");
	scanf("%d", &id);
	strcpy(req.action, "DEACTIVATE_ACC");
	req.user.id = id;
	ssize_t bytes = send(sock, &req, sizeof(Request), 0);
	if (bytes < 0) {
		printf("Deactivation Failed \n");
	} else {
		printf("Deactivation Succesfully !!\n");
	}
}


void activate_customer_account(int sock) {
	Request req;
	int id;
	printf("Enter the Customer ID to activate its account: ");
	scanf("%d", &id);
	strcpy(req.action, "ACTIVATE_ACC");
	req.user.id = id;
	ssize_t bytes = send(sock, &req, sizeof(Request), 0);
	if (bytes < 0) {
		printf("Activation Failed \n");
	} else {
		printf("Activation Succesfully !!\n");
	}
}

void logout_manager(int sock) {
	printf("\n");
	Request req;
	strcpy(req.action, "MANAGER_LOGOUT");
	send(sock, &req, sizeof(Request), 0);
}

void change_password_manager(int sock, User user) {
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


void review_customer_feedback_manager(int sock, User user) {

	Request req;

	strcpy(req.action, "GET_ALL_FEEDBACK");
	req.user = user;
	ssize_t bytes = send(sock, &req, sizeof(Request), 0);
	if (bytes < 0) {
		printf("Feedback cant be fetched\n");
	}


	int total_entries = 0;
	ssize_t bytes_received = recv(sock, &total_entries, sizeof(int), 0);
	if (bytes_received <= 0) {
		perror("Failed to receive feedback count");
		return NULL;
	}

	// Allocate memory for the feedback array
	Feedback* feedback_array = (Feedback*)malloc(total_entries * sizeof(Feedback));
	if (feedback_array == NULL) {
		printf("Memory allocation failed.\n");
		return NULL;
	}

	// Receive the entire array
	ssize_t total_size = total_entries * sizeof(Feedback);
	bytes_received = recv(sock, feedback_array, total_size, 0);
	if (bytes_received <= 0) {
		perror("Failed to receive feedback array");
		// free(feedback_array);

		return;
	}

	// total_entries--;

	for (int i = 0; i < total_entries; i++) {
		printf("%d. Customer Id: %d, Customer Name: %s\n", i + 1, feedback_array[i].customerID, feedback_array[i].customerName);
		printf("%s\n", feedback_array[i].customerFeedback);
		printf("\n");
	}



	// printf("\n");

	// FILE *file = fopen("database/feedback.dat", "rb");
	// fseek(file, 0, SEEK_SET);

	// Feedback feedback;
	// int i = 1;
	// while (fread(&feedback, sizeof(Feedback), 1, file)) {
	// 	printf("%d. Customer Id: %d, Customer Name: %s \n", i, feedback.customerID, feedback.customerName);
	// 	printf("%s \n", feedback.customerFeedback);
	// 	printf("\n");
	// 	i++;
	// }

	// fclose(file);
}


void clear_feedback(int sock, User user) {
	printf("Enter the Customer Id to clear its feedback: ");
	int id;
	scanf("%d", &id);
	Request req;

	strcpy(req.action, "CLEAR_FEEDBACK");
	req.user.id = id;

	ssize_t bytes = send(sock, &req, sizeof(Request), 0);
	if (bytes < 0) {
		printf("Feedback was not cleared\n");
	}
}

void review_customer_loan_manager(int sock, User user) {

	Request req;

	strcpy(req.action, "GET_ALL_LOANS");

	req.user = user;

	ssize_t bytes = send(sock, &req, sizeof(Request), 0);
	if (bytes < 0) {
		printf("Loans cannot be fetched\n");
		return;
	}


	int total_entries = 0;
	ssize_t bytes_received = recv(sock, &total_entries, sizeof(int), 0);
	if (bytes_received <= 0) {
		perror("Failed to receive loan count");
		return;
	}


	Loan* loan_array = (Loan*)malloc(total_entries * sizeof(Loan));
	if (loan_array == NULL) {
		printf("Memory allocation failed.\n");
		return;
	}


	ssize_t total_size = total_entries * sizeof(Loan);
	bytes_received = recv(sock, loan_array, total_size, 0);
	if (bytes_received <= 0) {
		perror("Failed to receive loan array");
		// free(loan_array);
		return;
	}

	for (int i = 0; i < total_entries; i++) {
		printf("%d. User Id: %d, Username: %s, Amount: %.2f, Status: %s, Type: %s, Assigned: %d\n",
		       i + 1,
		       loan_array[i].user_id,
		       loan_array[i].username,
		       loan_array[i].amount,
		       loan_array[i].status,
		       loan_array[i].type,
		       loan_array[i].employee_id);
		printf("\n");
	}


	free(loan_array);
}

void assign_employee(int sock, User user) {

	Request req;
	strcpy(req.action, "GET_ALL_LOANS");
	req.user = user;

	ssize_t bytes_sent = send(sock, &req, sizeof(Request), 0);
	if (bytes_sent < 0) {
		printf("Failed to send loan request.\n");
		return;
	}


	int loan_count = 0;
	ssize_t bytes_received = recv(sock, &loan_count, sizeof(int), 0);
	if (bytes_received <= 0) {
		perror("Failed to receive loan count");
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
		perror("Failed to receive loan array");
		free(loan_array); // Free memory on failure
		return;
	}


	printf("Available Loans:\n");
	for (int i = 0; i < loan_count; i++) {
		printf("%d. User ID: %d, Username: %s, Amount: %.2f, Status: %s, Type: %s\n",
		       i + 1, loan_array[i].user_id, loan_array[i].username, loan_array[i].amount,
		       loan_array[i].status, loan_array[i].type);
	}


	int index_to_assign;
	printf("Enter the index of the loan to assign an employee (1-%d): ", loan_count);
	scanf("%d", &index_to_assign);

	// Validate the index
	if (index_to_assign < 1 || index_to_assign > loan_count) {
		printf("Invalid index. Please try again.\n");
		free(loan_array);
		return;
	}


	int employee_id;
	printf("Enter the Employee ID to assign: ");
	scanf("%d", &employee_id);


	loan_array[index_to_assign - 1].employee_id = employee_id;


	strcpy(req.action, "UPDATE_LOAN");
	req.loan = loan_array[index_to_assign - 1];


	ssize_t bytes_send_loan = send(sock, &req, sizeof(Request), 0);
	if (bytes_send_loan < 0) {
		printf("Failed to send updated loan to server.\n");
	} else {
		printf("Loan assigned successfully to employee ID %d.\n", employee_id);
	}


	free(loan_array); // Free allocated memory
}



void manager_menu(int sock, User user) {
	while (1) {
		printf("\n Manager Menu:\n");
		printf("1. Deactivate Customer Accounts\n");
		printf("2. Activate Customer Accounts\n");
		printf("3. Assign Loan Application Processes to Employees\n");
		printf("4. View Customer Feedback\n");
		printf("5. Clear Feedback of Customer \n");
		printf("6. View Loan Applications \n");
		printf("7. Assign Loan Application to Employee\n");
		printf("8. Change Password\n");
		printf("9. Logout\n");
		printf("10. Exit\n");


		int choice;
		printf("Enter your choice: ");
		scanf("%d", &choice);

		switch (choice) {
		case 1:
			deactivate_customer_account(sock);
			break;
		case 2:
			activate_customer_account(sock);
			break;
		case 3:
			break;
		case 4:
			printf("\n");
			review_customer_feedback_manager(sock, user);
			break;
		case 5:
			clear_feedback(sock, user);
			break;
		case 6:
			printf("\n");
			review_customer_loan_manager(sock, user);
			break;
		case 7:
			assign_employee(sock, user);
			break;
		case 8:
			change_password_manager(sock, user);
			break;
		case 9:
			logout_manager(sock);
			return;  // Logout and return to login screen or previous menu
		case 10:
			printf("Exiting...\n");
			exit(0);  // Exit the program
		default:
			printf("Invalid choice. Please try again.\n");
		}
	}

}