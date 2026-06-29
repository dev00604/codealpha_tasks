#include <iostream>
#include <vector>
using namespace std;

class Subject
{
public:
    string subjectname;
    int credit;
    float gradepoint;

    // Constructor
    Subject(string name, int cr, float gp)
    {
        subjectname = name;
        credit = cr;
        gradepoint = gp;
    }
};

class Student
{
public:
    string name;
    vector<Subject> subjects;

    // Constructor
    Student(string studentName)
    {
        name = studentName;
    }

    void addSubject(string subjectName, int credit, float gradepoint)
    {
        Subject newSubject(subjectName, credit, gradepoint);
        subjects.push_back(newSubject);
    }

    float calculateCGPA()
    {
        int totalCredit = 0;
        float weightedGradePoint = 0;

        for (const Subject &subject : subjects)
        {
            totalCredit += subject.credit;
            weightedGradePoint += subject.credit * subject.gradepoint;
        }

        return (totalCredit > 0) ? (weightedGradePoint / totalCredit) : 0;
    }

    void displayCGPA()
    {
        cout << "\nStudent Name: " << name << endl;
        cout << "CGPA: " << calculateCGPA() << endl;
    }
};

int main()
{
    string studentName;
    int numSub;

    cout << "Enter student's name: ";
    getline(cin, studentName);

    cout << "Enter the number of subjects: ";
    cin >> numSub;

    Student student(studentName);

    for (int i = 0; i < numSub; i++)
    {
        string subjectName;
        int credit;
        float gradePoint;

        cin.ignore();

        cout << "\nEnter subject name: ";
        getline(cin, subjectName);

        cout << "Enter credit: ";
        cin >> credit;

        cout << "Enter grade point: ";
        cin >> gradePoint;

        student.addSubject(subjectName, credit, gradePoint);
    }

    student.displayCGPA();

    return 0;
}