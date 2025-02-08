#include <iostream>
#include <string>

#ifdef _WIN32
    #include <windows.h>
    #include <Lmcons.h>
#else
    #include <unistd.h>
    #include <pwd.h>
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
    return getUser() + ", hello in Byterush!";
}

int main(int argc, char* argv[]) {
    string command = arcv[1];

    cout << opening() << endl;

    cout << argv[1] << endl;
    return 0;
}