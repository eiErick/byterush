#include <iostream>
#include <string>
#include <filesystem>
#include <map>
#include <functional>
#include <fstream>

#ifdef _WIN32
    #include <windows.h>
    #include <Lmcons.h>
#else
    #include <unistd.h>
    #include <pwd.h>
    #include <cstdlib>
#endif

using namespace std;

string getUser() {
    #ifdef _WIN32
        char username[UNLEN + 1];
        DWORD username_len = UNLEN + 1;
        if (GetUserName(username, &username_len)) {
            return string(username);
        }
    #else
        uid_t uid = geteuid();
        struct passwd *pw = getpwuid(uid);
        if (pw) {
            return string(pw->pw_name);
        }
    #endif

    return "Unknown";
}

string opening() {
    return getUser() + ", hello in Byterush!\n";
}

void help() {
    cout << opening() << endl;
    cout << "add - add current path for backup" << endl;
    cout << "list - list all saved paths" << endl;
    cout << "delete - delete a path with index" << endl;
}

void writePathToFile(const string &currentPath, const string &filePath) {
    ofstream file(filePath, ios::app);

    if (file) {
        file << currentPath << endl;
        cout << "Path " << currentPath << " has been added successfully in " << filePath << endl;
    } else {
        cerr << "Error in adding " << currentPath << endl;
    }
}

void addPath(string currentPath, string filePath) {
    filesystem::path filePathObj = filePath;
    ofstream file(filePathObj, ios::app);

    if (!filesystem::exists(filePathObj)) {
        filesystem::path dirParent = filePathObj.parent_path();

        if (!filesystem::exists(dirParent)) {
            if (filesystem::create_directories(dirParent)) {
                cout << "Created the byterush directory!\n";
            } else {
                cerr << "Failed to create directory :/\n";
            }
        }

        ofstream file(filePath);
        if (file) {
            cout << "file created!" << endl;
        } else {
            cerr << "Failed to create directory :/" << endl;
        }
    }

    writePathToFile(currentPath, filePath);
}

void savePaths(vector<string> paths) {
    string home = getenv("HOME");
    filesystem::path dataBackup = home + "/.local/share/byterush/paths.conf";

    ofstream deleteFileBackup(dataBackup, ios::trunc);
    deleteFileBackup.close();

    ofstream fileBackup(dataBackup, ios::app);

    for (string line : paths) {
        fileBackup << line << "\n";
    }

    fileBackup.close();
}

vector<string> getPaths() {
    vector<string> lines;

    string home = getenv("HOME");
    ifstream backupPaths(home + "/.local/share/byterush/paths.conf");
    
    string line;

    while(getline(backupPaths, line)) {
        lines.push_back(line);
    }

    return lines;
}

void listPaths() {
    vector<string> lines = getPaths();

    cout << "Listing paths:\n\n";

    int index = 1;
    for (string line : lines) {
        cout << index << " - " << line << endl;
        index++;
    }
}

void deletePath() {
    int index;
    char confirm;

    cout << "Enter the path index: ";
    cin >> index;
    index--;

    vector<string> paths = getPaths();
    
    if (index + 1 > paths.size() && index < 1) {
        cerr << "Invalid index :/" << endl;
    } else {
        string path = paths[index];

        cout << "Do you want delete? " << path << " [y/n] ";
        cin >> confirm;
        
        if (confirm == 'y') {
            paths.erase(paths.begin() + index);
            savePaths(paths);
            cout << "Path deleted" << endl;
        }
    }
}

int main(int argc, char* argv[]) {
    string currentPath = filesystem::current_path();

    string home = getenv("HOME");
    string dirConf = "/.local/share/byterush";

    string pathsConf = home + dirConf + "/paths.conf";
    
    map<string, function<void()>> commands = {
        {"help", help},
        {"add", [currentPath, pathsConf]() { addPath(currentPath, pathsConf); }},
        {"list", listPaths},
        {"delete", deletePath},
    };

    string command = argv[1];

    if (commands.count(command)) {
        commands[command]();
    } else {
        help();
    }

    return 0;
}
