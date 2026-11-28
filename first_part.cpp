#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <algorithm>
#include <iomanip>
#include <limits>
using namespace std;

struct Patient { int id, age; string name, contact, address; };
struct Test { int id, dept, hours; string name, sample; double price; };
struct Request { int id, patient; vector<int> tests; string date, provider, status; };
struct Sample { int id, request, test; string date, time, condition, section, status; };
struct Result { int sample; string value, unit, reference; vector<string> history; };
struct Report { int request; string date, status; };
struct Payment { int request; double total, paid; };

string ask(string prompt) {
    cout << prompt;
    string value;
    getline(cin, value);
    if (value.empty()) throw runtime_error("This field cannot be empty.");
    return value;
}

int number(string text, int minimum = 0) {
    try {
        int value = stoi(text);
        if (value < minimum) throw runtime_error("Number is outside the allowed range.");
        return value;
    } catch (...) {
        throw runtime_error("Enter a valid whole number.");
    }
}

double money(string text) {
    try {
        double value = stod(text);
        if (value < 0) throw runtime_error("Amount cannot be negative.");
        return value;
    } catch (...) {
        throw runtime_error("Enter a valid amount.");
    }
}

template<class T>
T* findId(vector<T>& items, int id) {
    for (auto& item : items)
        if (item.id == id) return &item;
    return nullptr;
}

class LabSystem {
    vector<Patient> patients;
    vector<Test> tests;
    vector<Request> requests;
    vector<Sample> samples;
    vector<Result> results;
    vector<Report> reports;
    vector<Payment> payments;

    int nextPatient = 1, nextTest = 1, nextRequest = 1, nextSample = 1;

    template<class T>
    static void put(ofstream& file, const T& value) {
        file.write(reinterpret_cast<const char*>(&value), sizeof(value));
    }

    static void putString(ofstream& file, const string& value) {
        size_t length = value.size();
        put(file, length);
        file.write(value.data(), length);
    }

    template<class T>
    static void get(ifstream& file, T& value) {
        file.read(reinterpret_cast<char*>(&value), sizeof(value));
    }

    static void getString(ifstream& file, string& value) {
        size_t length;
        get(file, length);
        value.resize(length);
        file.read(&value[0], length);
    }

    Request* findRequest(int id) {
        for (auto& request : requests)
            if (request.id == id) return &request;
        return nullptr;
    }

    Sample* findSample(int id) {
        for (auto& sample : samples)
            if (sample.id == id) return &sample;
        return nullptr;
    }

    void save() {
        ofstream file("lab_data.txt");
        if (!file) throw runtime_error("Cannot write lab_data.txt");

        file << nextPatient << ' ' << nextTest << ' '
             << nextRequest << ' ' << nextSample << '\n';

        file << patients.size() << '\n';
        for (auto& p : patients)
            file << p.id << ' ' << p.age << ' ' << quoted(p.name) << ' '
                 << quoted(p.contact) << ' ' << quoted(p.address) << '\n';

        file << tests.size() << '\n';
        for (auto& t : tests)
            file << t.id << ' ' << t.dept << ' ' << t.hours << ' '
                 << setprecision(17) << t.price << ' '
                 << quoted(t.name) << ' ' << quoted(t.sample) << '\n';

        file << requests.size() << '\n';
        for (auto& r : requests) {
            file << r.id << ' ' << r.patient << ' ' << quoted(r.date) << ' '
                 << quoted(r.provider) << ' ' << quoted(r.status) << ' '
                 << r.tests.size();
            for (int id : r.tests) file << ' ' << id;
            file << '\n';
        }
