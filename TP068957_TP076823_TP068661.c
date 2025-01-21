#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <locale.h>
#include <ctype.h>

#define MAX_USERS 200
#define MAX_STUDENTS 100
#define MAX_LECTURERS 20
#define MAX_COURSES 10
#define MAX_GRADES 5
#define MAX_LENGTH 50 // for user input
#define MAX_DATE 12
#define MAX_HOLIDAYS 20
#define MAX_FEEDBACK_LENGTH 1000
#define MAX_ENTRIES 100
#define MAX_ATTENDANCE_RECORDS 20 


// ----------------------------------------------------------------
// ---------- STRUCTS ---------------------------------------------
// ----------------------------------------------------------------

typedef struct {
    int phone_number;
    char dob[11];
    char nationality[30];
    char gender;
} Details;

// Define a struct for attendance record
typedef struct AttendanceNode {
    int studentID;
    char date[5];
    int attendanceLab;
    int attendanceLecture;
    struct AttendanceNode* next; // Pointer to the next node in the linked list
} AttendanceNode;

typedef struct {
    int courseID;
    char course_name[MAX_LENGTH];
    int lecturerID;
    float marks;
    char grade;
    AttendanceNode* attendance_head; // Head of the attendance linked list
} Course;

typedef struct {
    int studentID;
    char student_fname[50];
    char student_lname[50];
    char level[4];
    Details student_details;
    Course enrolled_courses[2]; // Assume each student can enroll in 2 courses for now
    float cgpa;
} Student;

typedef struct {
    int userID;
    int password;
    char usertype[2]; // 's' for student, 'p' for programme admin, 'l' for lecturer, 'y' for system admin.
} User;

typedef struct HolidayNode {
    char holidayName[MAX_LENGTH];
    char startDate[11];
    char endDate[11];
    struct HolidayNode *next;
} HolidayNode;

typedef struct FeedbackNode {
    int feedbackID;
    char date[MAX_DATE];
    int courseID;
    char feedback[MAX_FEEDBACK_LENGTH];
    char reply[MAX_FEEDBACK_LENGTH]; // To store lecturer's reply
    struct FeedbackNode* next;
} FeedbackNode;

FeedbackNode* feedbackHead = NULL;
User users[MAX_USERS];
Student students[MAX_STUDENTS];
Course courses[MAX_COURSES];
AttendanceNode attendance[MAX_STUDENTS];
const char grades[MAX_GRADES] = { 'A', 'B', 'C', 'D', 'F' };
HolidayNode *holidayList; 

//-----------------------------------------------------------------
// -- FUNCTION PROTOTYPES -----------------------------------------
// ----------------------------------------------------------------
void capitalize(char* str);
int checkDOB(int year, int month, int day);
void clearBuffer();
int isNumerical(const char* input);
int getID(const char* entity);
int createUsers();
int createStudents();
int createCourses();
int createAttendance();
int createGrades();
int createFeedback();
void registerUser();
void viewUsers();
void deleteUser();
char calculateGrade(float marks, float lowerBoundary[MAX_GRADES]);
int updateStudentGrades();
void modifyLowerBoundaries(const char grades[]);
void updateCGPAToFile(int studentIndex, float cgpa);
void calculateCGPA();
void generateStudentReport();
void viewCourses();
void viewLecturers();
int createLecturers();
void viewGrades();
int lecturerMenu();
int studentMenu(int loggedInUserId);
int programmeAdminMenu();
int programAdmin_viewMenu();
int programAdmin_manageMenu();
int systemAdminMenu();
void mainMenu();
void registerAndManageUsers();
void manageCourses();
void manageGrades();
void viewAndUpdateStudentAttendance();
void updateStudentMarksFile();
int attendanceRecordExists(int studentID, int courseID, const char* date, Course* course);
int isValidDate(const char* date);
void viewAndReplyFeedback();
HolidayNode* createHolidayList();
void printHolidayList(HolidayNode *head);
void freeHolidayList(HolidayNode *head);
int checkHolidayDate(int month, int day, char *date);
void createNewHoliday(HolidayNode **head);
void deleteHoliday(HolidayNode **head);
int holidayManagement(HolidayNode **holidayList);
void viewPersonalDetails(int loggedInUserId);
void updatePersonalDetails(int loggedInUserId);
void viewEnrolledCoursesAndGrades(int loggedInUserId);
void viewAttendance(int loggedInUserId);
void viewHolidayCalendar();
void freeFeedbackList();

// ----------------------------------------------------------------
// -- HARDCODED INITIAL DATA --------------------------------------
// ----------------------------------------------------------------
int createUsers() {
    FILE* fp;
    fp = fopen("users.txt", "w");
    if (fp == NULL) {
        printf("(!) Error opening users.txt.\n");
        return 1;
    }
    users[0] = (User){ 1001, 119, "s" };
    users[1] = (User){ 2001, 272, "p" };
    users[2] = (User){ 3001, 123, "l" };
    users[3] = (User){ 4001, 456, "y" };
    users[4] = (User){ 1002, 119, "s" };
    users[5] = (User){ 2002, 272, "p" };
    users[6] = (User){ 3002, 123, "l" };
    users[7] = (User){ 4004, 456, "y" };
    for (int i = 0; i < 8; i++) {
        fprintf(fp, "%d,%d,%s\n", users[i].userID, users[i].password, users[i].usertype);
    }

    fclose(fp);
    return 0;
}

int numStudents = 0; // Variable to track the number of ALL students -- intialise as 0
int createStudents() {
    // Student 1 details
    students[0].studentID = 1001;
    strcpy(students[0].student_fname, "Miyeon");
    strcpy(students[0].student_lname, "Cho");
    strcpy(students[0].level, "F");
    students[0].cgpa = 3.0;
    students[0].student_details = (Details){ 1234567890, "1997-01-31", "International", 'F' };
    // Student 1 courses
    students[0].enrolled_courses[0] = (Course){ 101, "C Programming", 3001, 80.00, 'A' };
    students[0].enrolled_courses[1] = (Course){ 102, "Java", 3002, 50.00, 'C' };

    // Student 2 details
    students[1].studentID = 1002;
    strcpy(students[1].student_fname, "Soyeon");
    strcpy(students[1].student_lname, "Jeon");
    strcpy(students[1].level, "D1");
    students[1].cgpa = 3.2;
    students[1].student_details = (Details){ 987654321, "1998-08-26", "Local", 'F' };
    // Student 2 courses
    students[1].enrolled_courses[0] = (Course){ 101, "C Programming", 3001, 15.00, 'F' };
    students[1].enrolled_courses[1] = (Course){ 102, "Java", 3002, 70.00, 'B' };

    // Write student details to "studentdetails.txt"
    FILE* fp;
    fp = fopen("studentdetails.txt", "w");
    if (fp == NULL) {
        printf("Error opening file.\n");
        return 1;
    }

    for (int i = 0; i < 2; i++) {
        fprintf(fp, "%d,%s,%s,%d,%s,%s,%c,%.2f,%s\n",
            students[i].studentID,
            students[i].student_fname,
            students[i].student_lname,
            students[i].student_details.phone_number,
            students[i].student_details.dob,
            students[i].student_details.nationality,
            students[i].student_details.gender,
            students[i].cgpa,
            students[i].level);
    }
    fclose(fp);

    // Write student marks to "studentmarks.txt" FILE
    fp = fopen("studentmarks.txt", "w");
    if (fp == NULL) {
        printf("Error opening file.\n");
        return 1;
    }

    for (int i = 0; i < 2; i++) {
        fprintf(fp, "%d,", students[i].studentID);
        for (int j = 0; j < 2; j++) {
            fprintf(fp, "%d,%.2f,", students[i].enrolled_courses[j].courseID, students[i].enrolled_courses[j].marks);
            fprintf(fp, "%c", students[i].enrolled_courses[j].grade);
            if (j < 1) {
                fprintf(fp, ",");
            }
        }
        fprintf(fp, "\n");
    }
    fclose(fp);

    numStudents += 2;
    return 0;
}

int numCourses = 0; // Variable to track the number of courses -- initialise as 0
int createCourses() {
    FILE* fp = fopen("courses.txt", "w");
    if (fp == NULL) {
        printf("Error opening file.\n");
        return 1;
    }

    // Example course creation
    Course course1 = (Course){ 101, "C Programming", 3001, 0.0, 'F', NULL };
    courses[numCourses++] = course1;
    Course course2 = (Course){ 102, "Java",  3002, 0.0, 'F', NULL };
    courses[numCourses++] = course2;

    for (int i = 0; i < 2; i++) {
        fprintf(fp, "%d,%s,%d\n",
            courses[i].courseID, courses[i].course_name, courses[i].lecturerID);
    }

    fclose(fp);
    return 0;
}

int createAttendance() {
    FILE* fp = fopen("attendance.txt", "w");
    if (fp == NULL) {
        printf("Error opening file.\n");
        return 1;
    }

    // Sample attendance data creation (for demonstration)
    for (int i = 0; i < 2; i++) {
        for (int j = 0; j < 2; j++) { // Assuming each student has 2 courses
            AttendanceNode* node = (AttendanceNode*)malloc(sizeof(AttendanceNode));
            if (node == NULL) {
                printf("Memory allocation failed\n");
                fclose(fp);
                return 1;
            }
            node->studentID = students[i].studentID;
            strcpy(node->date, "W1"); // Example date
            node->attendanceLab = 1;  // Example data - present
            node->attendanceLecture = 1; // Example data - present
            node->next = students[i].enrolled_courses[j].attendance_head; // Link to the current head
            students[i].enrolled_courses[j].attendance_head = node; // Set new node as head

            fprintf(fp, "%d,%d,%s,%d,%d\n",
                node->studentID,
                students[i].enrolled_courses[j].courseID,
                node->date,
                node->attendanceLab,
                node->attendanceLecture);
        }
    }
    fclose(fp);
    return 0;
}

int createGrades() {
    float lowerBoundary[MAX_GRADES] = { 75.00, 65.00, 50.00, 40.00, 0.00 };
    FILE* file = fopen("grades.txt", "w");
    if (file == NULL) {
        printf("Error opening file!\n");
        return 1;
    }
    for (int i = 0; i < MAX_GRADES; i++) {
        fprintf(file, "%.2f\n", lowerBoundary[i]);
    }
    fclose(file);
    return 0;
}

int createFeedback() {
    // Hardcoded feedback entries
    char* dates[] = {"2024-04-01", "2024-04-02", "2024-04-03", "2024-04-04"};
    int courseIDs[] = {101, 102, 101, 102};
    char* feedbackTexts[] = {"Great lecture!", "I dont understand the assignment", "hello","Best lecturer ever!"};
    int feedbackCount = sizeof(courseIDs) / sizeof(courseIDs[0]); // Calculate the number of feedback entries

    int feedbackID = feedbackCount; // Start assigning feedback IDs from the total count downwards

    for (int i = 0; i < feedbackCount; i++) { // Loop through each feedback entry
        FeedbackNode* newNode = (FeedbackNode*)malloc(sizeof(FeedbackNode)); // Allocate memory for the new node
        if (!newNode) {
            printf("Memory allocation failed.\n");
            continue;
        }
        newNode->feedbackID=i+1; // Assign feedback ID
        strcpy(newNode->date, dates[i]);
        newNode->courseID = courseIDs[i];
        strcpy(newNode->feedback, feedbackTexts[i]); 
        strcpy(newNode->reply, ""); // Initialize reply as empty
        newNode->next = feedbackHead; // Inserting new nodes at the head 
        feedbackHead = newNode; // Update the head to the new node
    }

    // write the linked list to the file
    FILE* fp = fopen("feedbacks.txt", "w");
    if (!fp) {
        printf("Error opening file for writing.\n");
        return 1;
    }
    
    FeedbackNode* current = feedbackHead;
    while (current != NULL) {
        fprintf(fp, "%d,%s,%d,\"%s\",\"%s\"\n",
                current->feedbackID, current->date, current->courseID, current->feedback, current->reply);
        current = current->next;
    }
    
    fclose(fp);
    return 0;
}

HolidayNode* createHolidayList() {
    // Define holidays
    char holidays[][MAX_HOLIDAYS] = {"Thaipusam", "Chinese New Year", "Hari Raya Puasa", "Labour Day"};
    char startDates[][11] = {"2024-01-25", "2024-02-10", "2024-04-08", "2024-05-01"};
    char endDates[][11] = {"2024-01-25", "2024-02-13", "2024-04-11", "2024-05-01"};

    HolidayNode *head = NULL;
    HolidayNode *current = NULL;

    // Create nodes for each holiday and link them together
    for (int i = 0; i < sizeof(holidays) / sizeof(holidays[0]); i++) {
        HolidayNode *newHoliday = (HolidayNode*)malloc(sizeof(HolidayNode));
        if (newHoliday == NULL) {
            // Memory allocation failed
            printf("Memory allocation failed for holiday node.\n");
            // Free the allocated memory before returning NULL
            while (head != NULL) {
                current = head;
                head = head->next;
                free(current);
            }
            return NULL;
        }
        // Copy holiday details to the new node
        strcpy(newHoliday->holidayName, holidays[i]);
        strcpy(newHoliday->startDate, startDates[i]);
        strcpy(newHoliday->endDate, endDates[i]);
        newHoliday->next = NULL;

        // Link the new node to the list
        if (head == NULL) {
            head = newHoliday;
            current = newHoliday;
        } else {
            current->next = newHoliday;
            current = current->next;
        }
    }
    return head;
}

// ----------------------------------------------------------------
// -- OTHER FUNCTIONS ---------------------------------------------
// ----------------------------------------------------------------
int isValidDate(const char* date) {
    int year, month, day;
    if (sscanf(date, "%d-%d-%d", &year, &month, &day) != 3)
        return 0;
    if (year != 2024) // Enter the year is 2024
        return 0;
    if (month < 1 || month > 12) // Check month range
        return 0;
    if (day < 1 || day > 31) // Check day range
        return 0;
    return 1; // The date is valid
}

void capitalize(char* str) {
    // Handle NULL or empty string cases
    if (str == NULL || str[0] == '\0') {
        return;
    }
    int capitalizeNext = 1; // Flag to indicate whether the next character should be capitalized
    for (int i = 0; str[i] != '\0'; i++) {
        char currentChar = str[i];

        if (isspace(currentChar)) {
            capitalizeNext = 1; // Set the flag to capitalize the next character
        }
        else if (capitalizeNext) {
            str[i] = toupper(currentChar); // Capitalize the current character
            capitalizeNext = 0; // Reset the flag
        }
        else {
            str[i] = tolower(currentChar); // Lowercase the current character
        }
    }
}

int checkDOB(int year, int month, int day) {
    // Validate year range
    if (year < 1970 || year > 2008)
        return 1;
    // Validate month range
    if (month < 1 || month > 12)
        return 1;
    // Validate day range based on month and leap year
    int daysInMonth;
    switch (month) {
    case 2:
        // Check for leap year
        if ((year % 4 == 0 && year % 100 != 0) || year % 400 == 0)
            daysInMonth = 29;
        else
            daysInMonth = 28;
        break;
    case 4:
    case 6:
    case 9:
    case 11:
        daysInMonth = 30;
        break;
    default:
        daysInMonth = 31;
        break;
    }
    // Validate day
    if (day < 1 || day > daysInMonth)
        return 1;
    return 0;
}

void clearBuffer() {
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
}

void freeFeedbackList() {
    FeedbackNode* current = feedbackHead;
    while (current != NULL) {
        FeedbackNode* nextNode = current->next;
        free(current);
        current = nextNode;
    }
    feedbackHead = NULL; // Ensure the head pointer is no longer pointing to freed memory
}
// Function to check if input is numerical
int isNumerical(const char* input) {
    if (input == NULL || *input == '\0') {
        return 0; // Empty string or NULL is not numerical
    }

    while (*input != '\0' && *input != '\n') { // Stop at newline character
        if (!isdigit(*input)) {
            return 0; // Non-digit character found
        }
        input++; // Move to the next character
    }
    return 1; // All characters are digits
}

int getID(const char* entity) {
    char input[MAX_LENGTH]; // Buffer for user input
    int value;
    int validInput = 0;
    do {
        printf("Enter %s ID: ", entity);
        fgets(input, sizeof(input), stdin); // Read user input as a string
        // Remove newline character if present
        if (input[strlen(input) - 1] == '\n') {
            input[strlen(input) - 1] = '\0';
        }
        // Check if input is numerical
        if (isNumerical(input)) {
            value = atoi(input); // Convert string to integer
            validInput = 1; // Set flag to exit loop
        }
        else {
            printf("(!) Please enter a numerical value.\n");
        }
    } while (!validInput); // Repeat until valid input is received
    return value;
}

int isValidWeek(const char* week) {
    int len = strlen(week);
    // Check if the string starts with 'W' and has at least 2 characters
    if (len < 2 || week[0] != 'W') {
        return 0;
    }
    // Convert the substring after 'W' to an integer
    int weekNumber = atoi(week + 1);
    // Check if the week number is between 1 and 15
    if (weekNumber >= 1 && weekNumber <= 15) {
        return 1;
    }
    return 0;
}

// ----------------------------------------------------------------
// ----------------- SYSTEM ADMINISTRATOR -------------------------
// ----------------------------------------------------------------

int studentUserIDs[MAX_STUDENTS]; // Array to store NEW student IDs
int numStudentUsers = 0; // Variable to track the number of NEW students

int lecturerUserIDs[MAX_LECTURERS]; // Array to store lecturer IDs
int numLecturerUsers = 0; // Variable to track the number of lecturer users

void registerUser() {
    int newUserID, numUsers, newPassword;
    char newUserType[2], tempPassword[4];

    // Get the number of users before adding a new user
    numUsers = 0;
    while (users[numUsers].userID != 0) {
        numUsers++;
    }

    do {
        do {
            printf("['s' for Student, 'p' for Program Admin, 'l' for Lecturer, 'y' for System Admin]\n");
            printf("Enter user type: ");
            scanf("%s", newUserType);
            newUserType[0] = tolower(newUserType[0]); // Convert to lowercase
        } while (newUserType[0] != 's' && newUserType[0] != 'p' && newUserType[0] != 'l' && newUserType[0] != 'y');

        // Validate user ID format based on user type
        do {
            printf("Enter new user ID: ");
            scanf("%d", &newUserID);
            getchar(); // Clear input buffer

            switch (newUserType[0]) {
            case 's':
                if (newUserID / 1000 != 1) {
                    printf("(!) Student user ID must start with 1.\n");
                    newUserID = 0; // Reset invalid ID
                }
                break;
            case 'p':
                if (newUserID / 1000 != 2) {
                    printf("(!) Program Admin user ID must start with 2.\n");
                    newUserID = 0; // Reset invalid ID
                }
                break;
            case 'l':
                if (newUserID / 1000 != 3) {
                    printf("(!) Lecturer user ID must start with 3.\n");
                    newUserID = 0; // Reset invalid ID
                }
                break;
            case 'y':
                if (newUserID / 1000 != 4) {
                    printf("(!) System Admin user ID must start with 4.\n");
                    newUserID = 0; // Reset invalid ID
                }
                break;
            }
        } while (newUserID == 0);

        int valid = 1;
        for (int i = 0; i < numUsers; i++) {
            if (users[i].userID == newUserID) {
                valid = 0;
                printf("(!) User ID already exists.\n");
                break;
            }
        }

        if (valid) {
            break; // Break out of loop if user ID is valid
        }
    } while (1);

    do {
        printf("Enter new password (3 numbers): ");
        scanf("%s", tempPassword);
    } while (strlen(tempPassword) != 3 || !isdigit(tempPassword[0]) || !isdigit(tempPassword[1]) || !isdigit(tempPassword[2]) ||
             tempPassword[3] != '\0'); // Check if the password consists of 3 digits

    newPassword = atoi(tempPassword); // Convert string to integer

    FILE* fp = fopen("users.txt", "a"); // Open file in append mode to add new user
    if (fp == NULL) {
        printf("Error opening file.\n");
        return;
    }
    fprintf(fp, "%d,%d,%s\n", newUserID, newPassword, newUserType);
    fclose(fp);

    printf("(*) New user registered successfully.\n");

    // Store the new user in the users array
    users[numUsers].userID = newUserID;
    users[numUsers].password = newPassword;
    users[numUsers].usertype[0] = newUserType[0];
    users[numUsers].usertype[1] = '\0'; // Null-terminate the string

    // If the user type is 's', store the new userID in the studentUserIDs array
    if (newUserType[0] == 's') {
        if (numStudentUsers < MAX_STUDENTS) {
            studentUserIDs[numStudentUsers] = newUserID;
            numStudentUsers++;
        } else {
            printf("(!) Maximum number of student users reached.\n");
        }
    }

    // If the user type is 'l', store the new userID in the lecturerUserIDs array
    if (newUserType[0] == 'l') {
        if (numLecturerUsers < MAX_LECTURERS) {
            lecturerUserIDs[numLecturerUsers] = newUserID;
            numLecturerUsers++;
        } else {
            printf("(!) Maximum number of lecturer users reached.\n");
        }
    }
}

void viewUsers() {
    // Calculate the number of users
    int numUsers = sizeof(users) / sizeof(users[0]);

    printf("User List:\n");
    printf("+---------+----------+-----------+\n");
    printf("| User ID | Password | User Type |\n");
    printf("+---------+----------+-----------+\n");
    for (int i = 0; i < numUsers; i++) {
        if (users[i].userID != 0) {
            printf("| %-7d | %-8d | %-9s |\n", users[i].userID, users[i].password, users[i].usertype);
        }
    }
    printf("+---------+----------+-----------+\n");
}

void modifyUser() {
    int userID, numUsers;
    int found = 0;
    numUsers = 0; // Get the number of users before adding a new user
    while (users[numUsers].userID != 0) {
        numUsers++;
    }

    printf("List of existing users:\n"); // Display a list of existing users
    for (int i = 0; i < numUsers; i++) {
        printf("User ID: %d\n", users[i].userID);
        // Display other user details if needed
    }

    printf("Enter the user ID to modify: "); // Prompt the user to choose a user ID to modify
    scanf("%d", &userID);

    // Search for the selected user ID in the array
    for (int i = 0; i < numUsers; i++) {
        if (users[i].userID == userID) {
            found = 1;
            printf("User ID: %d\n", users[i].userID);
            printf("Modifying password...\n");
            char newPassword[MAX_LENGTH]; // Buffer to store user input for the new password
            do {
                printf("Enter new password: ");
                scanf("%s", newPassword);
                if (!isNumerical(newPassword)) {
                    printf("Invalid input. Password must consist of numerical characters.\n");
                }
            } while (!isNumerical(newPassword)); // Continue prompting until a valid numerical password is entered

            users[i].password = atoi(newPassword); // Convert the numerical password from string to integer
            printf("Password modified successfully.\n");
            break; // Exit loop after modifying the user
        }
    }

    // If the selected user ID is not found
    if (!found) {
        printf("User ID not found.\n");
    }
}

void deleteUser() {
    int inputUserID;
    printf("Enter User ID to delete: ");
    scanf("%d", &inputUserID);
    int found = 0;
    int numUsers;

    // Get the number of users before adding a new user
    numUsers = 0;
    while (users[numUsers].userID != 0) {
        numUsers++;
    }

    // Open users.txt for writing
    FILE* fp = fopen("users.txt", "w");
    if (fp == NULL) {
        printf("Error opening users.txt for writing.\n");
        return;
    }

    // Write users back to the file excluding the deleted user
    for (int i = 0; i < numUsers; i++) {
        if (users[i].userID == inputUserID) {
            found = 1;
            if (users[i].usertype[0] == 'l') {
                printf("Lecturers cannot be deleted.\n");
            } else {
                // Remove the user from the users array by shifting the remaining elements
                for (int j = i; j < numUsers - 1; j++) {
                    users[j] = users[j + 1];
                }
                memset(&users[numUsers - 1], 0, sizeof(User)); // Clear the last element
                printf("User deleted successfully.\n");
            }
        }
        // Write the users back to the file
        fprintf(fp, "%d,%d,%s\n", users[i].userID, users[i].password, users[i].usertype);
    }

    fclose(fp);

    if (!found) {
        printf("User with ID %d not found. Exiting.\n", inputUserID);
    }
}

int systemAdminSubmenu() {
    char choice;
    do {
        printf("\nManage Users\n");
        printf("1. Register User\n");
        printf("2. View Users\n");
        printf("3. Modify User Password\n");
        printf("4. Delete User\n");
        printf("5. Back\n");
        printf("Enter your choice: ");
        scanf(" %c", &choice);

        switch (choice) {
        case '1': registerUser(); break;
        case '2': viewUsers(); break;
        case '3': modifyUser(); break;
        case '4': deleteUser(); break;
        case '5': return 0;
        default: printf("Please enter a number between 1 and 5.\n");
        }
    } while (1);

    return 0;
}

// Function to update a student's grades
char calculateGrade(float marks, float lowerBoundary[MAX_GRADES]) {
    const char grades[MAX_GRADES] = { 'A', 'B', 'C', 'D', 'F' };
    for (int k = 0; k < MAX_GRADES; k++) {
        if (marks >= lowerBoundary[k]) {
            return grades[k];
        }
    }
    return grades[MAX_GRADES - 1]; // Return 'F' if no boundary is satisfied
}

int updateStudentGrades() {
    // Read lower boundaries from grades.txt
    float lowerBoundary[MAX_GRADES];
    FILE* gradesFile = fopen("grades.txt", "r");
    if (gradesFile == NULL) {
        printf("Error opening grades file!\n");
        return 1;
    }

    for (int i = 0; i < MAX_GRADES; i++) {
        if (fscanf(gradesFile, "%f", &lowerBoundary[i]) != 1) {
            printf("Error reading from grades file!\n");
            fclose(gradesFile);
            return 1;
        }
    }
    fclose(gradesFile);

    // Read student marks from studentmarks.txt and update grades
    FILE* marksFile = fopen("studentmarks.txt", "r");
    if (marksFile == NULL) {
        printf("Error opening marks file!\n");
        return 1;
    }

    int studentCount = 0;

    while (studentCount < MAX_STUDENTS &&
        fscanf(marksFile, "%d,%d,%f,%c,%d,%f,%c",
            &students[studentCount].studentID,
            &students[studentCount].enrolled_courses[0].courseID,
            &students[studentCount].enrolled_courses[0].marks,
            &students[studentCount].enrolled_courses[0].grade,
            &students[studentCount].enrolled_courses[1].courseID,
            &students[studentCount].enrolled_courses[1].marks,
            &students[studentCount].enrolled_courses[1].grade) == 7) {

        // Update grades based on lower boundaries
        for (int j = 0; j < 2; j++) {
            char grade = calculateGrade(students[studentCount].enrolled_courses[j].marks, lowerBoundary);
            students[studentCount].enrolled_courses[j].grade = grade;
        }

        studentCount++;
    }

    fclose(marksFile);

    // Rewrite updated marks to studentmarks.txt
    marksFile = fopen("studentmarks.txt", "w");
    if (marksFile == NULL) {
        printf("Error opening marks file for writing!\n");
        return 1;
    }

    for (int i = 0; i < studentCount; i++) {
        fprintf(marksFile, "%d,%d,%.2f,%c,%d,%.2f,%c\n",
            students[i].studentID,
            students[i].enrolled_courses[0].courseID,
            students[i].enrolled_courses[0].marks,
            students[i].enrolled_courses[0].grade,
            students[i].enrolled_courses[1].courseID,
            students[i].enrolled_courses[1].marks,
            students[i].enrolled_courses[1].grade);
    }

    fclose(marksFile);
    printf("Entries in STUDENTMARKS.TXT updated successfully.\n");

    // Update the grades directly in the students array
    for (int i = 0; i < studentCount; i++) {
        for (int j = 0; j < 2; j++) {
            students[i].enrolled_courses[j].grade = calculateGrade(students[i].enrolled_courses[j].marks, lowerBoundary);
        }
    }

    return 0;
}

// Once grading system is redefined, studentmarks.txt is automatically updated
void modifyLowerBoundaries(const char grades[]) {
    float lowerBoundary[MAX_GRADES];

    // Read lower boundaries from grades.txt
    FILE* file = fopen("grades.txt", "r");
    if (file == NULL) {
        printf("Error opening grades.txt.\n");
        return;
    }

    for (int i = 0; i < MAX_GRADES; i++) {
        if (fscanf(file, "%f", &lowerBoundary[i]) != 1) {
            printf("Error reading grades.txt.\n");
            fclose(file);
            return;
        }
    }

    fclose(file);

    printf("Enter new lower boundaries for each grade:\n");

    for (int i = 0; i < MAX_GRADES; i++) {
        float newBoundary;
        printf("Grade %c (current boundary: %.2f): ", grades[i], lowerBoundary[i]);
        scanf("%f", &newBoundary);

        if (newBoundary >= 0.0 && newBoundary <= 100.0) {
            lowerBoundary[i] = newBoundary;
        }
        else {
            printf("Invalid input! Lower boundary must be between 0 and 100.\n");
            i--; // Decrement i to re-prompt for the same grade
        }
    }

    // Write updated lower boundaries to grades.txt
    file = fopen("grades.txt", "w");
    if (file == NULL) {
        printf("Error opening grades.txt for writing!\n");
        return;
    }

    for (int i = 0; i < MAX_GRADES; i++) {
        fprintf(file, "%.2f\n", lowerBoundary[i]);
    }

    fclose(file);
    printf("Lower boundaries updated in grades.txt\n");

    if (updateStudentGrades() != 0) {
        printf("Error updating STUDENTMARKS.TXT.\n");
    }
}

void updateCGPAToFile(int studentIndex, float cgpa) {
    FILE* file = fopen("studentdetails.txt", "w");
    if (file == NULL) {
        printf("Error opening file for writing.\n");
        return;
    }

    // Write student details including CGPA to file
    for (int i = 0; i < studentIndex; i++) {
        fprintf(file, "%d,%s,%s,%d,%s,%s,%c,%.2f,%s\n",
            students[i].studentID,
            students[i].student_fname,
            students[i].student_lname,
            students[i].student_details.phone_number,
            students[i].student_details.dob,
            students[i].student_details.nationality,
            students[i].student_details.gender,
            students[i].cgpa,
            students[i].level);
    }

    // Write the current student's details including the new CGPA
    fprintf(file, "%d,%s,%s,%d,%s,%s,%c,%.2f,%s\n",
        students[studentIndex].studentID,
        students[studentIndex].student_fname,
        students[studentIndex].student_lname,
        students[studentIndex].student_details.phone_number,
        students[studentIndex].student_details.dob,
        students[studentIndex].student_details.nationality,
        students[studentIndex].student_details.gender,
        cgpa,
        students[studentIndex].level);

    fclose(file);
}

void calculateCGPA() {
    int studentCount = 0;

    while (studentCount < numStudents) {
        // Calculate CGPA for the current student
        float totalGradePoints = 0.0;

        for (int j = 0; j < 2; j++) {
            // Calculate grade points based on grades
            float gradePoint;
            switch (students[studentCount].enrolled_courses[j].grade) {
            case 'A': gradePoint = 4.0; break;
            case 'B': gradePoint = 3.0; break;
            case 'C': gradePoint = 2.0; break;
            case 'D':  gradePoint = 1.7; break;
            case 'F': gradePoint = 0.0; break;
            default: gradePoint = 0.0; break; // Handle invalid grades
            }

            // Update total grade points and credits
            totalGradePoints += gradePoint;
        }

        // Calculate CGPA for the current student
        float cgpa = totalGradePoints / 2;
        students[studentCount].cgpa = cgpa; // Update CGPA in the students array
        updateCGPAToFile(studentCount, cgpa); // Update CGPA to file

        studentCount++;
    }
}

void generateStudentReport() {
    for (int i = 0; i < MAX_STUDENTS; i++) {
        if (students[i].studentID == 0) {
            break; // Exit loop if no more students
        }
        printf("=== Student Report ===\n");
        printf("Student ID: %d\n", students[i].studentID);
        printf("Name: %s %s\n", students[i].student_fname, students[i].student_lname);
        printf("Level: %s\n", students[i].level);
        printf("CGPA: %.2f\n", students[i].cgpa);
        printf("Nationality: %s\n", students[i].student_details.nationality);
        printf("Gender: %c\n", students[i].student_details.gender);

        printf("+ Enrolled Courses:\n");
        for (int j = 0; j < 2; j++) {
            printf("+++++++++++++++++++++++\n");
            printf("+ Course ID: %d\n", students[i].enrolled_courses[j].courseID);
            printf("+ Course Name: %s\n", students[i].enrolled_courses[j].course_name);
            printf("+ Marks: %.2f\n", students[i].enrolled_courses[j].marks);
            printf("+ Grade: %c\n", students[i].enrolled_courses[j].grade);
            printf("+++++++++++++++++++++++\n");
        }

        printf("\n");
    }
}

// ----------------------------------------------------------------
// ----------------- PROGRAMME ADMINISTRATOR ----------------------
// ----------------------------------------------------------------

// References Course courses[MAX_COURSES]
void viewCourses() {
    printf("\n");
    printf("%-20s%-26s%-10s\n", "Course ID", "Course Name", "Lecturer ID");
    for (int i = 0; i < numCourses; i++) {
        printf("%-20d%-26s%-10d\n", courses[i].courseID, courses[i].course_name, courses[i].lecturerID);
    }
}

// References lecturerUserIDs[MAX_LECTURERS]
void viewLecturers() {
    FILE* file = fopen("users.txt", "r"); // Read data from users.txt
    if (file == NULL) {
        printf("Error opening file.\n");
        return;
    }

    numLecturerUsers = 0;
    int userID, password;
    char usertype;
    while (fscanf(file, "%d,%d,%c", &userID, &password, &usertype) == 3) {
        if (usertype == 'l') {
            lecturerUserIDs[numLecturerUsers] = userID;
            numLecturerUsers++;
            if (numLecturerUsers >= MAX_LECTURERS) {
                printf("Maximum number of lecturer users reached.\n");
                break;
            }
        }
    }

    fclose(file);

    // Display the IDs
    printf("Lecturer IDs:\n");
    for (int i = 0; i < numLecturerUsers; i++) {
        printf("%d\n", lecturerUserIDs[i]);
    }
}

// Function to validate lecturer ID
int validateLecturerID(int lecturerID) {
    for (int i = 0; i < numLecturerUsers; i++) {
        if (lecturerUserIDs[i] == lecturerID) {
            return 1; // Lecturer ID exists
        }
    }
    return 0; // Lecturer ID not found
}

// Function to validate course ID
int validateCourseID(int courseID) {
    for (int i = 0; i < numCourses; i++) {
        if (courses[i].courseID == courseID) {
            return 1; // Course ID exists
        }
    }
    return 0; // Course ID not found
}

// Update course and reassign lecturer
int updateCourse() {
    int courseID;
    int courseIndex;

    do {
        viewCourses(); // Display the current courses
        clearBuffer();
        printf("\n**ENTER 0 TO CANCEL**\n");
        courseID = getID("Course");
        if (courseID == 0) {
            return 0; // Cancel operation
        }

        // Find the index of the course with the given course ID
        courseIndex = -1;
        for (int i = 0; i < numCourses; i++) {
            if (courses[i].courseID == courseID) {
                courseIndex = i;
                break;
            }
        }

        if (courseIndex == -1) {
            printf("(!) Course not found.\n");
        }
    } while (courseIndex == -1); // Repeat until a valid course ID is entered

    // Change course name if the user doesn't press 0
    printf("\n**ENTER 0 TO SKIP**\n");
    printf("Enter course name: ");
    char tempName[MAX_LENGTH];
    scanf(" %[^\n]", tempName);
    clearBuffer(); // Clear the buffer
    if (strcmp(tempName, "0") != 0) {
        strcpy(courses[courseIndex].course_name, tempName);
        capitalize(courses[courseIndex].course_name); // Capitalize the course name
    }

    // Reassign the lecturer
    int lecturerID;
    do {
        viewLecturers(); // Display the current lecturer IDs
        lecturerID = getID("Lecturer");
        if (!validateLecturerID(lecturerID)) {
            printf("(!) Invalid lecturer ID.\n");
        }
    } while (!validateLecturerID(lecturerID));

    courses[courseIndex].lecturerID = lecturerID;

    // Write the updated course information to courses.txt
    FILE* file = fopen("courses.txt", "w");
    if (file == NULL) {
        printf("Error opening courses file.\n");
        return 1;
    }

    for (int i = 0; i < numCourses; i++) {
        fprintf(file, "%d,%s,%d\n", courses[i].courseID, courses[i].course_name, courses[i].lecturerID);
    }

    fclose(file);

    printf("(*) Course updated successfully.\n");

    // Update the enrolled courses for each student
    for (int i = 0; i < numStudents; i++) {
        for (int j = 0; j < 2; j++) {
            if (students[i].enrolled_courses[j].courseID == courseID) {
                strcpy(students[i].enrolled_courses[j].course_name, courses[courseIndex].course_name);
                students[i].enrolled_courses[j].lecturerID = courses[courseIndex].lecturerID;
                break; // Exit the loop once the course is updated for the student
            }
        }
    }
    return 0;
}

int addCourse() {
    // Prompt new course ID
    int courseID;
    do {
        viewCourses();
        clearBuffer();
        printf("\n**ENTER 0 TO CANCEL**\n");
        courseID = getID("Course");
        if (courseID == 0) {
            return 0;
        }
        if (validateCourseID(courseID)) {
            printf("(!) Course ID already exists.\n");
        } else if (courseID < 100 || courseID > 999) {
        printf("(!) Course ID must be a 3-digit number.\n");
        }
    } while (validateCourseID(courseID) || (courseID < 100 || courseID > 999));

    // Prompt new course name
    char courseName[MAX_LENGTH];
    printf("Enter Course Name: ");
    scanf(" %[^\n]", courseName);
    capitalize(courseName); // Capitalize the course name
    clearBuffer(); // Clear the buffer

    // Prompt for Lecturer ID
    int lecturerID;
    do {
        viewLecturers(); // Display the current lecturer IDs
        lecturerID = getID("Lecturer");
        if (!validateLecturerID(lecturerID)) {
            printf("(!) Invalid lecturer ID.\n");
        }
    } while (!validateLecturerID(lecturerID));

    // Append the new course information to courses.txt
    FILE* file = fopen("courses.txt", "a");
    if (file == NULL) {
        printf("Error opening courses file.\n");
        return 1;
    }

    fprintf(file, "%d,%s,%d\n", courseID, courseName, lecturerID);
    fclose(file);

    printf("(*)Course added successfully.\n");

    // Update the courses array
    float marks = 0.0;
    char grade = 'F';
    AttendanceNode* attendance_head = NULL;

    courses[numCourses].courseID = courseID;
    strcpy(courses[numCourses].course_name, courseName);
    courses[numCourses].lecturerID = lecturerID;
    courses[numCourses].marks = marks;
    courses[numCourses].grade = grade;
    courses[numCourses].attendance_head = attendance_head;
    numCourses++; // Increment the number of courses

    return 0;
}

// Enrol new students 
void enrolStudents() {
    printf("Unenrolled Students:\n");
    if (numStudentUsers == 0) {
        printf("(*)No pending enrollments.\n");
    }
    else {
        for (int i = 0; i < numStudentUsers; i++) {
            printf("%d\n", studentUserIDs[i]);
        }
    }
    printf("\n");

    FILE* file;

    file = fopen("studentdetails.txt", "a");
    if (file == NULL) {
        printf("(!) Error opening studentdetails.txt for appending.\n");
        return;
    }

    // Prompt for personal details
    for (int i = 0; i < numStudentUsers; i++) {
        int studentID = studentUserIDs[i];
        printf("Enter details for student with ID %d:\n", studentID);

        // Create a new Student object
        Student newStudent;

        newStudent.studentID = studentID; // Populate the Student object with input data

        printf("First Name: ");
        char fname[MAX_LENGTH];
        scanf("%[^\n]", fname);
        capitalize(fname);
        strcpy(newStudent.student_fname, fname);


        printf("Last Name: ");
        char lname[MAX_LENGTH];
        clearBuffer();
        scanf("%[^\n]", lname);
        capitalize(lname);
        strcpy(newStudent.student_lname, lname);

        printf("Phone number: ");
        clearBuffer();
        scanf("%d", &newStudent.student_details.phone_number);

        int year, month, day, validDOB;
        do {
            printf("Date of birth [YYYY MM DD]: ");
            clearBuffer();
            scanf("%d %d %d", &year, &month, &day);

            validDOB = checkDOB(year, month, day);
            if (validDOB == 1) {
                printf("(!) Date of birth is not valid.\n");
            }
        } while (validDOB != 0);

        char dob[11];
        sprintf(dob, "%d-%d-%d", year, month, day);
        sprintf(newStudent.student_details.dob, "%s", dob);

        char nationality[30];
        do {
            printf("Nationality [Local/International]: ");
            clearBuffer();
            scanf("%[^\n]", nationality);
            capitalize(nationality);

            if (strcmp(nationality, "Local") != 0 && strcmp(nationality, "International") != 0) {
                printf("(!) Invalid nationality.\n");
            }

        } while (strcmp(nationality, "Local") != 0 && strcmp(nationality, "International") != 0);
        strcpy(newStudent.student_details.nationality, nationality);

        char gender[MAX_LENGTH];
        int validGender;

        do {
            printf("Gender [Male/Female]: ");
            clearBuffer();
            scanf("%s", gender);
            capitalize(gender);

            // Validate gender
            if (strcmp(gender, "Male") == 0 || strcmp(gender, "Female") == 0) {
                validGender = 1;
            }
            else {
                printf("(!) Invalid gender.\n");
                validGender = 0;
            }

        } while (!validGender);

        if (strcmp(gender, "Male") == 0) {
            newStudent.student_details.gender = 'M';
        }
        else if (strcmp(gender, "Female") == 0) {
            newStudent.student_details.gender = 'F';
        }

        char level[MAX_LENGTH];
        int validLevel;
        do {
            printf("Enter level of study [F/D1/D2/D3/M]: ");
            clearBuffer();
            scanf("%[^\n]", level);
            capitalize(level);

            if (strcmp(level, "F") == 0 || strcmp(level, "D1") == 0 || strcmp(level, "D2") == 0 || strcmp(level, "D3") == 0 || strcmp(level, "M") == 0) {
                validLevel = 1;
            }
            else {
                printf("(!) Invalid level.\n");
                validLevel = 0;
            }

        } while (!validLevel);
        strcpy(newStudent.level, level);

        newStudent.cgpa = 0.00; // Initialize CGPA to 0.00

        // Enroll the student into courses
        for (int j = 0; j < 2; j++) {
            int courseIndex = -1;
            Course course;
            do {
                printf("Enter course ID #%d for student with ID %d: ", j + 1, studentID);
                int courseID;
                scanf("%d", &courseID);
                clearBuffer(); // Clear input buffer

                // Check if the student is already enrolled in the course
                int alreadyEnrolled = 0;
                for (int k = 0; k < j; k++) {
                    if (newStudent.enrolled_courses[k].courseID == courseID) {
                        alreadyEnrolled = 1;
                        printf("(!) Student is already enrolled in this course.\n");
                        break;
                    }
                }

                if (alreadyEnrolled) {
                    continue; // Skip to the next iteration of the loop
                }

                // Find the course details from the courses array
                for (int k = 0; k < numCourses; k++) {
                    if (courses[k].courseID == courseID) {
                        course = courses[k];
                        courseIndex = k;
                        break;
                    }
                }

                if (courseIndex == -1) {
                    printf("(!) Course not found.\n");
                }
            } while (courseIndex == -1); // Repeat until a valid course ID is entered

            // Add the course to the student's enrolled courses
            newStudent.enrolled_courses[j] = course;
        }

        // Write student details to file
        fprintf(file, "%d,%s,%s,%d,%s,%s,%c,%.2f,%s\n",
            newStudent.studentID,
            newStudent.student_fname,
            newStudent.student_lname,
            newStudent.student_details.phone_number,
            newStudent.student_details.dob,
            newStudent.student_details.nationality,
            newStudent.student_details.gender,
            newStudent.cgpa,
            newStudent.level);

        fclose(file);

        printf("(*) Student with ID %d enrolled successfully.\n", studentID);

        // Add the new student to the students array
        students[numStudents++] = newStudent;

        // Write student enrollment details to studentmarks.txt
        file = fopen("studentmarks.txt", "a");
        if (file == NULL) {
            printf("(!) Error opening studentmarks.txt for appending.\n");
            return;
        }

        fprintf(file, "%d", newStudent.studentID); // Write new student's ID to file
        for (int j = 0; j < 2; j++) {
            fprintf(file, ",%d,%.2f,%c",
                newStudent.enrolled_courses[j].courseID,
                newStudent.enrolled_courses[j].marks,
                newStudent.enrolled_courses[j].grade);
        }
        fprintf(file, "\n");

        fclose(file);

        // Find the index of the user in the studentUserIDs array
        int indexToRemove = -1;
        for (int i = 0; i < numStudentUsers; i++) {
            if (studentUserIDs[i] == studentID) {
                indexToRemove = i;
                break;
            }
        }

        // Remove the user by shifting elements in the array
        if (indexToRemove != -1) {
            for (int i = indexToRemove; i < numStudentUsers - 1; i++) {
                studentUserIDs[i] = studentUserIDs[i + 1];
            }
            numStudentUsers--; // Decrease the count of student users
            printf("Unenrolled Students: %d\n", numStudentUsers);
            printf("Enrolled Students: %d\n", numStudents);
        }
        else {
            printf("(!) User not found in studentUserIDs array.\n"); // In case of error
        }
    }
}

void viewEnrolledStudents() {
    printf("%-15s%-15s%-15s%-10s%-10s%-15s\n", "Student ID", "First Name", "Last Name", "Level", "Course 1", "Course 2");

    for (int i = 0; i < numStudents; i++) {
        Student currentStudent = students[i];
        printf("%-15d%-15.15s%-15.15s%-15.4s",
            currentStudent.studentID,
            currentStudent.student_fname,
            currentStudent.student_lname,
            currentStudent.level);

        if (currentStudent.enrolled_courses[0].courseID != 0) {
            printf("%-10d", currentStudent.enrolled_courses[0].courseID);
        }
        else {
            printf("%-10s", "N/A");
        }

        if (currentStudent.enrolled_courses[1].courseID != 0) {
            printf("%-10d\n", currentStudent.enrolled_courses[1].courseID);
        }
        else {
            printf("%-10s\n", "N/A");
        }

    }
}

int programAdmin_viewMenu() {
    char choice;
    do {
        printf("\n+------------View-----------+\n");
        printf("1. Current Courses\n");
        printf("2. All Students\n");
        printf("3. All Lecturers\n");
        printf("4. Back\n");
        printf("+---------------------------+\n");
        printf("Enter your choice: ");
        scanf(" %c", &choice);
        printf("+---------------------------+\n");

        switch (choice) {
        case '1': viewCourses(); break;
        case '2': viewEnrolledStudents(); break;
        case '3': viewLecturers(); break;
        case '4': return 0;
        default: printf("(!)Please enter a number between 1 and 4.\n");
        }
    } while (1);

    return 0;
}

int programAdmin_manageMenu() {
    char choice;
    do {
        printf("\n+----------Manage Courses--------+\n");
        printf("1. Add a New Course\n");
        printf("2. Update Current Course\n");
        printf("3. Back\n");
        printf("+--------------------------------+\n");
        printf("Enter your choice: ");
        scanf(" %c", &choice);
        printf("+--------------------------------+\n");

        switch (choice) {
        case '1':
            addCourse();
            printf("\nUPDATED COURSES\n");
            viewCourses();
            break;
        case '2':
            updateCourse();
            printf("\nUPDATED COURSES\n");
            viewCourses();
            break;
        case '3':
            return 0;
        default:
            printf("(!)Please enter a number between 1 and 4.\n");
        }
    } while (1);

    return 0;
}

int unenrolStudent() {
    int studentID;
    printf("\n**ENTER 0 TO CANCEL**\n");
    studentID = getID("Student");
    if (studentID == 0) {
        return 0;
    }

    int found = 0;
    int index = -1;

    // Find the index of the student in the students array
    for (int i = 0; i < numStudents; i++) {
        if (students[i].studentID == studentID) {
            found = 1;
            index = i;
            break;
        }
    }

    if (found) {
        // Shift the elements in the array to remove the student
        for (int i = index; i < numStudents - 1; i++) {
            students[i] = students[i + 1];
        }

        // Clear data of the last element
        memset(&students[numStudents - 1], 0, sizeof(Student));

        numStudents--; // Decrease the number of students

        // Update student details file after unenrollment
        FILE* file;
        file = fopen("studentdetails.txt", "w");
        if (file == NULL) {
            printf("Error opening studentdetails.txt for writing.\n");
            return 1;
        }

        // Write the student information to the file
        for (int i = 0; i < numStudents; i++) {
            fprintf(file, "%d,%s,%s,%d,%s,%s,%c,%.2f,%s\n",
                students[i].studentID,
                students[i].student_fname,
                students[i].student_lname,
                students[i].student_details.phone_number,
                students[i].student_details.dob,
                students[i].student_details.nationality,
                students[i].student_details.gender,
                students[i].cgpa,
                students[i].level);
        }

        fclose(file);

        // Update student marks file after unenrollment
        file = fopen("studentmarks.txt", "r");
        if (file == NULL) {
            printf("Error opening studentmarks.txt for reading.\n");
            return 1;
        }

        FILE* tempMarksFile = fopen("tempmarks.txt", "w");
        if (tempMarksFile == NULL) {
            printf("Error creating temporary file.\n");
            fclose(file);
            return 1;
        }

        char buffer[MAX_LENGTH];
        while (fgets(buffer, MAX_LENGTH, file) != NULL) {
            int currentStudentID;
            sscanf(buffer, "%d", &currentStudentID); // Read student ID from the buffer
            if (currentStudentID != studentID) {
                fputs(buffer, tempMarksFile);
            }
        }

        fclose(file);
        fclose(tempMarksFile);

        remove("studentmarks.txt");
        rename("tempmarks.txt", "studentmarks.txt");

        printf("(*)Student with ID %d unenrolled successfully.\n", studentID);
    }
    else {
        printf("(!)Student with ID %d not found.\n", studentID);
    }
    return 0;
}

void printHolidayList(HolidayNode *head) {
    // Count the number of holidays
    int count = 0;
    HolidayNode *temp = head;
    while (temp != NULL) {
        count++;
        temp = temp->next;
    }

    // Create an array of HolidayNode pointers to store the holidays
    HolidayNode **holidayArray = (HolidayNode**)malloc(count * sizeof(HolidayNode*));
    if (holidayArray == NULL) {
        printf("Memory allocation failed for holiday array.\n");
        return;
    }

    // Store the holidays in the array
    temp = head;
    int i = 0;
    while (temp != NULL) {
        holidayArray[i++] = temp;
        temp = temp->next;
    }

    // Sort the holiday array based on start dates
    for (int i = 0; i < count - 1; i++) {
        for (int j = 0; j < count - i - 1; j++) {
            if (strcmp(holidayArray[j]->startDate, holidayArray[j + 1]->startDate) > 0) {
                HolidayNode *temp = holidayArray[j];
                holidayArray[j] = holidayArray[j + 1];
                holidayArray[j + 1] = temp;
            }
        }
    }

    // Print the sorted holiday list
    printf("\t--------------------------------------------------------------\n");
    printf("\t|  %-30s|  %-11s|  %-11s|\n", "Holiday", "Start Date", "End Date");
    printf("\t--------------------------------------------------------------\n");
    for (int i = 0; i < count; i++) {
        printf("\t|  %-30s|  %-11s|  %-11s|\n", holidayArray[i]->holidayName, holidayArray[i]->startDate, holidayArray[i]->endDate);
    }
    printf("\t--------------------------------------------------------------\n");

    // Free the allocated memory for the holiday array
    free(holidayArray);

    // Create holidays.txt and write the holiday list to holidays.txt
    FILE *fp = fopen("holidays.txt", "w");
    if (fp == NULL) {
        printf("Failed to open holidays.txt for writing.\n");
        return;
    }

    temp = head;
    while (temp != NULL) {
        fprintf(fp, "%s,%s,%s\n", temp->holidayName, temp->startDate, temp->endDate);
        temp = temp->next;
    }

    fclose(fp);
}

void freeHolidayList(HolidayNode *head) {
    HolidayNode *temp;
    while (head != NULL) {
        temp = head;
        head = head->next;
        free(temp);
    }
}

int checkHolidayDate(int month, int day, char *date) {
    // *date is a buffer to hold MM-DD string
    // Validate month range
    if (month < 1 || month > 12)
        return 0;

    // Validate day range based on month and 2024 calendar
    switch (month) {
        case 2: // February
            if (day < 1 || day > 29)
                return 0;
            break;
        case 4: // April
        case 6: // June
        case 9: // September
        case 11: // November
            if (day < 1 || day > 30)
                return 0;
            break;
        default: // Other months
            if (day < 1 || day > 31)
                return 0;
            break;
    }

    // If the date is valid, combine month and day into a string MM-DD
    sprintf(date, "%02d-%02d", month, day);

    return 1;
}

void createNewHoliday(HolidayNode **head) {
    HolidayNode *newHoliday = (HolidayNode*)malloc(sizeof(HolidayNode));
    if (newHoliday == NULL) {
        printf("Memory allocation failed for new holiday.\n");
        return;
    }

    char newHolidayName[MAX_LENGTH];
    printf("Enter the name of the new holiday: ");
    scanf(" %[^\n]", newHolidayName); // Notice the space before %[^\n] to consume leading whitespace
    capitalize(newHolidayName);
    strcpy(newHoliday->holidayName, newHolidayName); 
    clearBuffer();

    int startYear = 2024, endYear = 2024;
    int startMonth, startDay, endMonth, endDay;
    char start_date[11], end_date[11]; // Buffers to hold YYYY-MM-DD strings

    // Prompt user for start month and day
    do {
        printf("Enter the start month and day of the holiday (MM-DD): ");
        scanf("%d-%d", &startMonth, &startDay);
        sprintf(start_date, "2024-%02d-%02d", startMonth, startDay);
        if (!checkHolidayDate(startMonth, startDay, start_date))
            printf("(!) Invalid start date.\n");
    } while (!checkHolidayDate(startMonth, startDay, start_date));

    // Prompt user for end month and day
    do {
        printf("Enter the end month and day of the holiday (MM-DD): ");
        scanf("%d-%d", &endMonth, &endDay);
        sprintf(end_date, "2024-%02d-%02d", endMonth, endDay);
        if (!checkHolidayDate(endMonth, endDay, end_date) || strcmp(start_date, end_date) > 0) {
            printf("(!) Invalid end date or end date is before start date.\n");
        }
    } while (!checkHolidayDate(endMonth, endDay, end_date) || strcmp(start_date, end_date) > 0);

    // Store start and end dates in the HolidayNode
    sprintf(newHoliday->startDate, "2024-%s", start_date);
    sprintf(newHoliday->endDate, "2024-%s", end_date);

    newHoliday->next = *head;
    *head = newHoliday;

    printf("(*) New holiday added successfully.\n");
}

void deleteHoliday(HolidayNode **head) {
    // If the list is empty, return
    if (*head == NULL) {
        printf("(!) No holidays to delete.\n");
        return;
    }

    // Count the number of holidays
    int count = 0;
    HolidayNode *temp = *head;
    while (temp != NULL) {
        count++;
        temp = temp->next;
    }

    // Create an array of HolidayNode pointers to store the holidays
    HolidayNode **holidayArray = (HolidayNode**)malloc(count * sizeof(HolidayNode*));
    if (holidayArray == NULL) {
        printf("Memory allocation failed for holiday array.\n");
        return;
    }

    // Store the holidays in the array
    temp = *head;
    int i = 0;
    while (temp != NULL) {
        holidayArray[i++] = temp;
        temp = temp->next;
    }

    // Sort the holiday array based on start dates
    for (int i = 0; i < count - 1; i++) {
        for (int j = 0; j < count - i - 1; j++) {
            if (strcmp(holidayArray[j]->startDate, holidayArray[j + 1]->startDate) > 0) {
                HolidayNode *temp = holidayArray[j];
                holidayArray[j] = holidayArray[j + 1];
                holidayArray[j + 1] = temp;
            }
        }
    }

    // Print the list of holidays with their indices
    printf("Select the holiday to delete:\n");
    for (int i = 0; i < count; i++) {
        printf("[%d] %s\n", i + 1, holidayArray[i]->holidayName);
    }

    // Prompt the user to enter the index of the holiday to delete
    int selectedIndex;
    printf("Enter the index of the holiday to delete: ");
    scanf("%d", &selectedIndex);

    // Validate the selected index
    if (selectedIndex < 1 || selectedIndex > count) {
        printf("(!) Invalid index.\n");
        free(holidayArray);
        return;
    }

    // Delete the selected holiday from the list
    HolidayNode *deletedHoliday = holidayArray[selectedIndex - 1];
    HolidayNode *prev = NULL;
    temp = *head;
    while (temp != NULL && temp != deletedHoliday) {
        prev = temp;
        temp = temp->next;
    }
    if (temp == NULL) {
        printf("(!) Holiday not found.\n");
        free(holidayArray);
        return;
    }
    if (prev == NULL) {
        // If the first holiday is to be deleted
        *head = temp->next;
    } else {
        prev->next = temp->next;
    }
    free(temp);
    printf("(*) Holiday deleted successfully.\n");

    // Free the allocated memory for the holiday array
    free(holidayArray);

    // Write the updated holiday list to holidays.txt
    FILE *fp = fopen("holidays.txt", "w");
    if (fp == NULL) {
        printf("Failed to open holidays.txt for writing.\n");
        return;
    }

    temp = *head;
    while (temp != NULL) {
        fprintf(fp, "%s,%s,%s\n", temp->holidayName, temp->startDate, temp->endDate);
        temp = temp->next;
    }

    fclose(fp);
}


int holidayManagement(HolidayNode **holidayList) {
    char choice;
    do {
        printf("\n+--------- Holiday Management --------+\n");
        printf("1. View & Refresh Holiday List\n");
        printf("2. Add New Holiday\n");
        printf("3. Delete Holiday\n");
        printf("4. Back\n");
        printf("+-------------------------------------+\n");
        printf("Enter your choice: ");
        scanf(" %c", &choice);
        printf("+-------------------------------------+\n");

        switch (choice) {
            case '1':
                printHolidayList(*holidayList);
                break;
            case '2':
                createNewHoliday(holidayList);
                break;
            case '3':
                deleteHoliday(holidayList);
                break;
            case '4':
                return 0;
            default:
                printf("(!) Please enter a number between 1 and 4.\n");
        }
    } while (1);

    return 0;
}

// ----------------------------------------------------------------
// --------------- LECTURER ---------------------------------------
// ----------------------------------------------------------------
int checkCourseID(int courseID, char* courseName) {
    for (int i = 0; i < MAX_COURSES; i++) {
        if (courses[i].courseID == courseID) {
            strcpy(courseName, courses[i].course_name);
            return 1; // True, course found
        }
    }

    return 0; // False, course not found
}

int checkStudentID(int studentID) {
    for (int i = 0; i < MAX_STUDENTS; i++) {
        if (students[i].studentID == studentID) {
            return 1; // True, student found
        }
    }
    return 0; // False, student not found
}

int checkAttendanceRecordExists(int courseID, int studentID, const char* date) { // Check if attendance record exists
    FILE *fp;
    int sID, cID, lab, lecture;
    char fileDate[4]; 
    fp = fopen("attendance.txt", "r");
    if (!fp) {
        printf("Error opening attendance file.\n");
        return 0; 
    }

    while (fscanf(fp, "%d,%d,%[^,],%d,%d\n", &sID, &cID, fileDate, &lab, &lecture) == 5) {
        if (sID == studentID && cID == courseID && strcmp(fileDate, date) == 0) {
            fclose(fp);
            return 1; // Record exists 
        }
    }

    fclose(fp);
    return 0; // Record does not exist
}

void viewAndUpdateStudentAttendance() {
    int choice;
    int studentFound = 0;
    int courseFound = 0;
    int studentID, courseID;
    char date[4];
    char courseName[50];
    int labAttendance, lectureAttendance;
    FILE* fp;

    do { // Loop for the main menu
        printf("1. View Attendance\n");
        printf("2. Update Attendance\n");
        printf("0. Back\n");
        printf("Choice: ");
        if (scanf("%d", &choice) != 1) {
            printf("Invalid input. Please enter an integer.\n");
            clearBuffer();
            continue;
        }

        switch (choice) {
            case 1:
                do {
                    printf("Enter CourseID to view attendance (-2 to return): ");
                    while (scanf("%d", &courseID) != 1) {
                        clearBuffer();
                        printf("Invalid input. Please enter an integer for CourseID.\n");
                        printf("Enter CourseID to view attendance (-2 to return): ");
                    }
                    
                    if (courseID == -2) break; // Allows user to exit
                    int courseFound = checkCourseID(courseID, courseName);

                    if (!courseFound) {
                        printf("Course not found. Please try again.\n");
                        continue; // If not found, prompt again
                    }

                    // If course is found, then display its name and proceed to show attendance
                    printf("++++++++++++++++++++++++++++++++++++\n");
                    printf("Course: %s\n", courseName);
                    printf("++++++++++++++++++++++++++++++++++++\n");

                    // Open the attendance file
                    fp = fopen("attendance.txt", "r");
                    if (fp == NULL) {
                        printf("Error opening file.\n");
                        return;
                    }

                    int found = 0;
                    int tempCourseID;
                    while (fscanf(fp, "%d,%d,%[^,],%d,%d\n", &studentID, &tempCourseID, date, &labAttendance, &lectureAttendance) == 5) {
                        if (tempCourseID == courseID) {
                            printf("StudentID: %d\nWeek: %s\nLabAttendance: %d\nLectureAttendance: %d\n", studentID, date, labAttendance, lectureAttendance);
                            printf("------------------------------------\n");
                            found = 1;
                        }
                    }

                    fclose(fp);

                    if (!found) {
                        printf("No attendance records found for Course: %s.\n", courseName);
                    }
                } while (courseID != -2); // Loop until -2 is entered
                break;


            case 2: // Update Attendance
                while (1) {
                    printf("Enter date (W1-W15) or -2 to return: ");
                    scanf("%4s", date);
                    if (strcmp(date, "-2") == 0) return; // Exit if -2 is entered

                    if (!isValidWeek(date)) {
                        printf("Invalid week format. Please enter a valid week (W1-W15) or -2 to return.\n");
                        continue;
                    }

                    while (1) { // Loop for course ID input
                        printf("Enter CourseID (-2 to return): ");
                        if (scanf("%d", &courseID) != 1) {
                            clearBuffer(); // Clear any invalid input
                            printf("Invalid input. Please enter an integer for CourseID.\n");
                            continue;
                        }
                        if (courseID == -2) break; // Exit to date input if -2
                        int courseFound = checkCourseID(courseID, courseName); // Check if course exists

                        if (!courseFound) {
                            printf("Course not found. Please try again.\n");
                            continue; // If not found, prompt again
                        }
                        // If course is found, display its name                    
                        printf("++++++++++++++++++++++++++++++++++++\n");
                        printf("Course: %s\n", courseName);
                        printf("++++++++++++++++++++++++++++++++++++\n");

                        while (1) { // Nested loop for student ID input
                            printf("Enter StudentID (-2 to return): ");
                            if (scanf("%d", &studentID) != 1) {
                                printf("Invalid input. Please enter an integer for StudentID.\n");
                                clearBuffer(); // Clear any invalid input
                                continue;
                            }
                            if (studentID == -2) break; // Return to course ID input if -2

                            int studentFound = checkStudentID(studentID);
                            if (!studentFound) {
                                printf("Student not found. Please try again.\n");
                                continue;
                            }

                            if (checkAttendanceRecordExists(courseID, studentID, date)) {
                                printf("Attendance record already exists for StudentID %d.\n", studentID);
                                printf("Please try another date, course ID, or StudentID.\n");
                            }
                            else {
                                int labAttendance, lectureAttendance;
                                do {
                                    printf("Enter lab attendance for StudentID %d (1 for present, 0 for absent): ", studentID);
                                    while (scanf("%d", &labAttendance) != 1 || (labAttendance != 0 && labAttendance != 1)) {
                                        clearBuffer(); // Clear the buffer in case of invalid input
                                        printf("Invalid input. Enter 1 for present, 0 for absent: ");
                                    }
                                } while (labAttendance != 0 && labAttendance != 1); // Loop until valid input is entered

                                do {
                                    printf("Enter lecture attendance for StudentID %d (1 for present, 0 for absent): ", studentID);
                                    while (scanf("%d", &lectureAttendance) != 1 || (lectureAttendance != 0 && lectureAttendance != 1)) {
                                        clearBuffer(); // Clear the buffer in case of invalid input
                                        printf("Invalid input. Enter 1 for present, 0 for absent: ");
                                    }
                                } while (lectureAttendance != 0 && lectureAttendance != 1); // Loop until valid input is entered

                                // Write attendance data to file
                                fp = fopen("attendance.txt", "a");
                                if (!fp) {
                                    printf("Error opening file for append.\n");
                                    return;
                                }
                                fprintf(fp, "%d,%d,%s,%d,%d\n", studentID, courseID, date, labAttendance, lectureAttendance);
                                fclose(fp);
                                printf("Attendance for StudentID %d on %s updated successfully for Course %s.\n", studentID, date, courseName);
                            } 
                        }
                        if (studentID == -2) continue; // If -2 is entered for student ID, re-prompt for course ID
                        break; // Exit the loop after successful update or if -2 is entered for course ID
                    }
                }
                break;

            case 0:
                return;

            default:
                printf("Invalid choice. Please try again.\n");
                break;
        }
    } while (1);
}

void viewAndUpdateStudentMark() {
    int choice,courseID, studentID;
    float Mark;
    FILE* fp;
    int studentFound = 0;
    int courseFound = 0;
    char courseName[50];
    float lowerBoundary[MAX_GRADES];
    fp = fopen("grades.txt", "r"); // Read the lower boundary grades from grades.txt
    if (fp == NULL) {
        printf("Error opening grades.txt.\n");
        return;
    }
    for (int i = 0; i < MAX_GRADES; i++) {
        fscanf(fp, "%f", &lowerBoundary[i]);
    }
    fclose(fp); // Close the file after reading

    do {
        printf("1. View Marks\n");
        printf("2. Update Marks\n");
        printf("0. Back\n");
        printf("Choice: ");
        if (scanf("%d", &choice) != 1) {
            printf("Invalid input. Please enter an integer.\n");
            clearBuffer();
            continue;
        }
        switch (choice) {
            case 1: // View marks
                do {
                    printf("Enter CourseID to view mark(-2 to return): ");
                    if (scanf("%d", &courseID) != 1) {
                        clearBuffer();
                        printf("Invalid input. Please enter an integer for course ID.\n");
                        continue; // Continue the loop to prompt for a valid course ID input again
                    }

                    if (courseID == -2) {
                        break; // Return to the previous menu immediately if -2 is entered
                    }
                    courseFound = checkCourseID(courseID, courseName);
                    if (!courseFound) {
                        printf("Course not found. Please try again.\n");
                        continue; // Continue the loop to prompt for a valid course ID
                    }
                    else {
                        printf("++++++++++++++++++++++++++\n");
                        printf("Course: %s\n", courseName);
                        printf("++++++++++++++++++++++++++\n");

                        // Display marks for students in the specified course
                        for (int i = 0; i < MAX_STUDENTS; i++) {
                            studentFound = 0;
                            if (students[i].studentID == 0) {
                                break;
                            }

                            printf("StudentID: %d\n", students[i].studentID);

                            for (int j = 0; j < 2; j++) {
                                if (students[i].enrolled_courses[j].courseID == courseID) {
                                    printf("Mark: %.2f\nGrade: %c\n", students[i].enrolled_courses[j].marks, students[i].enrolled_courses[j].grade);
                                    printf("------------------------------------\n");
                                    studentFound = 1;
                                    break;
                                }
                            }

                            if (!studentFound) {
                                printf("Student not enrolled in this course.\n");
                            }
                        }
                    }
                } while (courseID != -2); // Loop until -2 is entered

                break;

            case 2: // Update marks
                do {
                    printf("Enter CourseID (-2 to return): ");
                    while (scanf("%d", &courseID) != 1) {
                        printf("Invalid input. Please enter an integer for CourseID.\n");
                        clearBuffer();
                        printf("Enter CourseID (-2 to return): ");
                    }
                    if (courseID == -2) {
                        break; // Return to the previous menu
                    }
                    courseFound = checkCourseID(courseID, courseName);
                    if (!courseFound) {
                        printf("Course not found. Please try again.\n");
                        continue; // Continue the loop to prompt for a valid course ID
                    }

                    // Input loop for updating marks
                    do {
                        printf("Enter StudentID (-1 to finish updating): ");
                        while (scanf("%d", &studentID) != 1) {
                            printf("Invalid input. Please enter an integer for StudentID.\n");
                            clearBuffer();
                            printf("Enter StudentID (-1 to finish updating): ");
                        }

                        if (studentID == -1) {
                            break; // Exit the loop if -1 is entered
                        }
                        
                        studentFound= checkStudentID(studentID);
                        if (!studentFound) {
                            printf("Student not found. Please try again.\n");
                        }
                        else {
                            // Update marks for the student in the specified course
                            printf("Enter mark for student %d: ", studentID);
                            while (scanf("%f", &Mark) != 1 || Mark < 0 || Mark > 100) {
                                printf("Invalid input. Please enter a mark between 0 and 100.\n");
                                printf("Enter mark for student %d: ", studentID);
                                clearBuffer();
                            }

                            // Update the mark and grade for the student in the course
                            int studentIndex, courseIndex;
                            for (studentIndex = 0; studentIndex < MAX_STUDENTS; studentIndex++) {
                                if (students[studentIndex].studentID == studentID) {
                                    for (courseIndex = 0; courseIndex < 2; courseIndex++) {
                                        if (students[studentIndex].enrolled_courses[courseIndex].courseID == courseID) {
                                            students[studentIndex].enrolled_courses[courseIndex].marks = Mark;
                                            students[studentIndex].enrolled_courses[courseIndex].grade = calculateGrade(Mark, lowerBoundary);
                                            printf("Mark updated for student %d in course %s\n", studentID, courseName);
                                            break; 
                                        }
                                    }
                                    break;
                                }
                            }
                        }
                    } while (1); // Infinite loop for updating marks until broken out of

                    // Update the student marks file after all updates are done
                    fp = fopen("studentmarks.txt", "w");
                    if (fp == NULL) {
                        printf("Error opening studentmarks.txt for writing.\n");
                        return;
                    }

                    for (int i = 0; i < MAX_STUDENTS; i++) {
                        if (students[i].studentID == 0) { 
                            break;
                        }

                        fprintf(fp, "%d,%d,%.2f,%c,%d,%.2f,%c\n",
                            students[i].studentID,
                            students[i].enrolled_courses[0].courseID,
                            students[i].enrolled_courses[0].marks,
                            students[i].enrolled_courses[0].grade,
                            students[i].enrolled_courses[1].courseID,
                            students[i].enrolled_courses[1].marks,
                            students[i].enrolled_courses[1].grade);
                    }

                    fclose(fp);
                    printf("Marks updated successfully!\n");
                } while (1); // Loop indefinitely for updating marks until exited explicitly

                break;

            case 0:
                return; // Exit the function

            default:
                printf("Invalid choice. Please try again.\n"); 
                break;
        }
    } while (1); // Main menu loop
}

void viewAndReplyFeedback() {
    int choice;
    FILE *fp;
    FILE *tempFile;
    int courseID;
    int feedbackID;
    char courseName[50]; // Buffer for storing the course name if found   
    int foundFeedback = 0;
    do {
        printf("Choose an option:\n1. View Feedback\n2. Reply to Feedback\n0. Back\nChoice: ");
        if (scanf("%d", &choice) != 1) {
            printf("Invalid input. Please enter an integer.\n");
            clearBuffer();
            continue;
        }

        switch (choice) {
            case 1: { // View Feedback
                while (1) {
                    printf("Enter CourseID (-2 to return): ");
                    if (scanf("%d", &courseID) != 1) {
                        printf("Invalid input. Please enter an integer for CourseID.\n");
                        clearBuffer();
                        continue;
                    }
                    clearBuffer();

                    if (courseID == -2) break; // Go back to the main menu

                    int courseFound = checkCourseID(courseID, courseName);
                    if (!courseFound) {
                        printf("Course not found. Please try again.\n");
                        continue;
                    }

                    printf("++++++++++++++++++++++++++++++++++++\n");
                    printf("Feedback for Course: %s\n", courseName);
                    printf("++++++++++++++++++++++++++++++++++++\n");

                    FeedbackNode* current = feedbackHead;
                    
                    while (current) {
                        if (current->courseID == courseID) {
                            printf("Feedback ID: %d\nDate: %s\nFeedback: %s\nReply: %s\n",
                                   current->feedbackID, current->date, current->feedback,
                                   strlen(current->reply) > 0 ? current->reply : "Pending Reply");
                            printf("------------------------------------\n");
                            foundFeedback = 1;
                        }
                        current = current->next;
                    }

                    if (!foundFeedback) {
                        printf("No feedback available for Course ID: %d\n", courseID);
                    }
                    break;
                }
                break;
            }
            case 2: { // Reply to Feedback
                while (1) {
                    printf("Enter Feedback ID to reply (-2 to go back): ");
                    if (scanf("%d", &feedbackID) != 1) {
                        printf("Invalid input. Please enter an integer for Feedback ID.\n");
                        clearBuffer();
                        continue;
                    }
                    clearBuffer(); // Clear buffer after reading

                    if (feedbackID == -2) break; // Go back to the main menu

                    FeedbackNode* current = feedbackHead;
                    int found = 0;
                    while (current) {
                        if (current->feedbackID == feedbackID) {
                            printf("Enter your reply: ");
                            fgets(current->reply, MAX_FEEDBACK_LENGTH, stdin);
                            current->reply[strcspn(current->reply, "\n")] = 0; // Remove newline character
                            found = 1;
                            printf("Reply for Feedback ID %d for successfully updated.\n", feedbackID);
                            // Open the feedbacks file for reading and writing
                            fp = fopen("feedbacks.txt", "r+");
                            if (!fp) {
                                printf("Error opening feedbacks file.\n");
                                return;
                            }

                            // Temporary file for writing updated content
                            tempFile = fopen("temp.txt", "w");
                            if (!tempFile) {
                                printf("Error creating temporary file.\n");
                                fclose(fp);
                                return;
                            }

                            char line[MAX_ENTRIES];
                            while (fgets(line, sizeof(line), fp)) {
                                int id;
                                sscanf(line, "%d,", &id);
                                if (id == feedbackID) {
                                    fprintf(tempFile, "%d,%s,%d,\"%s\",\"%s\"\n",
                                            current->feedbackID, current->date, current->courseID, current->feedback, current->reply);
                                } else {
                                    fputs(line, tempFile); // Write unchanged lines to the temporary file
                                }
                            }

                            fclose(fp);
                            fclose(tempFile);

                            // Replace the original file with the temporary file
                            remove("feedbacks.txt");
                            rename("temp.txt", "feedbacks.txt");
                            break; // No need to continue searching
                        }
                        current = current->next;
                    }

                    if (!found) {
                        printf("Feedback ID not found. Please try again.\n");
                    }
                }
                break;
            }

            case 0: // Exit
                printf("Exiting.\n");
                freeFeedbackList(feedbackHead); // Free memory allocated for the feedback list
                return;
            default:
                printf("Invalid choice. Please try again.\n");
                break;
        }
    } while (choice != 0);
}

// ----------------------------------------------------------------
// ---------------------------- STUDENT ---------------------------
// ----------------------------------------------------------------

// Function to view personal details of the logged-in student
void viewPersonalDetails(int loggedInUserId) {
    // Iterate over the students array
    for (int i = 0; i < MAX_STUDENTS; i++) {
        // Check if the studentID matches the loggedInUserId
        if (students[i].studentID == loggedInUserId) {
            // Print personal details of the logged-in student
            printf("\t\tStudent ID: %d\n", students[i].studentID);
            printf("\t\tFull Name: %s %s\n", students[i].student_fname, students[i].student_lname);
            printf("\t\tLevel: %s\n", students[i].level);
            printf("\t\tPhone Number: %d\n", students[i].student_details.phone_number);
            printf("\t\tDate of Birth: %s\n", students[i].student_details.dob);
            printf("\t\tNationality: %s\n", students[i].student_details.nationality);
            printf("\t\tGender: %c\n", students[i].student_details.gender);
            printf("\t\tCGPA: %.2f\n", students[i].cgpa);
            printf("\t\tEnrolled Courses: %s, %s\n", students[i].enrolled_courses[0].course_name, students[i].enrolled_courses[1].course_name);
            return; // Exit the function after printing details of the logged-in student
        }
    }

    printf("\t\tNo personal details found for student ID '%d'.\n", loggedInUserId);
}

void updatePersonalDetails(int loggedInUserId) {
    int numStudents;
    while (students[numStudents].studentID != 0) {
        numStudents++;
    }

    // Find the student with the given ID in the array
    for (int i = 0; i < numStudents; i++) {
        if (students[i].studentID == loggedInUserId) {
            char choice;

            printf("\t\tUpdate Personal Details Menu\n");
            printf("\t\t------------------------------\n");
            printf("\t\t1. Phone Number\n");
            printf("\t\t2. Date of Birth\n");
            printf("\t\t3. Nationality\n");
            printf("\t\t4. Skip, do not update\n");
            printf("\t\t------------------------------\n");
            printf("\t\tEnter your choice (1-4): ");
            scanf(" %c", &choice);

            // Update the corresponding student detail based on user choice
            switch (choice) {
                case '1':
                    printf("\tEnter updated phone number: ");
                    scanf("%d", &students[i].student_details.phone_number);
                    break;
                case '2':
                    printf("\tEnter updated date of birth (YYYY-MM-DD): ");
                    scanf("%s", students[i].student_details.dob);
                    break;
                case '3':
                    printf("\tEnter updated nationality: ");
                    scanf("%s", students[i].student_details.nationality);
                    break;
                case '4':
                    printf("\tSkipping the update.\n");
                    return;
                default:
                    printf("\tInvalid choice! Please enter a number from 1 to 4.\n");
                    return;
            }

            // Update the details in the file
            FILE* fp = fopen("studentdetails.txt", "w");
            if (fp == NULL) {
                printf("\t(!) Error opening studentdetails.txt for writing.\n");
                return;
            }

            // Rewrite the file with the updated data from the array
            for (int j = 0; j < numStudents; j++) {
                 if (students[j].studentID != 0) {
                    fprintf(fp, "%d,%s,%s,%d,%s,%s,%.2f,%s\n",
                        students[j].studentID, students[j].student_fname, students[j].student_lname,
                        students[j].student_details.phone_number, students[j].student_details.dob,
                        students[j].student_details.nationality, students[j].cgpa, students[j].level);
                }
            }

            fclose(fp);
            printf("\tPersonal details updated successfully.\n");
            return;
        }
    }

    // If the student ID was not found
    printf("\tStudent with ID '%d' not found.\n", loggedInUserId);
}

void viewAttendance(int loggedInUserId) {
    FILE *fp = fopen("attendance.txt", "r");
    if (fp == NULL) {
        printf("Error opening file.\n");
        return;
    }

    int studentID, courseID, labAttendance, lectureAttendance;
    char date[4];
    int found = 0;

    printf("Attendance records:\n");

    while (fscanf(fp, "%d,%d,%[^,],%d,%d\n", &studentID, &courseID, date, &labAttendance, &lectureAttendance) == 5) {
        if (studentID == loggedInUserId) { // Check if the record belongs to the logged-in user
            printf("StudentID: %d\n", studentID);
            printf("CourseID: %d\n", courseID);
            printf("Date: %s\n", date);
            printf("LabAttendance: %d\n", labAttendance);
            printf("LectureAttendance: %d\n", lectureAttendance);
            printf("--------------------\n");
            found = 1;
        }
    }

    fclose(fp);

    if (!found) {
        printf("No attendance records found for Student ID: %d\n", loggedInUserId);
    }
}

void viewEnrolledCoursesAndGrades(int loggedInUserId) {
    FILE* fp = fopen("studentmarks.txt", "r");
    if (fp == NULL) {
        printf("\t(!) Error opening studentmarks.txt.\n");
        return;
    }

    Student student; // Define a Student struct variable to hold data from the file

    int found = 0; // Variable to track if the student with given ID is found
    // Read student course details from the file and compare with the logged-in user ID
    while (fscanf(fp, "%d,%d,%f,%c,%d,%f,%c\n", &students[0].studentID, &courses[0].courseID, &courses[0].marks, &courses[0].grade,
        &courses[1].courseID, &courses[1].marks, &courses[1].grade) != EOF) {

        if (students[0].studentID == loggedInUserId) {
            printf("\t\tStudent ID: %d\n", students[0].studentID);
            printf("\t\tCourse ID: %d\t Mark: %.2f\t Grade: %c\n", courses[0].courseID, courses[0].marks, courses[0].grade);
            printf("\t\tCourse ID: %d\t Mark: %.2f\t Grade: %c\n", courses[1].courseID, courses[1].marks, courses[1].grade);
        }
    }

    fclose(fp); // Close the file after the loop
}

void viewHolidayCalendar() {
    FILE *fp = fopen("holidays.txt", "r");
    if (fp == NULL) {
        printf("Failed to open holidays.txt for reading.\n");
        return;
    }

    printf("Holiday Calendar:\n");
    printf("--------------------------------------------------\n");
    printf("| %-20s | %-11s | %-11s |\n", "Holiday", "Start Date", "End Date");
    printf("--------------------------------------------------\n");

    char holidayName[MAX_LENGTH], startDate[11], endDate[11];
    while (fscanf(fp, "%[^,],%[^,],%s\n", holidayName, startDate, endDate) == 3) {
        printf("| %-20s | %-11s | %-11s |\n", holidayName, startDate, endDate);
    }

    printf("--------------------------------------------------\n");

    fclose(fp);
}

// ----------------------------------------------------------------
// --------------- MENUS ------------------------------------------
// ----------------------------------------------------------------
int lecturerMenu() {
    int option;
    do {
        printf("\t+-------------------------------------------------------+\n");
        printf("\t|\t\tLecturer Menu\t\t\t\t|\n");
        printf("\t+-------------------------------------------------------+\n");
        printf("\t|\t1. View & Update Student Attendance\t\t|\n");
        printf("\t|\t2. View & Update Student Course Marks\t\t|\n");
        printf("\t|\t3. View & Reply Feedback from Students\t\t|\n");
        printf("\t|\t4. Log Out\t\t\t\t\t|\n");
        printf("\t+-------------------------------------------------------+\n");
        printf("\t\t\t\tOption: ");
        if (scanf("%d", &option) != 1) {
            clearBuffer();
            option = -1; // Set option to an invalid value
        }
        printf("\t+-------------------------------------------------------+\n");

        switch (option) {
        case 1: viewAndUpdateStudentAttendance(); break;
        case 2: viewAndUpdateStudentMark(); break;
        case 3: viewAndReplyFeedback(); break;
        case 4: printf("Logging out...\n"); return 0;
        default: printf("Invalid option.\n"); break;
        }
    } while (option != 4);


    return 0;
}

int programAdminMenu() {
    int option;
    do {
        printf("\t+-------------------------------------------------------+\n");
        printf("\t|\t\tProgramne Administrator Menu\t\t|\n");
        printf("\t+-------------------------------------------------------+\n");
        printf("\t|\t1. View\t\t\t\t\t\t|\n");
        printf("\t|\t2. Update Course(s)\t\t\t\t|\n");
        printf("\t|\t3. Enroll Student(s)\t\t\t\t|\n");
        printf("\t|\t4. Uneroll Student(s)\t\t\t\t|\n");
        printf("\t|\t5. Manage Holiday Schedule\t\t\t|\n");
        printf("\t|\t6. Log Out\t\t\t\t\t|\n");
        printf("\t+-------------------------------------------------------+\n");
        printf("\t\t\t\tOption: ");
        if (scanf("%d", &option) != 1) {
            int c;
            while ((c = getchar()) != '\n' && c != EOF);
            option = -1; // Set option to an invalid value
        }
        getchar();
        printf("\t+-------------------------------------------------------+\n");

        switch (option) {
        case 1: programAdmin_viewMenu(); break;
        case 2: programAdmin_manageMenu(); break;
        case 3: enrolStudents(); break;
        case 4: unenrolStudent(); break;
        case 5: holidayManagement(&holidayList); freeHolidayList(holidayList); break;
        case 6: printf("Logging out...\n"); return 0;
        default: printf("(!) Invalid option. Please try again.\n"); break;
        }
    } while (option != 6);
    return 0;
}

int systemAdminMenu() {
    int option;
    do {
        printf("\n\t+-------------------------------------------------------+\n");
        printf("\t|\t\tSystem Administrator Menu\t\t|\n");
        printf("\t+-------------------------------------------------------+\n");
        printf("\t|\t1. Manage Users\t\t\t\t\t|\n");
        printf("\t|\t2. Define Grading System\t\t\t|\n");
        printf("\t|\t3. Generate Report\t\t\t\t|\n");
        printf("\t|\t4. Log Out\t\t\t\t\t|\n");
        printf("\t+-------------------------------------------------------+\n");
        printf("\t\t\t\t\tOption: ");
        if (scanf("%d", &option) != 1) {
            int c;
            while ((c = getchar()) != '\n' && c != EOF);
            option = -1; // Set option to an invalid value
        }
        getchar();
        printf("\t+-------------------------------------------------------+\n");

        switch (option) {
        case 1: systemAdminSubmenu(); break;
        case 2: modifyLowerBoundaries(grades); calculateCGPA(); break;
        case 3: generateStudentReport(); break;
        case 4: printf("Logging out...\n"); return 0;
        default:printf("Invalid option. Please try again.\n"); break;
        }
    } while (option != 4);
    return 0;
}

int studentMenu(int loggedInUserId) {
    int option;
    do {
        printf("\t+-------------------------------------------------------+\n");
        printf("\t|\t\t\tSTUDENT MENU\t\t\t|\n");
        printf("\t+-------------------------------------------------------+\n");
        printf("\t|\t1. View Personal Details\t\t\t|\n");
        printf("\t|\t2. Update Personal Details\t\t\t|\n");
        printf("\t|\t3. View Enrolled Courses and Grades\t\t|\n");
        printf("\t|\t4. View Attendance\t\t\t\t|\n");
        printf("\t|\t5. View Holidays\t\t|\n");
        printf("\t|\t6. Log out\t\t\t\t\t|\n");
        printf("\t+-------------------------------------------------------+\n");
        printf("\tEnter your choice: ");
        if (scanf("%d", &option) != 1) {
            int c;
            while ((c = getchar()) != '\n' && c != EOF);
            option = -1; // Set choice to an invalid value
        }
        getchar();
        printf("\t+-------------------------------------------------------+\n");

        switch (option) {
        case 1: viewPersonalDetails(loggedInUserId); break;
        case 2: updatePersonalDetails(loggedInUserId); break;
        case 3: viewEnrolledCoursesAndGrades(loggedInUserId); break;
        case 4: viewAttendance(loggedInUserId); break;
        case 5: viewHolidayCalendar(); break;
        case 6: printf("\tLogging out.......\n"); return 0;
        default: printf("\t\tInvalid choice! Please try again.\n"); break;
        }
    } while (option != 10);
    return 0;
}

int login() {
    int login_id, login_pw;
    int attempt = 0;
    int found = 0;

    while (attempt < 3) {
        printf("(Attempt %d) Enter UserID: ", attempt + 1);
        if (scanf("%d", &login_id) != 1 || login_id == 0) { // Check for 0 input
            printf("Invalid input. Please enter a valid integer greater than 0.\n");
            clearBuffer(); // Clear the input buffer to remove invalid input
            attempt++;
            continue; // Skip to the next attempt
        }

        printf("Enter password: ");
        if (scanf("%d", &login_pw) != 1 || login_pw == 0) { // Check for 0 input
            printf("Invalid input. Please enter a valid integer greater than 0.\n");
            clearBuffer(); // Clear the input buffer to remove invalid input
            attempt++;
            continue; // Skip to the next attempt
        }

        // Check if both username and password match any user
        for (int i = 0; i < MAX_USERS; i++) {
            if (login_id == users[i].userID && login_pw == users[i].password) {
                printf("Login successful. User ID: %d\n", users[i].userID);
                found = 1;
                switch (users[i].usertype[0]) { // Direct user based on type
                case 'l': lecturerMenu(); break;
                case 's': studentMenu(login_id); break;
                case 'p': programAdminMenu(); break;
                case 'y': systemAdminMenu(); break;
                }
                return 0; // Login success
            }
        }

        if (!found) {
            printf("Invalid username or password.\n");
            attempt++;
        }
    }

    printf("Login failed after 3 attempts.\n");
    return -1;
}

void mainMenu() {
    int option;
    do {
        printf("\tMain Menu\n");
        printf("\t+-------------------------------+\n");
        printf("\t|\tMENU OPTIONS\t\t|\n");
        printf("\t+-------------------------------+\n");
        printf("\t|\t1. Login\t\t|\n");
        printf("\t|\t2. Quit Application\t|\n");
        printf("\t+-------------------------------+\n");
        printf("\t\tOption: ");
        if (scanf("%d", &option) != 1) {
            printf("Invalid input. Please enter a valid integer.\n");
            clearBuffer();
            continue;
        }
        printf("\t+-------------------------------+\n");

        switch (option) {
            case 1:
                login();
                break;
            case 2:
                printf("Quitting application.\n");
                return;
            default:
                printf("Invalid choice. Please try again.\n");
                break;
        }
    } while (1);
}

int createRecords() {
    if (createUsers() != 0) {
        printf("Error creating users.\n");
        return 1;
    }
    else {
        printf("USERS.TXT created successfully.\n");
    }

    if (createStudents() != 0) {
        printf("Error creating students.\n");
        return 1;
    }
    else {
        printf("STUDENTDETAILS.TXT and STUDENTMARKS.TXT created successfully.\n");
    }

    if (createCourses() != 0) {
        printf("Error creating courses.\n");
        return 1;
    }
    else {
        printf("COURSES.TXT created successfully.\n");
    }

    if (createAttendance() != 0) {
        printf("Error creating attendance.\n");
        return 1;
    }
    else {
        printf("ATTENDANCE.TXT created successfully.\n");
    }

    if (createGrades() != 0) {
        printf("Error creating grades.\n");
        return 1;
    }
    else {
        printf("GRADES.TXT created successfully.\n");
    }

    if (createFeedback() != 0) {
        printf("Error creating feedbacks.\n");
        return 1;
    }
    else {
        printf("FEEDBACKS.TXT created successfully.\n");
    }
}

int main() {
    createRecords();
    holidayList = createHolidayList(); // Initialize the holiday list
    if (holidayList == NULL) {
        printf("Failed to initialize holiday list.\n");
        return 1;
    } else {
        printf("Holiday list initialized successfully.\n");
    }
    
    mainMenu();
    freeHolidayList(holidayList); 
    return 0;
}