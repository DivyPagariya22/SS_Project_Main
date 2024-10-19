# SS_MiniProject

# Banking Management System

A multi-role banking management system allowing customers, bank employees, managers, and administrators to perform various banking operations. The system employs socket programming, file locking, and threading to handle concurrent operations while ensuring data consistency and synchronization.

## Programming Languages and Technologies Used

- **Programming Language**: C
- **Socket Programming**: For communication between the server and clients.
- **File Locking**: To ensure data consistency during concurrent access.
- **System Calls**: For various operations like file management and threading.
- **Threading**: To handle multiple clients simultaneously.

## Getting Started

Follow these steps to start the project:

### Build the Project

Run the following command to compile the project:

```
make
```

```
./server
```


# Open client in another terminal

```
./client
```


## Additional Features and Functionalities

### User Roles and Access Control

- **Role-Based Access**: Different user roles (Customer, Bank Employee, Manager, Administrator) are defined, with specific permissions and functionalities for each role.
- **User Management**: Administrators can add, modify, and remove users, ensuring proper management of access and roles.

### Transaction Management

- **Transaction History**: Users can view a detailed history of their transactions, including deposits, withdrawals, and transfers.
- **Real-Time Updates**: The system provides real-time updates for account balances and transaction statuses.

### Loan Management System

- **Loan Application Process**: A structured workflow for loan applications, including submission, review, approval, and rejection.
- **Loan Types**: Multiple loan types (e.g., Personal, Car, Home) are available for customers, each with specific criteria and terms.

### Feedback and Support

- **Feedback Submission**: Customers can submit feedback on services, which is reviewed by managers for continuous improvement.
- **Help and Support**: Users can access support features for assistance with their banking needs.

### Performance and Scalability

- **Optimized Performance**: The system is designed to handle multiple concurrent users efficiently, ensuring minimal latency.
- **Scalability**: The architecture allows for easy scaling to accommodate a growing number of users and transactions.

### Future Enhancements

- **Mobile App Integration**: Plans for developing a mobile application to provide banking services on-the-go.
- **Enhanced Security Measures**: Future updates may include multi-factor authentication and additional encryption for sensitive data.







