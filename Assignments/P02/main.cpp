/******************************************************************************
 *
 *  Author:           Narasimha Reddy Koduri
 *  Email:            nkoduri0503@gmail.com
 *  Label:            P02
 *  Title:            Substring-Based Word Search in JSON
 *  Course:           CMPS 3013
 *  Semester:         Spring Semester 2024
 *
 *  Description:
 *        This program reads a JSON file and searches for words
 *        that start with a given substring. It then displays the
 *        matched words and their associated values from the JSON
 *        in the console.
 *
 *  Usage:
 *        $ ./program [json_file_path] [substring]
 *        - json_file_path: Path to the JSON file to be read.
 *        - substring: Substring to search for in the JSON keys.
 *
 *  Files:
 *        main.cpp            : Main program file
 *        headers/json.hpp    : Header for JSON operations (nlohmann::json library)
 *        headers/mygetch.hpp : Header for getch function
 *        headers/timer.hpp   : Header for timing operations
 *        headers/rang.hpp    : Header for colored console output
 *        data/dictionary.json: JSON file containing key-value pairs (dictionary)
 *******************************************************************************/

#include "./headers/json.hpp"    // JSON operations
#include "./headers/mygetch.hpp" // Getch function
#include "./headers/timer.hpp"   // Timing operations
#include "./headers/rang.hpp"    // Colored console output
#include <algorithm>             // Algorithm operations
#include <chrono>                // Time-related operations
#include <fstream>               // File operations
#include <iostream>              // Standard I/O operations
#include <thread>                // Thread handling
#include <time.h>                // Time functions
#include <vector>                // Vector operations

#ifdef _WIN32
#include <windows.h> // Windows-specific headers
#else
#include <unistd.h> // UNIX-specific headers
#endif

#define DEBUG false // Debug flag for conditional compilation

using namespace std;
using namespace rang;        // Namespace for colored output
using json = nlohmann::json; // Alias for nlohmann JSON

// Function to load a JSON file and return a json object
json loadJsonFile(const string &filePath)
{
    ifstream fileStream(filePath); // Input file stream for the JSON file
    if (!fileStream.is_open())
    {                                                        // Check if file is open
        cerr << "Failed to open file: " << filePath << endl; // Error message
        exit(1);                                             // Exit with error code 1
    }
    json jsonObject;          // JSON object to store the file content
    fileStream >> jsonObject; // Load JSON content into jsonObject
    fileStream.close();       // Close the file stream
    return jsonObject;        // Return the JSON object
}

// Class for word-related operations
class WordUtils
{
public:
    /**
     * @brief Find words that start with a specified substring in a JSON object
     * @param jsonObject JSON object to search in
     * @param substring Substring to search for in the JSON keys
     * @return Vector of words that start with the given substring
     */
    static vector<string> findWordsWithSubstring(const json &jsonObject, const string &substring)
    {
        vector<string> matchingWords; // Vector to store matching words

        // Iterate through each item in the JSON object
        for (auto &[key, value] : jsonObject.items())
        {
            // Check if the key starts with the specified substring (case-insensitive)
            if (key.find(substring) == 0)
            {
                matchingWords.push_back(key); // Add the matching key to the vector
            }
        }

        return matchingWords; // Return the vector of matching words
    }
};

// Function to clear the console screen
void clearScreen()
{
    // ANSI escape code for clearing the console screen
    cout << "\033[2J\033[1;1H"; // Clear the console and move cursor to the top
}

int main(int argc, char **argv)
{
    // Check if command-line arguments are provided correctly
    if (argc < 3)
    {                                                                       // At least 2 arguments expected
        cout << "Usage: " << argv[0] << " <json_file> <substring>" << endl; // Usage instruction
        return 1;                                                           // Return with error code
    }

    // Load the JSON file specified in the command-line argument
    json jsonObject = loadJsonFile(argv[1]); // Load the JSON file

    // Substring to search for in the JSON keys
    string substring = argv[2];

    // Find words in the JSON object that start with the given substring
    vector<string> matchingWords = WordUtils::findWordsWithSubstring(jsonObject, substring);

    // Display the number of matching words
    cout << endl
         << "Matches: " << fg::yellow << matchingWords.size() << fg::reset << endl
         << endl;

    // Display the matching words in the console
    if (matchingWords.size() == 1)
    {
        // If there is only one match, display its associated value
        string matchedWord = matchingWords[0];          // Get the matched word
        string matchingValue = jsonObject[matchedWord]; // Get the value from the JSON

        // Print the matched word in red and its definition in blue
        cout << fg::red << matchedWord << endl;
        cout << endl
             << fg::blue << "Definition:" << endl
             << fg::yellow << matchingValue << endl
             << endl;
    }
    else
    {
        // If there are multiple matches, display up to 10 of them with proper formatting
        int displayCount = 0; // Counter for displayed words
        for (const auto &word : matchingWords)
        {
            // Find the location of the substring within the word
            size_t location = word.find(substring);

            if (location != string::npos)
            {
                // If the substring is found, highlight it
                cout << fg::blue << word.substr(0, location);                          // Text before the substring
                cout << fg::red << word.substr(location, substring.length());          // The substring itself
                cout << fg::blue << word.substr(location + substring.length()) << " "; // Text after the substring
            }
            else
            {
                cout << fg::blue << word << " "; // If not found, print the entire word
            }

            displayCount++; // Increment the counter
            if (displayCount >= 10)
            {
                break; // Only display the first 10 matching words
            }
        }
        cout << endl
             << endl; // New line at the end
    }

    return 0; // Successful completion
}
