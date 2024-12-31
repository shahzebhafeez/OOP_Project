#include <iostream>
#include <string>

using namespace std;

class Person {
protected:
    int id;
    string fullName;
    int graduationYear;

public:
    Person(int identifier, string name, int year) : id(identifier), fullName(name), graduationYear(year) {}

    virtual void displayInfo() const {
        cout << "ID: " << id << endl;
        cout << "Name: " << fullName << endl;
        cout << "Graduation Year: " << graduationYear << endl;
    }

    int getGraduationYear() const {
        return graduationYear;
    }
};

class Graduate : public Person {
private:
    string faculty;
    string researchTitle;

public:
    Graduate(int identifier, string name, int year, string dept, string thesis)
        : Person(identifier, name, year), faculty(dept), researchTitle(thesis) {}

    string getResearchTitle() const {
        return researchTitle;
    }

    void displayInfo() const override {
        Person::displayInfo();
        cout << "Faculty: " << faculty << endl;
        cout << "Research Topic: " << researchTitle << endl;
    }
};

class GraduationCeremony {
private:
    int eventYear;
    Person* attendees[50];
    int attendeeCount = 0;

public:
    GraduationCeremony(int year) : eventYear(year) {}

    void addAttendee(Person* person) {
        if (person->getGraduationYear() == eventYear && attendeeCount < 50) {
            attendees[attendeeCount++] = person;
        }
    }

    void showAttendees() const {
        cout << "Attendees for Graduation Ceremony " << eventYear << ":\n";
        for (int i = 0; i < attendeeCount; i++) {
            attendees[i]->displayInfo();
            cout << "---\n";
        }
    }
};

class AcademicConference {
private:
    string conferenceTitle;
    int eventYear;
    Graduate* invitedGraduates[50];
    int inviteCount = 0;

public:
    AcademicConference(string title, int year) : conferenceTitle(title), eventYear(year) {}

    void inviteGraduate(Graduate* graduate, GraduationCeremony& ceremony) {
        if (graduate->getGraduationYear() < eventYear) {
            ceremony.addAttendee(graduate);
            if (inviteCount < 50) {
                invitedGraduates[inviteCount++] = graduate;
            }
        }
    }

    void showInvitedGraduates() const {
        cout << "Graduate Invitees for " << conferenceTitle << " (" << eventYear << "):\n";
        for (int i = 0; i < inviteCount; i++) {
            invitedGraduates[i]->displayInfo();
            cout << "---\n";
        }
    }
};

int main() {
    Person undergrad1(999, "Shahzeb hafeez", 2027);
    Graduate grad1(1000, "Hasnat Sajid", 2027, "Engineering", "Machine Learning Applications");

    GraduationCeremony ceremony2025(2025);
    ceremony2025.addAttendee(&undergrad1);
    ceremony2025.addAttendee(&grad1);

    ceremony2025.showAttendees();

    AcademicConference aiConference("AI Innovations Summit", 2026);
    aiConference.inviteGraduate(&grad1, ceremony2025);

    aiConference.showInvitedGraduates();

    return 0;
}
