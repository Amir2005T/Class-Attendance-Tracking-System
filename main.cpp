#include "AttendanceSystem.h"
#include <iostream>

using namespace std;

int main() {
    AttendanceSystem sys;
    int pChoice, mChoice;
    string id, name;

    while (true) {
        system("cls");
        cout << "====================================\n";
        cout << "   MMU ATTENDANCE PORTAL \n";
        cout << "====================================\n";
        cout << "1. Teacher Portal (Admin)\n2. Student Portal\n3. Exit\nChoice: ";
        cin >> pChoice;

        if (pChoice == 1) {
            if (sys.login()) {
                while (true) {
                    system("cls");
                    cout << "--- TEACHER DASHBOARD ---\n";
                    cout << "1. Add Student\n2. Mark Attendance\n3. View Stats\n4. Edit Student\n5. Delete Student\n6. Logout\nChoice: ";
                    cin >> mChoice;
                    if (mChoice == 6) break;
                    switch(mChoice) {
                        case 1:
                            cout << "ID (Numbers only): "; cin >> id;
                            while (!sys.isNumeric(id)) { cout << "Invalid ID! Numbers only: "; cin >> id; }
                            cin.ignore(); cout << "Name: "; getline(cin, name);
                            sys.addStudent(id, name); break;
                        case 2: sys.markAttendance(); break;
                        case 3: sys.displayStatistics(); break;
                        case 4: cout << "ID to Edit: "; cin >> id; sys.editStudent(id); break;
                        case 5: cout << "ID to Delete: "; cin >> id; sys.deleteStudent(id); break;
                    }
                    system("pause");
                }
            }
        } else if (pChoice == 2) {
            cout << "Enter Student ID (Numbers only): "; cin >> id;
            while (!sys.isNumeric(id)) { cout << "Invalid ID! Numbers only: "; cin >> id; }
            sys.searchStudent(id); system("pause");
        } else if (pChoice == 3) break;
    }
    return 0;
}
