#include "AttendanceSystem.h"
#include <iostream>
#include <iomanip>
#include <fstream>
#include <windows.h>

using namespace std;

// UI Color Coding helper for the evaluation sheet "Enhanced UI" requirement
void setColor(int color) {
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    SetConsoleTextAttribute(hConsole, color);
}

// Input Validation: Ensures Student IDs are strictly numeric
bool AttendanceSystem::isNumeric(string id) {
    if (id.empty()) return false;
    for (char const &c : id) {
        if (isdigit(c) == 0) return false;
    }
    return true;
}

AttendanceSystem::AttendanceSystem() : totalSessions(0) {
    for (int i = 0; i < TABLE_SIZE; i++)
        hashTable[i] = nullptr;
    loadFromFile();
}

// Destructor: Performs nested memory cleanup to prevent leaks
AttendanceSystem::~AttendanceSystem() {
    saveToFile();
    for (int i = 0; i < TABLE_SIZE; i++) {
        Student* temp = hashTable[i];
        while (temp) {
            Student* toDelete = temp;
            AttendanceDate* dTemp = toDelete->historyHead;
            while(dTemp) {
                AttendanceDate* dDel = dTemp;
                dTemp = dTemp->next;
                delete dDel;
            }
            temp = temp->next;
            delete toDelete;
        }
    }
}

int AttendanceSystem::hashFunction(string id) {
    int sum = 0;
    for (char c : id) sum += c;
    return sum % TABLE_SIZE;
}

void AttendanceSystem::saveToFile() {
    ofstream outFile("attendance.txt");
    outFile << totalSessions << endl;
    for (int i = 0; i < TABLE_SIZE; i++) {
        Student* temp = hashTable[i];
        while (temp) {
            outFile << temp->id << "|" << temp->name << "|" << temp->attendanceCount << endl;
            temp = temp->next;
        }
    }
    outFile.close();
}

void AttendanceSystem::loadFromFile() {
    ifstream inFile("attendance.txt");
    if(!inFile) return;

    inFile >> totalSessions;
    inFile.ignore();
    string id, name;
    int count;

    while (getline(inFile, id, '|') && getline(inFile, name, '|') && (inFile >> count)) {
        inFile.ignore();
        addStudent(id, name);
        int idx = hashFunction(id);
        if(hashTable[idx]) hashTable[idx]->attendanceCount = count;
    }
    inFile.close();
}

void AttendanceSystem::addStudent(string id, string name) {
    int index = hashFunction(id);
    Student* newNode = new Student(id, name);
    newNode->next = hashTable[index];
    hashTable[index] = newNode;
}

// Mark Attendance: Relational implementation with FIFO Queue
void AttendanceSystem::markAttendance() {
    string sDate, sSubject;
    setColor(11);
    cout << "Enter Subject Name: "; cin.ignore(); getline(cin, sSubject);
    cout << "Enter Date (DD/MM/YYYY): "; cin >> sDate;
    setColor(7);

    QueueNode *qFront = nullptr, *qRear = nullptr;
    for (int i = 0; i < TABLE_SIZE; i++) {
        Student* temp = hashTable[i];
        while (temp) {
            QueueNode* newNode = new QueueNode(temp);
            if (!qRear) qFront = qRear = newNode;
            else { qRear->next = newNode; qRear = newNode; }
            temp = temp->next;
        }
    }
    if (!qFront) return;

    totalSessions++;
    while (qFront) {
        Student* current = qFront->studentRef;
        char choice;
        cout << "Is " << current->name << " (" << current->id << ") present for " << sSubject << "? (y/n): ";
        cin >> choice;

        string status = (choice == 'y' || choice == 'Y') ? "Present" : "Absent";
        if (status == "Present") current->attendanceCount++;

        AttendanceDate* newDate = new AttendanceDate(sDate, status, sSubject);
        newDate->next = current->historyHead;
        current->historyHead = newDate;

        QueueNode* toDelete = qFront;
        qFront = qFront->next;
        delete toDelete;
    }
}

// Student Portal: Granular subject table with individual requirements
void AttendanceSystem::searchStudent(string id) {
    int index = hashFunction(id);
    Student* temp = hashTable[index];
    while (temp) {
        if (temp->id == id) {
            system("cls");
            setColor(11); cout << "========================================================\n";
            cout << "                STUDENT ATTENDANCE RECORD               \n";
            cout << "========================================================\n"; setColor(7);
            cout << "Name : " << temp->name << " | ID : " << temp->id << endl << endl;

            cout << "--- Attendance by Subject ---\n";
            cout << left << setw(15) << "Subject" << setw(10) << "Total" << setw(10) << "Present" << "Status/Rate\n";
            cout << "--------------------------------------------------------\n";

            struct SubData { string name; int t; int p; } subs[50];
            int subCount = 0;
            AttendanceDate* d = temp->historyHead;
            while (d) {
                int found = -1;
                for(int i=0; i<subCount; i++) if(subs[i].name == d->subject) found = i;
                if(found != -1) { subs[found].t++; if(d->status == "Present") subs[found].p++; }
                else if(subCount < 50) { subs[subCount] = {d->subject, 1, (d->status == "Present"? 1 : 0)}; subCount++; }
                d = d->next;
            }

            for (int i = 0; i < subCount; i++) {
                float rate = ((float)subs[i].p / subs[i].t) * 100;
                cout << left << setw(15) << subs[i].name << setw(10) << subs[i].t << setw(10) << subs[i].p;
                if (rate < 80.0) { setColor(12); cout << fixed << setprecision(2) << rate << "% [BELOW REQ]"; }
                else { setColor(10); cout << fixed << setprecision(2) << rate << "% [SATISFACTORY]"; }
                setColor(7); cout << endl;
            }

            cout << "\n--- Detailed History ---\n";
            d = temp->historyHead;
            while (d) {
                cout << "Date: " << left << setw(12) << d->date << " | Subject: " << setw(12) << d->subject << " | Status: ";
                if(d->status == "Present") setColor(10); else setColor(12);
                cout << d->status << endl; setColor(7);
                d = d->next;
            }
            return;
        }
        temp = temp->next;
    }
    setColor(12); cout << "Student ID not found.\n"; setColor(7);
}

// Teacher Portal: Statistics report showing subject breakdowns
void AttendanceSystem::displayStatistics() {
    Student* flatList = nullptr;
    for (int i = 0; i < TABLE_SIZE; i++) {
        Student* temp = hashTable[i];
        while (temp) {
            Student* copy = new Student(temp->id, temp->name);
            copy->historyHead = temp->historyHead;
            copy->next = flatList; flatList = copy;
            temp = temp->next;
        }
    }
    flatList = mergeSort(flatList);

    setColor(11);
    cout << "\n" << left << setw(10) << "ID" << setw(15) << "Name" << setw(15) << "Subject" << "Rate (%) / Status" << endl;
    cout << "----------------------------------------------------------------------\n"; setColor(7);

    while (flatList) {
        struct SubData { string name; int t; int p; } subs[50];
        int sC = 0;
        AttendanceDate* d = flatList->historyHead;
        while (d) {
            int f = -1;
            for(int i=0; i<sC; i++) if(subs[i].name == d->subject) f = i;
            if(f != -1) { subs[f].t++; if(d->status == "Present") subs[f].p++; }
            else if(sC < 50) { subs[sC] = {d->subject, 1, (d->status == "Present"? 1 : 0)}; sC++; }
            d = d->next;
        }

        for (int i = 0; i < sC; i++) {
            float r = ((float)subs[i].p / subs[i].t) * 100;
            if (i == 0) cout << left << setw(10) << flatList->id << setw(15) << flatList->name;
            else cout << left << setw(10) << "" << setw(15) << "";
            cout << left << setw(15) << subs[i].name;
            if (r < 80.0) { setColor(12); cout << fixed << setprecision(2) << r << "% [LOW]"; }
            else { setColor(10); cout << fixed << setprecision(2) << r << "% [OK]"; }
            setColor(7); cout << endl;
        }
        cout << "----------------------------------------------------------------------\n";
        Student* del = flatList; flatList = flatList->next; delete del;
    }
}

// Merge Sort Algorithms
Student* AttendanceSystem::merge(Student* first, Student* second) {
    if (!first) return second;
    if (!second) return first;
    if (first->name <= second->name) {
        first->next = merge(first->next, second);
        return first;
    } else {
        second->next = merge(first, second->next);
        return second;
    }
}

Student* AttendanceSystem::split(Student* head) {
    Student *fast = head, *slow = head;
    while (fast->next && fast->next->next) {
        fast = fast->next->next;
        slow = slow->next;
    }
    Student* temp = slow->next;
    slow->next = nullptr;
    return temp;
}

Student* AttendanceSystem::mergeSort(Student* node) {
    if (!node || !node->next) return node;
    Student* second = split(node);
    node = mergeSort(node);
    second = mergeSort(second);
    return merge(node, second);
}

void AttendanceSystem::editStudent(string oldId) {
    int index = hashFunction(oldId);
    Student* temp = hashTable[index];
    Student* prev = nullptr;
    while (temp && temp->id != oldId) { prev = temp; temp = temp->next; }
    if (temp) {
        string newId, newName;
        cout << "Enter New Name (Enter to skip): "; cin.ignore(); getline(cin, newName);
        cout << "Enter New ID (Enter to skip): "; getline(cin, newId);
        if (!newName.empty()) temp->name = newName;
        if (!newId.empty() && newId != oldId) {
            if (!isNumeric(newId)) { cout << "Invalid ID. Numeric only.\n"; return; }
            if (prev == nullptr) hashTable[index] = temp->next;
            else prev->next = temp->next;
            temp->id = newId;
            int nIdx = hashFunction(newId);
            temp->next = hashTable[nIdx];
            hashTable[nIdx] = temp;
        }
    }
}

void AttendanceSystem::deleteStudent(string id) {
    int index = hashFunction(id);
    Student *temp = hashTable[index], *prev = nullptr;
    while (temp && temp->id != id) { prev = temp; temp = temp->next; }
    if (!temp) return;
    if (!prev) hashTable[index] = temp->next;
    else prev->next = temp->next;
    delete temp;
}

bool AttendanceSystem::login() {
    string p;
    cout << "Admin Password: "; cin >> p;
    return (p == "admin123");
}
