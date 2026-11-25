#include <iostream>
#include <fstream>
#include <string>
#include <iomanip>
#include <sstream>
#include <cstdlib>

using namespace std;

const int MAX_DOCTORS = 5;
const int MAX_PER_DAY = 3;

const int SCHEDULED = 1;
const int COMPLETED = 2;
const int CANCELLED = 3;

const int ACTION_ADD = 1;
const int ACTION_UPDATE = 2;
const int ACTION_DELETE = 3;

string toLower(string s)
{
    for (int i = 0; i < (int)s.size(); i++)
    {
        if (s[i] >= 'A' && s[i] <= 'Z')
            s[i] = s[i] + 32;
    }
    return s;
}

string trim(string s)
{
    int start = 0;
    while (start < (int)s.size() && (s[start] == ' ' || s[start] == '\t' || s[start] == '\r'))
        start++;
    int end = s.size();
    while (end > start && (s[end - 1] == ' ' || s[end - 1] == '\t' || s[end - 1] == '\r'))
        end--;
    return s.substr(start, end - start);
}

string toText(int n)
{
    ostringstream out;
    out << n;
    return out.str();
}

bool isNumber(string s)
{
    if (s.empty() || s.size() > 9)
        return false;
    for (int i = 0; i < (int)s.size(); i++)
    {
        if (s[i] < '0' || s[i] > '9')
            return false;
    }
    return true;
}

bool validDate(string d)
{
    if (d.size() != 10 || d[4] != '-' || d[7] != '-')
        return false;
    for (int i = 0; i < 10; i++)
    {
        if (i != 4 && i != 7 && (d[i] < '0' || d[i] > '9'))
            return false;
    }

    int year = atoi(d.substr(0, 4).c_str());
    int month = atoi(d.substr(5, 2).c_str());
    int day = atoi(d.substr(8, 2).c_str());
    if (year < 2024 || year > 2100 || month < 1 || month > 12)
        return false;

    int days[12] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
    int maxDay = days[month - 1];
    bool leap = (year % 4 == 0 && year % 100 != 0) || year % 400 == 0;
    if (month == 2 && leap)
        maxDay = 29;
    return day >= 1 && day <= maxDay;
}

int splitLine(string line, string parts[], int maxParts)
{
    int count = 0;
    string current = "";
    for (int i = 0; i < (int)line.size(); i++)
    {
        if (line[i] == '|' && count < maxParts - 1)
        {
            parts[count] = current;
            count++;
            current = "";
        }
        else
        {
            current += line[i];
        }
    }
    parts[count] = current;
    return count + 1;
}

string statusName(int status)
{
    if (status == SCHEDULED)
        return "Scheduled";
    if (status == COMPLETED)
        return "Completed";
    return "Cancelled";
}

string getInput(string message)
{
    string s;
    cout << message;
    if (!getline(cin, s))
    {
        cout << "\nInput ended. Exiting.\n";
        exit(0);
    }
    return s;
}

int readInt(string message, int low, int high)
{
    while (true)
    {
        string s = trim(getInput(message));
        if (isNumber(s))
        {
            int value = atoi(s.c_str());
            if (value >= low && value <= high)
                return value;
        }
        cout << "Invalid input. Enter a number between " << low << " and " << high << ".\n";
    }
}

string readText(string message)
{
    while (true)
    {
        string s = trim(getInput(message));
        if (s.empty())
            cout << "Input cannot be empty.\n";
        else if (s.find('|') != string::npos)
            cout << "The character | is not allowed.\n";
        else
            return s;
    }
}

string readPhone(string message)
{
    while (true)
    {
        string s = trim(getInput(message));
        bool ok = s.size() >= 7 && s.size() <= 15;
        for (int i = 0; i < (int)s.size(); i++)
        {
            if (s[i] < '0' || s[i] > '9')
                ok = false;
        }
        if (ok)
            return s;
        cout << "Phone must be 7 to 15 digits.\n";
    }
}

string readDate(string message)
{
    while (true)
    {
        string s = trim(getInput(message));
        if (validDate(s))
            return s;
        cout << "Invalid date. Use YYYY-MM-DD, for example 2026-12-05.\n";
    }
}

struct Doctor
{
    int id;
    string name;
    string department;
};

Doctor doctors[MAX_DOCTORS] = {
    {1, "Dr. Ahmed Raza", "Cardiology"},
    {2, "Dr. Sana Malik", "Pediatrics"},
    {3, "Dr. Bilal Khan", "Orthopedics"},
    {4, "Dr. Hina Farooq", "General Surgery"},
    {5, "Dr. Usman Tariq", "Emergency"}};

int findDoctor(int id)
{
    for (int i = 0; i < MAX_DOCTORS; i++)
    {
        if (doctors[i].id == id)
            return i;
    }
    return -1;
}

void showDoctors()
{
    cout << left << setw(6) << "ID" << setw(22) << "Doctor" << "Department\n";
    for (int i = 0; i < MAX_DOCTORS; i++)
    {
        cout << left << setw(6) << doctors[i].id << setw(22) << doctors[i].name << doctors[i].department << "\n";
    }
}

struct Patient
{
    int id;
    string name;
    int age;
    string gender;
    string phone;
    string disease;
    Patient *next;

    Patient()
    {
        id = 0;
        age = 0;
        next = NULL;
    }
};

void copyPatient(Patient *to, Patient *from)
{
    to->id = from->id;
    to->name = from->name;
    to->age = from->age;
    to->gender = from->gender;
    to->phone = from->phone;
    to->disease = from->disease;
}

void swapPatients(Patient *a, Patient *b)
{
    Patient temp;
    copyPatient(&temp, a);
    copyPatient(a, b);
    copyPatient(b, &temp);
}

void showPatientHeader()
{
    cout << left << setw(8) << "ID" << setw(22) << "Name" << setw(6) << "Age" << setw(9) << "Gender"
         << setw(17) << "Phone" << "Disease\n";
}

void showPatient(Patient *p)
{
    cout << left << setw(8) << p->id << setw(22) << p->name << setw(6) << p->age << setw(9) << p->gender
         << setw(17) << p->phone << p->disease << "\n";
}

class PatientList
{
public:
    Patient *head;
    Patient *tail;
    int count;

    PatientList()
    {
        head = NULL;
        tail = NULL;
        count = 0;
    }

    bool isEmpty()
    {
        return head == NULL;
    }

    Patient *findById(int id)
    {
        Patient *temp = head;
        while (temp != NULL)
        {
            if (temp->id == id)
                return temp;
            temp = temp->next;
        }
        return NULL;
    }

    bool add(Patient p)
    {
        if (findById(p.id) != NULL)
            return false;

        Patient *newNode = new Patient;
        copyPatient(newNode, &p);

        if (head == NULL)
        {
            head = newNode;
            tail = newNode;
        }
        else
        {
            tail->next = newNode;
            tail = newNode;
        }
        count++;
        return true;
    }

    bool remove(int id, Patient &removed)
    {
        Patient *prev = NULL;
        Patient *temp = head;

        while (temp != NULL && temp->id != id)
        {
            prev = temp;
            temp = temp->next;
        }
        if (temp == NULL)
            return false;

        copyPatient(&removed, temp);

        if (prev == NULL)
            head = temp->next;
        else
            prev->next = temp->next;

        if (temp == tail)
            tail = prev;

        delete temp;
        count--;
        return true;
    }

    bool isAfter(Patient *a, Patient *b, int field)
    {
        if (field == 1)
            return a->id > b->id;
        if (field == 2)
            return toLower(a->name) > toLower(b->name);
        return a->age > b->age;
    }

    void bubbleSort(int field)
    {
        bool swapped = true;
        while (swapped)
        {
            swapped = false;
            for (Patient *p = head; p != NULL && p->next != NULL; p = p->next)
            {
                if (isAfter(p, p->next, field))
                {
                    swapPatients(p, p->next);
                    swapped = true;
                }
            }
        }
    }

    Patient *binarySearch(int id)
    {
        if (count == 0)
            return NULL;

        Patient **arr = new Patient *[count];
        int n = 0;
        for (Patient *p = head; p != NULL; p = p->next)
        {
            arr[n] = p;
            n++;
        }

        for (int i = 1; i < n; i++)
        {
            Patient *key = arr[i];
            int j = i - 1;
            while (j >= 0 && arr[j]->id > key->id)
            {
                arr[j + 1] = arr[j];
                j--;
            }
            arr[j + 1] = key;
        }

        int low = 0;
        int high = n - 1;
        Patient *result = NULL;
        while (low <= high && result == NULL)
        {
            int mid = (low + high) / 2;
            if (arr[mid]->id == id)
                result = arr[mid];
            else if (arr[mid]->id < id)
                low = mid + 1;
            else
                high = mid - 1;
        }

        delete[] arr;
        return result;
    }

    void display()
    {
        showPatientHeader();
        for (Patient *p = head; p != NULL; p = p->next)
            showPatient(p);
        cout << "Total patients: " << count << "\n";
    }
};

struct UndoNode
{
    int type;
    Patient data;
    UndoNode *next;
};

class UndoStack
{
public:
    UndoNode *top;

    UndoStack()
    {
        top = NULL;
    }

    bool isEmpty()
    {
        return top == NULL;
    }

    void push(int type, Patient *p)
    {
        UndoNode *newNode = new UndoNode;
        newNode->type = type;
        copyPatient(&newNode->data, p);
        newNode->next = top;
        top = newNode;
    }

    bool pop(int &type, Patient &data)
    {
        if (isEmpty())
            return false;

        UndoNode *temp = top;
        type = temp->type;
        copyPatient(&data, &temp->data);
        top = top->next;
        delete temp;
        return true;
    }
};

struct Request
{
    int patientId;
    int doctorId;
    string date;
    Request *next;
};

class RequestQueue
{
public:
    Request *front;
    Request *rear;
    int count;

    RequestQueue()
    {
        front = NULL;
        rear = NULL;
        count = 0;
    }

    bool isEmpty()
    {
        return front == NULL;
    }

    void enqueue(int patientId, int doctorId, string date)
    {
        Request *newNode = new Request;
        newNode->patientId = patientId;
        newNode->doctorId = doctorId;
        newNode->date = date;
        newNode->next = NULL;

        if (rear == NULL)
        {
            front = newNode;
            rear = newNode;
        }
        else
        {
            rear->next = newNode;
            rear = newNode;
        }
        count++;
    }

    bool dequeue(Request &r)
    {
        if (isEmpty())
            return false;

        Request *temp = front;
        r.patientId = temp->patientId;
        r.doctorId = temp->doctorId;
        r.date = temp->date;

        front = front->next;
        if (front == NULL)
            rear = NULL;

        delete temp;
        count--;
        return true;
    }

    void display()
    {
        if (isEmpty())
        {
            cout << "No requests are waiting.\n";
            return;
        }
        cout << left << setw(6) << "No." << setw(12) << "Patient ID" << setw(22) << "Doctor" << "Date\n";
        int position = 1;
        for (Request *r = front; r != NULL; r = r->next)
        {
            cout << left << setw(6) << position << setw(12) << r->patientId
                 << setw(22) << doctors[findDoctor(r->doctorId)].name << r->date << "\n";
            position++;
        }
    }
};

struct Appointment
{
    int id;
    int patientId;
    int doctorId;
    string date;
    int status;
    Appointment *next;
};

void swapAppointments(Appointment *a, Appointment *b)
{
    int id = a->id;
    int patientId = a->patientId;
    int doctorId = a->doctorId;
    string date = a->date;
    int status = a->status;

    a->id = b->id;
    a->patientId = b->patientId;
    a->doctorId = b->doctorId;
    a->date = b->date;
    a->status = b->status;

    b->id = id;
    b->patientId = patientId;
    b->doctorId = doctorId;
    b->date = date;
    b->status = status;
}

class AppointmentList
{
public:
    Appointment *head;
    Appointment *tail;
    int nextId;
    int count;

    AppointmentList()
    {
        head = NULL;
        tail = NULL;
        nextId = 1;
        count = 0;
    }

    bool isEmpty()
    {
        return head == NULL;
    }

    Appointment *findById(int id)
    {
        for (Appointment *a = head; a != NULL; a = a->next)
        {
            if (a->id == id)
                return a;
        }
        return NULL;
    }

    void add(int id, int patientId, int doctorId, string date, int status)
    {
        Appointment *newNode = new Appointment;
        newNode->id = id;
        newNode->patientId = patientId;
        newNode->doctorId = doctorId;
        newNode->date = date;
        newNode->status = status;
        newNode->next = NULL;

        if (head == NULL)
        {
            head = newNode;
            tail = newNode;
        }
        else
        {
            tail->next = newNode;
            tail = newNode;
        }
        count++;

        if (id >= nextId)
            nextId = id + 1;
    }

    int countScheduled(int doctorId, string date)
    {
        int total = 0;
        for (Appointment *a = head; a != NULL; a = a->next)
        {
            if (a->doctorId == doctorId && a->date == date && a->status == SCHEDULED)
                total++;
        }
        return total;
    }

    bool alreadyBooked(int patientId, int doctorId, string date)
    {
        for (Appointment *a = head; a != NULL; a = a->next)
        {
            if (a->patientId == patientId && a->doctorId == doctorId && a->date == date && a->status == SCHEDULED)
                return true;
        }
        return false;
    }

    int cancelForPatient(int patientId)
    {
        int cancelled = 0;
        for (Appointment *a = head; a != NULL; a = a->next)
        {
            if (a->patientId == patientId && a->status == SCHEDULED)
            {
                a->status = CANCELLED;
                cancelled++;
            }
        }
        return cancelled;
    }

    bool comesAfter(Appointment *a, Appointment *b)
    {
        if (a->date != b->date)
            return a->date > b->date;
        return a->id > b->id;
    }

    void bubbleSortByDate()
    {
        bool swapped = true;
        while (swapped)
        {
            swapped = false;
            for (Appointment *a = head; a != NULL && a->next != NULL; a = a->next)
            {
                if (comesAfter(a, a->next))
                {
                    swapAppointments(a, a->next);
                    swapped = true;
                }
            }
        }
    }
};

PatientList patients;
AppointmentList appointments;
RequestQueue requests;
UndoStack undoStack;

void saveAll()
{
    ofstream patientFile("patients.txt");
    for (Patient *p = patients.head; p != NULL; p = p->next)
    {
        patientFile << p->id << "|" << p->name << "|" << p->age << "|" << p->gender << "|"
                    << p->phone << "|" << p->disease << "\n";
    }

    ofstream appointmentFile("appointments.txt");
    for (Appointment *a = appointments.head; a != NULL; a = a->next)
    {
        appointmentFile << a->id << "|" << a->patientId << "|" << a->doctorId << "|"
                        << a->date << "|" << a->status << "\n";
    }

    ofstream requestFile("requests.txt");
    for (Request *r = requests.front; r != NULL; r = r->next)
    {
        requestFile << r->patientId << "|" << r->doctorId << "|" << r->date << "\n";
    }
}

void loadPatients()
{
    ifstream file("patients.txt");
    string line;
    while (getline(file, line))
    {
        string parts[6];
        if (splitLine(trim(line), parts, 6) != 6)
            continue;
        if (!isNumber(parts[0]) || !isNumber(parts[2]))
            continue;

        Patient p;
        p.id = atoi(parts[0].c_str());
        p.name = parts[1];
        p.age = atoi(parts[2].c_str());
        p.gender = parts[3];
        p.phone = parts[4];
        p.disease = parts[5];
        patients.add(p);
    }
}

void loadAppointments()
{
    ifstream file("appointments.txt");
    string line;
    while (getline(file, line))
    {
        string parts[5];
        if (splitLine(trim(line), parts, 5) != 5)
            continue;
        if (!isNumber(parts[0]) || !isNumber(parts[1]) || !isNumber(parts[2]) || !isNumber(parts[4]))
            continue;

        int doctorId = atoi(parts[2].c_str());
        int status = atoi(parts[4].c_str());
        if (findDoctor(doctorId) == -1 || !validDate(parts[3]) || status < SCHEDULED || status > CANCELLED)
            continue;

        appointments.add(atoi(parts[0].c_str()), atoi(parts[1].c_str()), doctorId, parts[3], status);
    }
}

void loadRequests()
{
    ifstream file("requests.txt");
    string line;
    while (getline(file, line))
    {
        string parts[3];
        if (splitLine(trim(line), parts, 3) != 3)
            continue;
        if (!isNumber(parts[0]) || !isNumber(parts[1]) || !validDate(parts[2]))
            continue;

        int doctorId = atoi(parts[1].c_str());
        if (findDoctor(doctorId) == -1)
            continue;

        requests.enqueue(atoi(parts[0].c_str()), doctorId, parts[2]);
    }
}

void registerPatient()
{
    Patient p;
    p.id = readInt("Patient ID: ", 1, 999999);
    if (patients.findById(p.id) != NULL)
    {
        cout << "A patient with ID " << p.id << " already exists. Registration cancelled.\n";
        return;
    }

    p.name = readText("Name: ");
    p.age = readInt("Age (0-120): ", 0, 120);
    int gender = readInt("Gender (1 = Male, 2 = Female): ", 1, 2);
    p.gender = gender == 1 ? "Male" : "Female";
    p.phone = readPhone("Phone: ");
    p.disease = readText("Disease / reason for visit: ");

    patients.add(p);
    undoStack.push(ACTION_ADD, &p);
    saveAll();
    cout << "Patient " << p.name << " registered.\n";
}

void updatePatient()
{
    if (patients.isEmpty())
    {
        cout << "No patients registered.\n";
        return;
    }

    int id = readInt("Patient ID to update: ", 1, 999999);
    Patient *p = patients.findById(id);
    if (p == NULL)
    {
        cout << "Patient not found.\n";
        return;
    }

    showPatientHeader();
    showPatient(p);
    cout << "1. Name\n2. Age\n3. Gender\n4. Phone\n5. Disease\n0. Cancel\n";
    int choice = readInt("Field to update: ", 0, 5);
    if (choice == 0)
        return;

    Patient before;
    copyPatient(&before, p);

    if (choice == 1)
        p->name = readText("New name: ");
    else if (choice == 2)
        p->age = readInt("New age (0-120): ", 0, 120);
    else if (choice == 3)
        p->gender = readInt("New gender (1 = Male, 2 = Female): ", 1, 2) == 1 ? "Male" : "Female";
    else if (choice == 4)
        p->phone = readPhone("New phone: ");
    else
        p->disease = readText("New disease: ");

    undoStack.push(ACTION_UPDATE, &before);
    saveAll();
    cout << "Record updated.\n";
}

void deletePatient()
{
    if (patients.isEmpty())
    {
        cout << "No patients registered.\n";
        return;
    }

    int id = readInt("Patient ID to delete: ", 1, 999999);
    Patient removed;
    if (!patients.remove(id, removed))
    {
        cout << "Patient not found.\n";
        return;
    }

    int cancelled = appointments.cancelForPatient(id);
    undoStack.push(ACTION_DELETE, &removed);
    saveAll();

    cout << "Patient " << removed.name << " deleted.";
    if (cancelled > 0)
        cout << " " << cancelled << " scheduled appointment(s) cancelled.";
    cout << "\n";
}

void searchById()
{
    int id = readInt("Patient ID: ", 1, 999999);
    Patient *p = patients.binarySearch(id);
    if (p == NULL)
    {
        cout << "Patient not found.\n";
        return;
    }
    showPatientHeader();
    showPatient(p);
}

void searchByName()
{
    string key = readText("Name (full or part): ");
    bool found = false;

    for (Patient *p = patients.head; p != NULL; p = p->next)
    {
        if (toLower(p->name).find(toLower(key)) != string::npos)
        {
            if (!found)
                showPatientHeader();
            showPatient(p);
            found = true;
        }
    }

    if (!found)
        cout << "No patient matches \"" << key << "\".\n";
}

void searchPatient()
{
    if (patients.isEmpty())
    {
        cout << "No patients registered.\n";
        return;
    }

    cout << "1. Search by ID\n2. Search by name\n0. Back\n";
    int choice = readInt("Choice: ", 0, 2);
    if (choice == 1)
        searchById();
    else if (choice == 2)
        searchByName();
}

void sortPatients()
{
    if (patients.isEmpty())
    {
        cout << "No patients registered.\n";
        return;
    }

    cout << "1. By ID\n2. By name\n3. By age\n0. Back\n";
    int choice = readInt("Sort by: ", 0, 3);
    if (choice == 0)
        return;

    patients.bubbleSort(choice);
    saveAll();
    patients.display();
}

void undoLast()
{
    int type;
    Patient data;
    if (!undoStack.pop(type, data))
    {
        cout << "Nothing to undo.\n";
        return;
    }

    if (type == ACTION_ADD)
    {
        Patient removed;
        patients.remove(data.id, removed);
        appointments.cancelForPatient(data.id);
        cout << "Undo: registration of " << data.name << " reverted.\n";
    }
    else if (type == ACTION_UPDATE)
    {
        Patient *p = patients.findById(data.id);
        if (p != NULL)
            copyPatient(p, &data);
        cout << "Undo: previous record of " << data.name << " restored.\n";
    }
    else
    {
        patients.add(data);
        cout << "Undo: patient " << data.name << " restored.\n";
    }
    saveAll();
}

void patientMenu()
{
    while (true)
    {
        cout << "\n--- Patient Management ---\n";
        cout << "1. Register patient\n2. Update patient\n3. Delete patient\n4. Search patient\n";
        cout << "5. Display all patients\n6. Sort patients\n7. Undo last change\n0. Back\n";
        int choice = readInt("Choice: ", 0, 7);

        if (choice == 0)
            return;
        else if (choice == 1)
            registerPatient();
        else if (choice == 2)
            updatePatient();
        else if (choice == 3)
            deletePatient();
        else if (choice == 4)
            searchPatient();
        else if (choice == 5)
        {
            if (patients.isEmpty())
                cout << "No patients registered.\n";
            else
                patients.display();
        }
        else if (choice == 6)
            sortPatients();
        else
            undoLast();
    }
}

void showAppointments(int filter)
{
    bool found = false;
    for (Appointment *a = appointments.head; a != NULL; a = a->next)
    {
        if (filter != 0 && a->status != filter)
            continue;

        if (!found)
        {
            cout << left << setw(6) << "ID" << setw(26) << "Patient" << setw(22) << "Doctor"
                 << setw(13) << "Date" << "Status\n";
        }
        found = true;

        Patient *p = patients.findById(a->patientId);
        string patientName = p != NULL ? p->name : "(removed)";
        cout << left << setw(6) << a->id << setw(26) << patientName
             << setw(22) << doctors[findDoctor(a->doctorId)].name
             << setw(13) << a->date << statusName(a->status) << "\n";
    }

    if (!found)
        cout << "No appointments to show.\n";
}

void requestAppointment()
{
    int patientId = readInt("Patient ID: ", 1, 999999);
    if (patients.findById(patientId) == NULL)
    {
        cout << "Patient not found. Register the patient first.\n";
        return;
    }

    showDoctors();
    int doctorId = readInt("Doctor ID: ", 1, MAX_DOCTORS);
    string date = readDate("Date (YYYY-MM-DD): ");

    requests.enqueue(patientId, doctorId, date);
    saveAll();
    cout << "Request added to the queue. Position: " << requests.count << "\n";
}

void processRequest()
{
    Request r;
    if (!requests.dequeue(r))
    {
        cout << "No requests are waiting.\n";
        return;
    }

    Patient *p = patients.findById(r.patientId);
    string doctorName = doctors[findDoctor(r.doctorId)].name;

    if (p == NULL)
    {
        cout << "Request rejected: patient " << r.patientId << " no longer exists.\n";
    }
    else if (appointments.alreadyBooked(r.patientId, r.doctorId, r.date))
    {
        cout << "Request rejected: " << p->name << " already has an appointment with "
             << doctorName << " on " << r.date << ".\n";
    }
    else if (appointments.countScheduled(r.doctorId, r.date) >= MAX_PER_DAY)
    {
        cout << "Request rejected: " << doctorName << " is fully booked on " << r.date << ".\n";
    }
    else
    {
        int id = appointments.nextId;
        appointments.add(id, r.patientId, r.doctorId, r.date, SCHEDULED);
        cout << "Appointment " << id << " scheduled for " << p->name << " with "
             << doctorName << " on " << r.date << ".\n";
    }
    saveAll();
}

void changeStatus(int newStatus)
{
    if (appointments.isEmpty())
    {
        cout << "No appointments exist.\n";
        return;
    }

    int id = readInt("Appointment ID: ", 1, 999999);
    Appointment *a = appointments.findById(id);

    if (a == NULL)
        cout << "Appointment " << id << " does not exist.\n";
    else if (a->status == COMPLETED)
        cout << "Appointment " << id << " is already completed.\n";
    else if (a->status == CANCELLED)
        cout << "Appointment " << id << " is already cancelled.\n";
    else
    {
        a->status = newStatus;
        saveAll();
        if (newStatus == COMPLETED)
            cout << "Appointment " << id << " marked as completed.\n";
        else
            cout << "Appointment " << id << " cancelled.\n";
    }
}

void appointmentMenu()
{
    while (true)
    {
        cout << "\n--- Appointments ---\n";
        cout << "1. Request appointment\n2. Process next request\n3. Show waiting requests\n";
        cout << "4. Show all appointments\n5. Mark appointment completed\n6. Cancel appointment\n";
        cout << "7. Show appointments by status\n8. Sort appointments by date\n9. Show doctors\n0. Back\n";
        int choice = readInt("Choice: ", 0, 9);

        if (choice == 0)
            return;
        else if (choice == 1)
            requestAppointment();
        else if (choice == 2)
            processRequest();
        else if (choice == 3)
            requests.display();
        else if (choice == 4)
            showAppointments(0);
        else if (choice == 5)
            changeStatus(COMPLETED);
        else if (choice == 6)
            changeStatus(CANCELLED);
        else if (choice == 7)
        {
            cout << "1. Scheduled\n2. Completed\n3. Cancelled\n";
            showAppointments(readInt("Status: ", 1, 3));
        }
        else if (choice == 8)
        {
            appointments.bubbleSortByDate();
            saveAll();
            showAppointments(0);
        }
        else
            showDoctors();
    }
}

int main()
{
    loadPatients();
    loadAppointments();
    loadRequests();

    cout << "Hospital Appointment and Patient Management System\n";
    cout << "Loaded " << patients.count << " patients, " << appointments.count << " appointments and "
         << requests.count << " waiting requests from files.\n";

    while (true)
    {
        cout << "\n===== Main Menu =====\n";
        cout << "1. Patient management\n2. Appointments\n0. Save and exit\n";
        int choice = readInt("Choice: ", 0, 2);

        if (choice == 0)
        {
            saveAll();
            cout << "Data saved. Goodbye.\n";
            return 0;
        }
        else if (choice == 1)
            patientMenu();
        else
            appointmentMenu();
    }
}
