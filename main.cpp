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

vector<string> getPaths(const string &filePath) {
    ifstream paths(filePath);

    vector<string> lines;
    string line;

    while(getline(paths, line)) {
        lines.push_back(line);
    }

    return lines;
}

bool validatePath(const string &currentPath, const string &filePath) {
    vector<string> paths = getPaths(filePath);

    for (string line : paths) {
        if (line == currentPath) {
            return false;
        }
    }

    return true;
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

void addPath(const string &currentPath, const string &filePath) {
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

    if (validatePath(currentPath, filePath)) {
        writePathToFile(currentPath, filePath);
    } else {
        cout << "This path is already stored! :)" << endl;
    }
}

void savePaths(const string &filePath, const vector<string> &paths) {
    filesystem::path filePathObj = filePath;

    ofstream deleteFileBackup(filePathObj, ios::trunc);
    deleteFileBackup.close();

    ofstream file(filePathObj, ios::app);

    for (string line : paths) {
        file << line << "\n";
    }
}

void listPaths(const string &filePath) {
    vector<string> lines = getPaths(filePath);

    cout << "Listing paths:\n\n";

    int index = 1;
    for (string line : lines) {
        cout << index << " - " << line << endl;
        index++;
    }
}

void deletePath(const string &filePath) {
    int index;
    char confirm;

    cout << "Enter the path index: ";
    cin >> index;
    index--;

    vector<string> paths = getPaths(filePath);
    
    if (index + 1 > paths.size() && index < 1) {
        cerr << "Invalid index :/" << endl;
    } else {
        string path = paths[index];

        cout << "Do you want delete? " << path << " [y/n] ";
        cin >> confirm;
        
        if (confirm == 'y') {
            paths.erase(paths.begin() + index);
            savePaths(filePath, paths);
            cout << "Path deleted" << endl;
        }
    }
}

int main(int argc, char* argv[]) {
    string currentPath = filesystem::current_path();

    string home = getenv("HOME");
    string dirConf = "/.local/share/byterush";

    string pathsConf = home + dirConf + "/paths.conf";
    string locationsConf = home + dirConf + "/locations.conf";
    
    map<string, function<void()>> commands = {
        {"help", help},
        {"add", [currentPath, pathsConf] { addPath(currentPath, pathsConf); }},
        {"list", [pathsConf] { listPaths(pathsConf); }},
        {"delete", [pathsConf] { deletePath(pathsConf); }},
        {"location", [currentPath, locationsConf] { addPath(currentPath, locationsConf); }}
    };

    string command = argv[1];

    if (commands.count(command)) {
        commands[command]();
    } else {
        help();
    }

    return 0;
}
