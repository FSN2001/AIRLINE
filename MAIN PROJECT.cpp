#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define MAX_FLIGHTS 50
#define MAX_SEATS 30
#define MAX_USERS 100

typedef struct {
    int seatNumber;
    int isBooked;
    char passengerName[50];
} Seat;

typedef struct {
    int flightID;
    char origin[30];
    char destination[30];
    char date[15];
    char time[10];
    int totalSeats;
    Seat seats[MAX_SEATS];
} Flight;

typedef struct {
    char username[30];
    char password[30];
    int isAdmin;  // 1 = Admin, 0 = Passenger
} User;

Flight flights[MAX_FLIGHTS];
User users[MAX_USERS];
int flightCount = 0;
int userCount = 0;
User *currentUser = NULL;

// Forward declarations
void loadFlights();
void saveFlights();
void loadUsers();
void saveUsers();
void loginMenu();
void adminMenu();
void passengerMenu();
void bookSeat();
void viewSeatMap(int flightID);
void addFlight();
void registerUser();
void searchFlights();
void viewAllFlights();
void injectDefaults();

void pause() {
    printf("\nPress Enter to continue...");
    getchar(); getchar();
}

// File Persistence
void loadFlights() {
    FILE *fp = fopen("flights.dat", "rb");
    if (fp) {
        fread(&flightCount, sizeof(int), 1, fp);
        fread(flights, sizeof(Flight), flightCount, fp);
        fclose(fp);
    }
}

void saveFlights() {
    FILE *fp = fopen("flights.dat", "wb");
    fwrite(&flightCount, sizeof(int), 1, fp);
    fwrite(flights, sizeof(Flight), flightCount, fp);
    fclose(fp);
}

void loadUsers() {
    FILE *fp = fopen("users.dat", "rb");
    if (fp) {
        fread(&userCount, sizeof(int), 1, fp);
        fread(users, sizeof(User), userCount, fp);
        fclose(fp);
    }
}

void saveUsers() {
    FILE *fp = fopen("users.dat", "wb");
    fwrite(&userCount, sizeof(int), 1, fp);
    fwrite(users, sizeof(User), userCount, fp);
    fclose(fp);
}

// Login/Register
void registerUser() {
    User u;
    printf("\nEnter new username: ");
    scanf("%s", u.username);
    printf("Enter password: ");
    scanf("%s", u.password);
    u.isAdmin = 0; // passengers only here
    users[userCount++] = u;
    saveUsers();
    printf("Registration successful!\n");
}

void loginMenu() {
    int choice;
    char user[30], pass[30];
    while (1) {
        printf("\n--- Airline Reservation System ---\n");
        printf("press 1 for Login\n");
        printf("press 2 for Register\n");
        printf("press 3 for Exit\n");
        printf("Enter choice: ");
        scanf("%d", &choice);

        if (choice == 1) {
            printf("Username: ");
            scanf("%s", user);
            printf("Password: ");
            scanf("%s", pass);
            for (int i = 0; i < userCount; i++) {
                if (strcmp(users[i].username, user) == 0 && strcmp(users[i].password, pass) == 0) {
                    currentUser = &users[i];
                    printf("Login successful! Welcome to FSN airways, %s\n", currentUser->username);
                    if (currentUser->isAdmin)
                        adminMenu();
                    else
                        passengerMenu();
                    return;
                }
            }
            printf("Invalid credentials!\n");
        } else if (choice == 2) {
            registerUser();
        } else {
            exit(0);
        }
    }
}

// Admin Functions
void addFlight() {
	int n;
    Flight f;
    printf("=========WELCOME TO FSN AIRLINE===============\n");
    printf("how many flights you want to add:");
    scanf("%d",&n);
    for(int i=0;i<n;i++){
    printf("\nEnter Flight ID: ");
    scanf("%d", &f.flightID);
    printf("Origin: ");
    scanf("%s", f.origin);
    printf("Destination: ");
    scanf("%s", f.destination);
    printf("Date (dd-mm-yyyy): ");
    scanf("%s", f.date);
    printf("Time (HH:MM)(0 to 24 ): ");
    scanf("%s", f.time);
    f.totalSeats = MAX_SEATS;

    for (int i = 0; i < MAX_SEATS; i++) {
        f.seats[i].seatNumber = i + 1;
        f.seats[i].isBooked = 0;
        strcpy(f.seats[i].passengerName, "");
    }

    flights[flightCount++] = f;
    saveFlights();
    printf("Flight added successfully!\n");

	}
    
}

void viewAllFlights() {
    printf("\n--- All Flights ---\n");
    for (int i = 0; i < flightCount; i++) {
        printf("FlightID: %d | %s to %s | Date: %s | Time: %s\n",
            flights[i].flightID,
            flights[i].origin,
            flights[i].destination,
            flights[i].date,
            flights[i].time);
    }
}

// Passenger Functions
void searchFlights() {
    char src[30], dest[30];
    int found = 0;
    printf("\nEnter Origin: ");
    scanf("%s", src);
    printf("Enter Destination: ");
    scanf("%s", dest);

    for (int i = 0; i < flightCount; i++) {
        if (strcmp(flights[i].origin, src) == 0 && strcmp(flights[i].destination, dest) == 0) { // Fixed the logical operator
            printf("FlightID: %d | Date: %s | Time: %s\n",
                flights[i].flightID,
                flights[i].date,
                flights[i].time);
            found = 1;
        }
    }
    if (!found)
        printf("No flights found.\n");
}
void bookSeat() {
    int fID, seatNo, numSeats;
    printf("\nEnter Flight ID: ");
    scanf("%d", &fID);

    for (int i = 0; i < flightCount; i++) {
        if (flights[i].flightID == fID) {
            viewSeatMap(fID);
            printf("How many seats do you want to book? ");
            scanf("%d", &numSeats);

            for (int j = 0; j < numSeats; j++) {
                printf("Choose seat number %d to book: ", j + 1);
                scanf("%d", &seatNo);
                
                if (seatNo < 1 || seatNo > MAX_SEATS) {
                    printf("Invalid seat number! Please choose a number between 1 and %d.\n", MAX_SEATS);
                    return;
                }
                if (flights[i].seats[seatNo - 1].isBooked) {
                    printf("Seat %d is already booked! Please choose another seat.\n", seatNo);
                    j--; // Decrement j to allow the user to re-enter a seat number for this iteration
                    continue; // Skip to the next iteration of the loop
                }
                flights[i].seats[seatNo - 1].isBooked = 1;
                strcpy(flights[i].seats[seatNo - 1].passengerName, currentUser->username);
                printf("Seat %d booked successfully!\n", seatNo);
            }
            saveFlights();
            return;
        }
    }
    printf("Flight ID not found!\n");
}



void viewSeatMap(int flightID) {
    for (int i = 0; i < flightCount; i++) {
        if (flights[i].flightID == flightID) {
            printf("\nSeat Map for Flight %d:\n", flightID);
            for (int j = 0; j < MAX_SEATS; j++) {
                if (j % 6 == 0) printf("\n");
                if (flights[i].seats[j].isBooked)
                    printf("[XX] ");
                else
                    printf("[%02d] ", j + 1);
            }
            printf("\n");
            return;
        }
    }
}

// Menu Functions
void adminMenu() {
    int ch;
    while (1) {
        printf("\n--- Admin Menu ---\n");
        printf("1. Add Flight\n");
        printf("2. View Flights\n");
        printf("3. Logout\n");
        printf("Enter choice: ");
        scanf("%d", &ch);
        if (ch == 1) addFlight();
        else if (ch == 2) viewAllFlights();
        else break;
    }
}

void passengerMenu() {
    int ch;
    while (1) {
        printf("\n--- Passenger Menu ---\n");
        printf("press 1 for Search Flights\n");
        printf("press 2 for Book Ticket\n");
        printf("press 3 for Exit\n");
        printf("Enter choice: ");
        scanf("%d", &ch);
        if (ch == 1) searchFlights();
        else if (ch == 2) bookSeat();
        else break;
    }
}

void injectDefaults() {
    if (userCount == 0) {
        strcpy(users[userCount].username, "admin");
        strcpy(users[userCount].password, "admin123");
        users[userCount].isAdmin = 1;
        userCount++;
        saveUsers();
    }

    if (flightCount == 0) {
        Flight f;
        f.flightID = 1001;
        strcpy(f.origin, "chennai");
        strcpy(f.destination, "delhi");
        strcpy(f.date, "15-04-2025");
        strcpy(f.time, "10:00");
        f.totalSeats = MAX_SEATS;
        for (int i = 0; i < MAX_SEATS; i++) {
            f.seats[i].seatNumber = i + 1;
            f.seats[i].isBooked = 0;
            strcpy(f.seats[i].passengerName, "");
        }
        flights[flightCount++] = f;

        strcpy(f.origin, "mumbai");
        strcpy(f.destination, "bangalore");
        f.flightID = 1002;
        strcpy(f.date, "16-04-2025");
        strcpy(f.time, "08:00");
        for (int i = 0; i < MAX_SEATS; i++) {
            f.seats[i].seatNumber = i + 1;
            f.seats[i].isBooked = 0;
            strcpy(f.seats[i].passengerName, "");
        }
        flights[flightCount++] = f;

        saveFlights();
    }
}

// Main
int main() {
	addFlight();
    loadFlights();
    loadUsers();
    injectDefaults();
    loginMenu();
    return 0;
}



