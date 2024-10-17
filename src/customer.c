#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>
#include "user.h"
#include "login.h"
#include "globals.h"

User getUserClient(int id) {
	FILE *file = fopen("database/data.dat", "rb");
	fseek(file, (id - 1) * sizeof(User), SEEK_SET);
	User user;
	fread(&user, sizeof(User), 1, file);
	fclose(file);
	return user;
}

void logout_customer(int sock) {
	printf("\n");
	Request req;
	strcpy(req.action, "CUSTOMER_LOGOUT");
	send(sock, &req, sizeof(Request), 0);
}

void get_account_balance(int sock, User user) {
	Request req;
	strcpy(req.action, "GET_BALANCE");
	req.user = user;
	send(sock, &req, sizeof(Request), 0);


	ssize_t bytes_received = recv(sock, &user, sizeof(user), 0);
	if (bytes_received < 0) {
		perror("Receive failed");
		return; // Handle error
	} else if (bytes_received == 0) {
		printf("Connection closed by peer\n");
		return;
	}
	// printf("Current Account Balance: %f\n", user.account_balance);
	printf("\n Current Account Balance: %.4f \n", user.account_balance);
}

void deposit_money(int sock, User user, bool flag) {

	float deposit_amount;
	printf("Enter the amount to deposit: ");
	scanf("%f", &deposit_amount);
	user = getUserClient(user.id);
	if (!flag && deposit_amount > user.account_balance) {
		printf("\n You have insuffienct funds \n");
		return;
	}
	if (!flag) deposit_amount = -1.00 * deposit_amount;
	Request req;
	req.amount = deposit_amount;
	req.user = user;
	strcpy(req.action, "DEPOSIT");
	ssize_t bytes = send(sock, &req, sizeof(Request), 0);
	if (bytes < 0) {
		printf("Transaction Failed \n");
	} else {
		printf("Transaction Succesfull !!\n");
	}


}

void transfer_funds(int sock, User user) {
	int id;
	printf("Enter the Account ID for Transfer: ");
	scanf("%d", &id);
	float amount;
	printf("Enter the amount to send: ");
	scanf("%f", &amount);
	Request req;
	req.transfer_id = id;
	req.user = user;
	req.amount = amount;
	strcpy(req.action, "TRANSFER_FUNDS");
	ssize_t bytes = send(sock, &req, sizeof(Request), 0);
	if (bytes < 0) {
		printf("Transaction Failed \n");
	}

	char err[50];
	ssize_t bytes_received = recv(sock, &err, sizeof(err), 0);


	if (strcmp(err, "insuffienct funds") == 0) printf("\n You have %s\n", err);

}


void change_password(int sock, User user) {
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

void view_transaction_history(int sock, User user) {
	FILE *file = fopen("database/transaction.dat", "rb+");
	if (file == NULL) {
		printf("Error! Can't open file.\n");
		exit(1);
	}

	Transaction trans;
	// fseek(file, 0, SEEK_SET);
	int i = 20; // Limit to store Transaction
	while (i && fread(&trans, sizeof(trans), 1, file)) {
		if (trans.from == user.id) {
			i--;
			if (trans.to == user.id) {
				if (trans.amount < 0) printf("You have Withdrawn (%.4f) \n", trans.amount);
				else printf("You have Deposited (%.4f) \n", trans.amount);
			} else {
				if (trans.amount < 0) printf("You have Transfered (%.4f) to %s \n", trans.amount, trans.to_username);
				else printf("You have Recived (%.4f) from %s \n", trans.amount, trans.to_username);
			}
		}
	}

	fclose(file);
}

void add_feedback(int sock, User user) {

	int c;
	while ((c = getchar()) != '\n' && c != EOF); // Clear the input buffer

	char sentence[1000];

	printf("Enter a sentence: ");
	fgets(sentence, sizeof(sentence), stdin);

	// Remove the newline character at the end, if present
	sentence[strcspn(sentence, "\n")] = '\0';

	Request req;
	req.user = user;

	Feedback feedback;
	feedback.customerID = user.id;
	strcpy(feedback.customerName, user.username);
	strcpy(feedback.customerFeedback, sentence);
	feedback.review = 0;

	req.feedback = feedback;

	strcpy(req.action, "ADD_FEEDBACK");

	printf("%s \n", req.feedback.customerFeedback);

	ssize_t bytes = send(sock, &req, sizeof(Request), 0);
	if (bytes < 0) {
		printf("Feedback Failed !!\n");
	} else {
		printf("Feedback Submitted !!\n");
	}

}

void apply_for_loan(int sock, User user) {
    Loan loan;
    loan.user_id = user.id;
    strcpy(loan.username, user.username);
    loan.employee_id = -1; // No employee assigned yet
    strcpy(loan.status, "Processing"); // Initial status
    loan.amount = 0.0;

    printf("Enter the loan amount: ");
    scanf("%f", &loan.amount);

    printf("Select the type of loan:\n");
    printf("1. Car Loan\n");
    printf("2. Gold Loan\n");
    printf("3. Home Loan\n");
    printf("4. Education Loan\n");
    printf("5. Personal Loan\n");
    
    int loan_type_choice;
    printf("Enter your choice: ");
    scanf("%d", &loan_type_choice);

    switch (loan_type_choice) {
        case 1:
            strcpy(loan.type, "Car");
            break;
        case 2:
            strcpy(loan.type, "Gold");
            break;
        case 3:
            strcpy(loan.type, "Home");
            break;
        case 4:
            strcpy(loan.type, "Education");
            break;
        case 5:
            strcpy(loan.type, "Personal");
            break;
        default:
            printf("Invalid choice. Please try again.\n");
            return;
    }

    // Send loan request to server
    Request req;
    req.loan = loan;
    strcpy(req.action, "APPLY_LOAN");

    ssize_t bytes = send(sock, &req, sizeof(Request), 0);
    if (bytes < 0) {
        printf("Loan application failed.\n");
    } else {
        printf("Loan application submitted successfully!\n");
    }
}


void customer_menu(int sock, User user) {
	while (1) {
		printf("\nCustomer Menu:\n");
		printf("1. View Account Balance\n");
		printf("2. Deposit Money\n");
		printf("3. Withdraw Money\n");
		printf("4. Transfer Funds\n");
		printf("5. Apply for a Loan\n");
		printf("6. Change Password\n");
		printf("7. Add Feedback\n");
		printf("8. View Transaction History\n");
		printf("9. Logout\n");
		printf("10. Exit\n");

		int choice;
		printf("Enter your choice: ");
		scanf("%d", &choice);

		switch (choice) {
		case 1:
			get_account_balance(sock, user);
			break;
		case 2:
			deposit_money(sock, user, 1);
			break;
		case 3:
			deposit_money(sock, user, 0);
			break;
		case 4:
			transfer_funds(sock, user);
			break;
		case 5:
			apply_for_loan(sock, user);
			break;
		case 6:
			change_password(sock, user);
			break;
		case 7:
			add_feedback(sock, user);
			break;
		case 8:
			printf("\n");
			view_transaction_history(sock, user);
			printf("\n");
			break;
		case 9:
			logout_customer(sock);
			return;
		case 10:
			printf("Exiting...\n");
			exit(0); // Exit the program
		default:
			printf("Invalid choice. Please try again.\n");
		}
	}
}


