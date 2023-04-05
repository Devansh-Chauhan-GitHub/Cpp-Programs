#include <iostream>
#include <fstream>
#include <vector>
#include <sstream>
#include <mysqlx/xdevapi.h>

using namespace std;

class Student {
private:
    string name;
    int rollNo;
    float mathMarks;
    float scienceMarks;
    float englishMarks;
    float totalMarks;
    float percentage;
public:
    Student(string name, int rollNo, float mathMarks, float scienceMarks, float englishMarks) {
        this->name = name;
        this->rollNo = rollNo;
        this->mathMarks = mathMarks;
        this->scienceMarks = scienceMarks;
        this->englishMarks = englishMarks;
        this->totalMarks = mathMarks + scienceMarks + englishMarks;
        this->percentage = totalMarks / 3;
    }

    string getName() {
        return name;
    }

    int getRollNo() {
        return rollNo;
    }

    float getMathMarks() {
        return mathMarks;
    }

    float getScienceMarks() {
        return scienceMarks;
    }

    float getEnglishMarks() {
        return englishMarks;
    }

    float getTotalMarks() {
        return totalMarks;
    }

    float getPercentage() {
        return percentage;
    }
};

vector<Student> students;

void writeToFile(Student s) {
    ofstream file("students.txt", ios::app);
    if (file) {
        file << s.getName() << "," << s.getRollNo() << "," << s.getMathMarks() << "," << s.getScienceMarks() << "," << s.getEnglishMarks() << endl;
        file.close();
    } else {
        cout << "Error writing to file" << endl;
    }
}

void readFromFile() {
    ifstream file("students.txt");
    if (file) {
        string line;
        while (getline(file, line)) {
            stringstream ss(line);
            string name;
            int rollNo;
            float mathMarks, scienceMarks, englishMarks;
            getline(ss, name, ',');
            ss >> rollNo;
            ss.ignore();
            ss >> mathMarks;
            ss.ignore();
            ss >> scienceMarks;
            ss.ignore();
            ss >> englishMarks;
            ss.ignore();
            Student s(name, rollNo, mathMarks, scienceMarks, englishMarks);
            students.push_back(s);
        }
        file.close();
    } else {
        cout << "Error reading from file" << endl;
    }
}

void insertIntoDatabase(Student s) {
    try {
        mysqlx::Session session("localhost", 33060, "root", "password");
        mysqlx::Schema schema = session.getSchema("student_records");
        mysqlx::Table students = schema.getTable("students");
        students.insert("name", "rollNo", "mathMarks", "scienceMarks", "englishMarks", "totalMarks", "percentage")
            .values(s.getName(), s.getRollNo(), s.getMathMarks(), s.getScienceMarks(), s.getEnglishMarks(), s.getTotalMarks(), s.getPercentage())
            .execute();
        cout << "Record inserted into database" << endl;
    } catch (mysqlx::Error e) {
        cout << "Error inserting record into database: " << e.what() << endl;
    }
}
void readFromDatabase() {
    try {
        mysqlx::Session session("localhost", 33060, "root", "password");
        mysqlx::Schema schema = session.getSchema("student_records");
        mysqlx::Table students = schema.getTable("students");
        mysqlx::RowResult rows = students.select("*").execute();
        for (auto row : rows) {
            string name = row[0];
            int rollNo = row[1];
            float mathMarks = row[2];
            float scienceMarks = row[3];
            float englishMarks = row[4];
            float totalMarks = row[5];
            float percentage = row[6];
            Student s(name, rollNo, mathMarks, scienceMarks, englishMarks);
            students.push_back(s);
        }
        cout << "Records read from database" << endl;
    } catch (mysqlx::Error e) {
        cout << "Error reading records from database: " << e.what() << endl;
    }
}

void printStudents() {
    for (auto s : students) {
        cout << "Name: " << s.getName() << endl;
        cout << "Roll No.: " << s.getRollNo() << endl;
        cout << "Math Marks: " << s.getMathMarks() << endl;
        cout << "Science Marks: " << s.getScienceMarks() << endl;
        cout << "English Marks: " << s.getEnglishMarks() << endl;
        cout << "Total Marks: " << s.getTotalMarks() << endl;
        cout << "Percentage: " << s.getPercentage() << "%" << endl;
        cout << endl;
    }
}

int main() {
    readFromFile();
    readFromDatabase();
    printStudents();

    string name;
    int rollNo;
    float mathMarks, scienceMarks, englishMarks;

    cout << "Enter name: ";
    getline(cin, name);
    cout << "Enter roll no.: ";
    cin >> rollNo;
    cout << "Enter math marks: ";
    cin >> mathMarks;
    cout << "Enter science marks: ";
    cin >> scienceMarks;
    cout << "Enter english marks: ";
    cin >> englishMarks;
    cin.ignore();
    Student s(name, rollNo, mathMarks, scienceMarks, englishMarks);
    writeToFile(s);
    insertIntoDatabase(s);

    return 0;
}


       
