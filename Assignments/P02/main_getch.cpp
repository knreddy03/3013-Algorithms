/******************************************************************************
 *
 *  Author:           Narasimha Reddy Koduri
 *  Email:            nkoduri0503@gmail.com
 *  Label:            P02
 *  Title:            Substring-Based Word Search in JSON using GETCH
 *  Course:           CMPS 3013
 *  Semester:         Spring Semester 2024
 *
 *  Description:
 *        This program uses a "getch" function to get a single character from
 *        user input without displaying it on the terminal. It allows users to
 *        type characters to search for words in a JSON file, where a partial
 *        match results in the display of matching words.
 *
 *  Usage:
 *        $ ./program <json_file_path>
 *        - json_file_path: Path to the JSON file containing dictionary words.
 *
 *  Files:
 *        - main.cpp            : Main program source code
 *        - headers/json.hpp    : Header for JSON operations
 *        - headers/mygetch.hpp : Header for `getch` function
 *        - headers/timer.hpp   : Header for timing operations
 *        - headers/rang.hpp    : Header for colored console output
 *        - data/dictionary.json: JSON file containing dictionary words
 *
 *******************************************************************************/

#include "./headers/json.hpp"
#include "./headers/mygetch.hpp"
#include "./headers/timer.hpp"
#include "./headers/rang.hpp"
#include <algorithm>
#include <chrono>
#include <fstream>
#include <iostream>
#include <thread>
#include <time.h>
#include <vector>

#ifdef _WIN32
#include <windows.h>
#else
#include <unistd.h>
#endif

#define DEBUG false

using namespace std;
using namespace rang;
using json = nlohmann::json;

void clearScreen()
{
#ifdef _WIN32
    // Windows-specific code
    HANDLE hStdOut = GetStdHandle(STD_OUTPUT_HANDLE);
    COORD coord = {0, 0};
    DWORD count;

    CONSOLE_SCREEN_BUFFER_INFO csbi;
    GetConsoleScreenBufferInfo(hStdOut, &csbi);

    FillConsoleOutputCharacter(hStdOut, ' ', csbi.dwSize.X * csbi.dwSize.Y, coord, &count);
    SetConsoleCursorPosition(hStdOut, coord);
#else
    // ANSI escape code for Unix-like systems
    std::cout << "\033[2J\033[1;1H";
#endif
}

// Used but could be used better
void debug(string s = "", string end = "\n")
{
    if (!DEBUG)
    {
        return;
    }
    static int call = 1;
    cout << "Debug " << call << ":" << s << end;
    call++;
}

// Not Used
void print(string s, string end = "\n")
{
    cout << s << end;
}

// Not Used
void sleep_for_milliseconds(int milliseconds)
{
    std::this_thread::sleep_for(std::chrono::milliseconds(milliseconds));
}

// /**
//  * countLines
//  *
//  * Description:
//  *      Originally wrote this to count size of input file so
//  *      I could allocate enough memory for an array of strings.
//  *      Even though I went with a vector below, this isn't totally
//  *      useless.
//  * Params:
//  *      string file_name - file to get the line count
//  *
//  * Returns:
//  *      int - line count
//  */
// int countLines(string file_name) {
//     ifstream inFile(file_name);
//     return count(istreambuf_iterator<char>(inFile), istreambuf_iterator<char>(), '\n');
// }

/**
 * loadData
 *
 * Description:
 *      Loads a file of strings (words, names, whatever) reading them in
 *      with one word per line. So words must be delimited by newlines '\n'
 * Params:
 *      string file_name - file to get the line count
 *
 * Returns:
 *      int - line count
 */

vector<string> loadDataFromJSON(string file_name)
{
    debug("loadfile");
    ifstream fin(file_name); // Open JSON file
    json data;               // JSON object to hold data

    // Read JSON data from file
    fin >> data;

    // Extract keys from JSON object
    vector<string> keys;
    for (json::iterator it = data.begin(); it != data.end(); ++it)
    {
        keys.push_back(it.key());
    }

    return keys;
}

/**
 * partialMatch
 *
 * Description:
 *      Finds partial matches in an array of strings and returns them. It
 *      doesn't matter where in the string the match is.
 * Params:
 *      vector<string>  array       - array to search
 *      string          substring   - substring to search for in each word
 *
 * Returns:
 *      vector<string> - holding all the matches to substring
 */
vector<string> partialMatch(const vector<string> &keys, const string &substring)
{
    vector<string> matches; // to hold any matches

    if (substring.size() == 0)
    {
        return matches;
    }

    for (int i = 0; i < keys.size(); i++)
    { // loop through array
        // Check if the beginning of the string matches the substring
        if (keys[i].substr(0, substring.size()) == substring)
        {
            matches.push_back(keys[i]); // add to matches
        }
    }

    return matches;
}

void titleBar(string title, int length = 0)
{
    string line = string(length, ' ');
    cout << style::underline << fg::cyan << title << style::reset << line << endl;
}

int main()
{

    char k; // holder for character being typed
    string key;
    string letter = "";     // var to concatenate letters to
    vector<string> words;   // array of animal names
    vector<string> matches; // any matches found in vector of animals
    int loc;                // location of substring to change its color
    string resultsLabel = "RESULTS:";
    string line;
    string pbp;
    bool deleting = false;
    string temp;

    // auto sclr = system("clear");

    Timer T;   // create a timer
    T.Start(); // start it

    debug();

    words = loadDataFromJSON("./data/dictionary.json");

    T.End(); // end the current timer

    // print out how long it took to load the animals file
    debug();
    // While capital Z is not typed keep looping

    while ((k = getch()) != 'Z')
    {
        debug();
        clearScreen();
        T.Start(); // start it
        // Tests for a backspace and if pressed deletes
        // last letter from "word".
        if ((int)k == 127)
        {
            if (letter.size() > 0)
            {
                letter = letter.substr(0, letter.size() - 1);
                deleting = true;
            }
        }
        else
        {
            deleting = false;
            // Make sure a letter was pressed and only letter
            if (!isalpha(k))
            {
                cout << bgB::red << fgB::gray << "Letters only!" << fg::reset << bg::reset << endl;
                sleep(1);
                continue;
            }

            // We know its a letter, lets make sure its lowercase.
            // Any letter with ascii value < 97 is capital so we
            // lower it.
            if ((int)k < 97)
            {
                k += 32;
            }
            letter += k; // append char to word
        }

        // Find any animals in the array that partially match
        // our substr word
        matches = partialMatch(words, letter);

        if ((int)k != 32)
        { // if k is not a space print it

            key = to_string(k);
            T.End();
            // Getting the correct precision to print out in c++ is not
            // straight forward. Strings and precision output are convoluted
            // at best. And drive you crazy at worst

            // pbp = "Play By Play";

            if (int(k) == 127)
            {
                cout << "del";
            }
            cout << fg::reset << style::reset << fg::red << letter << fg::reset << endl;
            cout << endl;
            cout << fg::green;

            for (auto &c : letter)
                c = tolower(c);
            // This prints out all found matches
            int count = 0;
            for (int i = 0; i < matches.size(); i++)
            {
                // find the substring in the word
                loc = matches[i].find(letter);
                // if its found
                if (loc != string::npos)
                {
                    // print one letter at a time turning on red or green
                    //  depending on if the matching subtring is being printed
                    for (int j = 0; j < matches[i].size(); j++)
                    {
                        // if we are printing the substring turn it red
                        if (j >= loc && j <= loc + letter.size() - 1)
                        {
                            cout << fg::red;
                        }
                        else
                        {
                            cout << fg::blue;
                        }
                        cout << matches[i][j];
                    }
                    cout << fg::blue;
                    count += 1;
                }
                cout << " ";
            }
            cout << fg::reset << endl
                 << endl;
            cout << fg::yellow << count << fg::green << " words found in " << fg::reset << style::reset << fg::yellow << printf("%.4f", (double)T.NanoSeconds() / 1000000000)
                 << fg::reset << fg::green << " seconds" << endl;
        }
    }
    return 0;
}
