/*
Makayla Soh | 5/20/2022
Filename: dirEX.cpp
Purpose: 
    Example of how DIR works and the dirent structure (all defined in dirent.h header)
Program Description:
    Opens that current directory. Reads the first entry in the directory into a dirent structure. 
    Prints attributes from the entry. Reads the next entry. Continues for all entries in the directory.

more info about dirent.h: https://www.man7.org/linux/man-pages/man0/dirent.h.0p.html
more info about dirent struct (for each entry): https://www.gnu.org/software/libc/manual/html_node/Directory-Entries.html
*/

#include <iostream>
#include <sys/mman.h>
#include <unistd.h>
#include <dirent.h> // has all info for opendir(), closedir(), readdir(), struct dirent, etc.

using namespace std;

int main()
{
    // read current directory into cwd using getcwd()
    char cwd[1000];
    getcwd(cwd, 1000);
    cout << cwd << endl;

    //open cwd
    DIR *directory = opendir(cwd);

    // create a structure to hold an entry in the directory
    // note: the dirent structure is defined in dirent.h
    struct dirent *entry;

    //read first dir from directory (cwd)
    entry = readdir(directory);

    // while entry is not null
    // loop through and display some attributes of each entry
    while(entry) {

        // attribute: d_name
        cout << "\nEntry Name: " << entry->d_name << endl;

        // attribute: d_fileno
        cout << "File serial number: " << entry->d_fileno << endl;

        // attribute: d_type
        if(entry->d_type == DT_REG)
            cout << "Type of file: regular file"<< endl;
        else if(entry->d_type == DT_DIR)
            cout << "Type of file: directory"<< endl;
        else if(entry->d_type == DT_FIFO)
            cout << "Type of file: named pipe"<< endl;
        else
            cout << "Type of file: something else ... look up in the manual!"<< endl;


        // read next entry in directory
        entry = readdir(directory);
    }

    closedir(directory);

    return 0;
}