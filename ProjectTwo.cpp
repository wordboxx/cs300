// --- IMPORTS
#include <iostream>
#include <fstream>
#include <sstream>
#include <map>
#include <vector>
#include <algorithm>
#include <filesystem>
#include <limits>

// --- NAMESPACES
using namespace std;
namespace fs = std::filesystem;

// --- STRUCTS
// Defines structure to hold course information
struct Course {
    string courseNumber;
    string courseTitle;
    vector<string> prerequisites;
};

map<string, Course> courses;

// --- FUNCTIONS
string trim(const string& str) {
    // Sanitizes and trims user input string just in case
    size_t start = str.find_first_not_of(" \t\r\n");
    size_t end = str.find_last_not_of(" \t\r\n");
    return (start == string::npos || end == string::npos) ? "" : str.substr(start, end - start + 1);
}

void loadCoursesFromFile(const string& filename) {
    // Loads courses from CSV file into global map

    // ifstream object
    ifstream file(filename);

    // File open error handling
    if (!file.is_open()) {
        cout << "Error: Unable to open file " << filename << endl;
        return;
    }

    courses.clear();
    string line;

    // Loop through file lines
    while (getline(file, line)) {
        stringstream ss(line);
        string token;
        vector<string> tokens;

        // Read and split line by commas into tokens
        while (getline(ss, token, ',')) {
            tokens.push_back(trim(token));
        }

        // Single token is invalid; skip
        if (tokens.size() < 2) {
            continue;
        }

        // Create Course object
        Course course;
        // Populate course number and title
        course.courseNumber = tokens[0];
        course.courseTitle = tokens[1];
        // Populate with prereqs
        for (int i = 2; i < tokens.size(); ++i) {
            course.prerequisites.push_back(tokens[i]);
        }

        // Add course to map
        courses[course.courseNumber] = course;
    }

    // Close file
    file.close();
}

// Print entire course list
void printCourseList() {
    for (const auto& pair : courses) {
        cout << pair.second.courseNumber << ", " << pair.second.courseTitle << endl;
    }
    cout << endl;
}

// Print specific course details
void printCourseDetails(const Course& course) {
    cout << course.courseNumber << ", " << course.courseTitle << endl;

    // Print prereqs if they exist
    if (!course.prerequisites.empty()) {
        cout << "Prerequisites: ";
        for (int i = 0; i < course.prerequisites.size(); ++i) {
            cout << course.prerequisites[i];
            if (i < course.prerequisites.size() - 1) {
                cout << ", ";
            }
        }
        cout << endl;
    }
    cout << endl;
}

// Menu function
void printMenu(const vector<string>& list) {
    // Takes list of strings to print as menu options, rather than hardcoding
    cout << "Course Planner" << endl;
    for (int i = 0; i < list.size(); ++i) {
        cout << i + 1 << ". " << list[i] << endl;
    }
    cout << "9. Exit" << endl;
}

// Helper to get validated integer input
int getValidatedInteger(string prompt, int min, int max) {
    // Declare input variable
    int input;

    // Loop until valid input is received
    while (true) {
        cout << prompt;
        cin >> input;
        // If the user's input is invalid, clear the cin and loop around
        if (cin.fail() || input < min || input > max) {
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            cout << "Invalid input. Only " << min << " and " << max << " are allowed!" << endl;
        } else {
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            return input;
        }
    }
}

// menu functions
// Loads data from CSV to map
void actionLoadData(bool& dataLoaded) {
    // First check if data is already loaded
    if (dataLoaded) {
        cout << "Already loaded this file!" << endl;
        return;
    }

    // Prepare vector of CSV files in same directory as executable
    vector<string> csvFiles;
    cout << endl;
    cout << "Available CSV files:" << endl;
    // We iterate through each file in the directory...
    int index = 1;
    for (const auto& entry : fs::directory_iterator(fs::current_path())) {
        // ... making sure they're CSVs
        if (entry.path().extension() == ".csv") {
            // If so, we print the filename and add to vector from earlier
            cout << index++ << ". " << entry.path().filename() << endl;
            csvFiles.push_back(entry.path().string());
        }
    }

    // On the off chance there's no CSVs, we cout the user a message
    if (csvFiles.empty()) {
        cout << "No CSV files found in the current directory." << endl;
        return;
    }

    // If there are CSVs, the user selects one to load
    try {
        int selection = getValidatedInteger("Select a file to load (enter number): ", 1, csvFiles.size());
        loadCoursesFromFile(csvFiles[selection - 1]);
        dataLoaded = true;
        cout << "Loaded successfully from " << csvFiles[selection - 1] << endl;
        // On the off chance the file somehow doesn't load
    } catch (const exception& e) {
        cout << "Something went wrong loading the file: " << e.what() << endl;
        dataLoaded = false;
    }
}

// Redundant function to print course list
void actionPrintCourseList(bool dataLoaded) {
    if (!dataLoaded) {
        cout << "Gotta load data first!" << endl;
    } else {
        printCourseList();
    }
}

// Prints a specific course from user's input
void actionPrintCourse(bool dataLoaded) {
    if (!dataLoaded) {
        cout << "Gotta load data first!" << endl;
    } else {
        // Populate vector with course keys
        vector<string> courseKeys;
        int idx = 0;
        cout << "Available Courses:" << endl;
        // Iterates through course mape and prints key-value pairs
        for (const auto& pair : courses) {
            cout << idx << ". " << pair.first << " - " << pair.second.courseTitle << endl;
            // Populates courseKeys vector with course numbers
            courseKeys.push_back(pair.first);
            ++idx;
        }
        
        // If there's no courses, we return
        if (courseKeys.empty()) {
            cout << "No courses available." << endl;
            return;
        }

        // User selects course to view (defaults int to -1 for error checking)
        int sel = -1;
        while (true) {
            sel = getValidatedInteger("Select a course to view: ", 0, static_cast<int>(courseKeys.size()) - 1);
            // Casts sel to int so we can make sure it's in range
            if (sel >= 0 && sel < static_cast<int>(courseKeys.size())) {
                break;
            } else {
                cout << "Invalid option. Please select a number between 0 and " << (courseKeys.size() - 1) << "." << endl;
            }
        }

        // Prints course details
        printCourseDetails(courses[courseKeys[sel]]);
    }
}

// --- MAIN START
int main() {
    int choice = 0;
    bool dataLoaded = false;
    vector<string> menuOptions = {
        "Load Data Structure",
        "Print Course List",
        "Print Course",
    };

    // Loops until user opts to exit
    int minMenu = 1;
    int maxMenu = menuOptions.size();
    while (choice != 9) {

        // Prints menu options vector
        printMenu(menuOptions);
        while (true) {
            choice = getValidatedInteger("What would you like to do? ", minMenu, 9);
            if ((choice >= minMenu && choice <= maxMenu) || choice == 9) {
                break;
            } else {
                cout << "Invalid option. Please select ";
                for (int i = minMenu; i <= maxMenu; ++i) {
                    cout << i;
                    if (i < maxMenu) cout << ", ";
                }

                // Tacks on the exit option
                cout << ", or 9." << endl;
            }
        }

        // Switch-case for menu options
        switch (choice) {
            case 1:
                actionLoadData(dataLoaded);
                break;
            case 2:
                actionPrintCourseList(dataLoaded);
                break;
            case 3:
                actionPrintCourse(dataLoaded);
                break;
            case 9:
                cout << "Exiting..." << endl;
                break;
        }
    }

    // Exit
    return 0;
}
