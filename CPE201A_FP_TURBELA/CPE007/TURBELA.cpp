#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
using namespace std;

int main() {
    ifstream file("textfile.txt");
    string line;
    int lineNum = 0;

    while (getline(file, line)) {
        lineNum++;
        double num;
        istringstream check(line);

        if (check >> num) {
            cout << "Number: " << num << endl;
        } else {
            cout << "Skipped line " << lineNum << ": \"" << line << "\" (not a number)" << endl;
        }
    }

    file.close();
    return 0;

}
