/**
 * Quick and Dirty File Splitter
 *
 * Takes an input file (typically a csv file) and strips out a specific percentage of random lines, and puts them in
 * different file.  This is useful for generating a random test source for testing success of Machine Learning
 * algorithms.  I've had to do that a lot lately, and made this quick app to let me specify percentages.
 *
 * Features:
 *	Gets the correct number of lines based on percentage specified.
 *  Doesn't require a lot of memory.
 *  O(n) - linear based on filesize - to prevent duplicate random lines.
 *  Skew coming soon...
 *  Retains order of lines.
 * 
 * Right now it doesn't give you the option of naming the output files, but that's coming soon.
 * Also, I'll add skewing to the randomization, so I can get more lines from earlier or later in the file
 * (for cases that are "time" dependent, etc.).
 *
 * Compile with: 
 *      g++ -std=c++0x -o genTestFile genTestFile.cpp 
 *
 * Example Run:
 * ./genTestFile samples.csv 10
 *  This takes the samples.csv and makes testing.csv with 10% of the lines and puts the remainder in training.csv.
 *
 * @author Stephen Cook
 * @copyright Stephen Cook 2013
 **/

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <cstdlib>
#include <algorithm>
#include <unordered_map>

using namespace std;

int main(int argc, char* argv[]) {
    if (argc != 3) { // Check the value of argc. If not enough parameters have been passed, inform user and exit.
        cout << "Usage:  " << argv[0] << " <infile> <percent to remove [1-99]>\n"; // Inform the user of how to use the program
        return(0);
    } else { // if we got enough parameters...
        std::string inFile;
        int percOut,percIn,totalLines, trainLines,testLines;

        inFile = argv[1];
        istringstream(argv[2]) >> percOut;

        // Check to see that it's in range...
        if(percOut < 1 || percOut > 99) {
                cout << "Percentage to put in testing file must be between 1 and 99.\n";
                return(0);
        }

        // Set the percentage to keep for training.
        percIn=100 - percOut;


        cout << "Splitting " << inFile << " into training (" << percIn << "%) and testing ("<< percOut << "%) files.\n";

        // verify that the file exists, and open it.
        std::ifstream ifile(inFile.c_str(), ios::in);
        if(!ifile) {
                cout << "Error: The file does not exist or is unavailable: " << inFile << "\n";
                return(0);
        }

        // Count the lines in the file
        totalLines=0;
        std::string tmp;
        while(getline(ifile,tmp)) 
                ++totalLines;

        testLines = totalLines * (percOut/100.0);
        trainLines = totalLines - testLines;
        cout << "Total samples: " << totalLines << "\n";
        cout << "Total Testing Lines:  " << testLines << "\n";
        cout << "Total Training Lines: " << trainLines << "\n";


        unordered_map<int,bool> lines;
        int rline;
        srand(time(NULL));
        for(int x=0;x<testLines;x++) {
                rline=((double) rand() / (RAND_MAX)) *(totalLines+1);
                //cout << x << " := " << rline << "\n";
                //lines[rline]=true;
                unordered_map<int,bool>::const_iterator got = lines.find (rline);
                if(got == lines.end()) {
                        lines.insert(make_pair<int,int>(rline,true));
                        //cout << "Added " << rline << std::endl;
                } else {
                        x--;
                        //cout << "Got a duplicate: " << rline << std::endl;
                }
        }

		/*
		* uncomment if you want to see which line numbers were selected.
        for (auto it = lines.begin(); it != lines.end(); ++it) {
                cout << it->first << endl;
        }
        cin.get();
		*/
		
        // Reset read from input file
        ifile.clear();
        ifile.seekg(0,ios::beg);

        // Open the output files. (Need to set dynamic names based in inFile...)
        ofstream trainFile,testFile;
        trainFile.open("training.csv");
        testFile.open("testing.csv");
        int x=0;
        while(getline(ifile,tmp)) {
                unordered_map<int,bool>::const_iterator got = lines.find (x);
                if(got == lines.end()) {
                        //cout << "TRAIN: ";
                        trainFile << tmp << endl;
                } else {
                        //cout << "WRITE TO TEST FILE: ";
                        testFile << tmp << endl;
                }
                //cout << tmp << "\n";
                x++;
        }
        
		// Close the files.
		trainFile.close();
        testFile.close();

        return 1;
    }
}