#ifndef ATTENDANCE_SYSTEM_H
#define ATTENDANCE_SYSTEM_H

#include <string>
#include <iostream>

using namespace std;

const int TABLE_SIZE = 10;

// DS 1: Manual Linked List for Dates (Nested History)
struct AttendanceDate {
    string date;
    string status;
    string subject; // STORES THE SPECIFIC SUBJECT
    AttendanceDate* next;
    AttendanceDate(string d, string s, string sub)
        : date(d), status(s), subject(sub), next(nullptr) {}
};

// DS 2: Hash Table Student Node
struct Student {
    string id;
    string name;
    int attendanceCount;
    AttendanceDate* historyHead;
    Student* next; // For External Chaining
    Student(string i, string n) : id(i), name(n), attendanceCount(0), historyHead(nullptr), next(nullptr) {}
};

// DS 3: Manual Queue for Marking (FIFO Logic)
struct QueueNode {
    Student* studentRef;
    QueueNode* next;
    QueueNode(Student* s) : studentRef(s), next(nullptr) {}
};

class AttendanceSystem {
private:
    Student* hashTable[TABLE_SIZE];
    int totalSessions;

    // Manual Algorithms
    int hashFunction(string id);
    Student* mergeSort(Student* head);
    Student* split(Student* head);
    Student* merge(Student* first, Student* second);

public:
    AttendanceSystem();
    ~AttendanceSystem();

    void addStudent(string id, string name);
    void editStudent(string id);
    void deleteStudent(string id);
    void searchStudent(string id);
    void markAttendance();
    void displayStatistics();

    // Security & Persistence
    bool login();
    void saveToFile();
    void loadFromFile();

    // Input Validation Helper
    bool isNumeric(string id);
};

#endif
