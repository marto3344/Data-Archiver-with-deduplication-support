#include<iostream>
#include<filesystem>
#include <vector>
#include <cstring>
#include <exception>

namespace fs  = std::filesystem;

void parseDirectories(size_t start, const size_t end, const  char* input [], std::vector<fs::path>& directories){
    while (start < end)
    {   
        fs::path p (input[start]);
        if(!fs::exists(p))
        {   
            std::string errorMsg = "Invalid path: ";
            errorMsg.append(input[start]);
            errorMsg+='\n';
            errorMsg.append("Make sure the path exists! Also use quotations if needed!");
            throw std::invalid_argument(errorMsg);
        }
        directories.push_back(fs::canonical(p));
        start++;
    }
    
}

int findCommand (const char ** commandList, const size_t listSize, const char* command)
{
    int left = 0;
    int right = listSize - 1;
    while (left <= right)
    {
        int middle =  (left + right)/2;
        int compare = strcmp(command, commandList[middle]); 
        if(compare == 0)
        {
            return middle;
        }
        else if (compare > 0)
        {
           left = middle + 1;
        }
        else{
            right = middle -1;
        }
    }  
    return -1;
}

int main(int argc, const char * argv[])
{
    if(argc < 2)
    {
        std::cout<<"Error! Empty argument list! Pass help as an argument to see the available commands!\n";
        return -1;
    }
    const size_t COMMANDS_COUNT = 5;
    const char* commandsList [COMMANDS_COUNT] = {"check", "create", "extract","help", "update"};

    int commandNumber = findCommand(commandsList,COMMANDS_COUNT, argv[1]);
    if(commandNumber == -1)
    {
        std::cout<<"Error! Unrecognized command: "<<argv[1]<<"! Pass help as an argument to see the available commands!\n";
        return -1;
    }
    if(commandNumber  == 0)//Check
    {
        
    }
    else if (commandNumber == 1)//Create
    {
        if(argc < 4)
        {
            std::cout<<"Error! Not enough arguments for create!\n";
            std::cout<<"Use help to view the list of commands!";
            return - 1;
        }
        bool hashOnly = (strcmp(argv[2], "hash-only") == 0);
        try
        {
            std::string archiveName = hashOnly? argv[3]:argv[2];
            size_t directoriesStart = hashOnly? 4:3;
            std::vector<fs::path> directories;
            parseDirectories(directoriesStart,argc,argv,directories);
        }
        catch(const std::exception& e)
        {
            std::cerr << e.what() << '\n';
            return -1;
        }
        
        

    }
    else if (commandNumber == 2)//Extract
    {
        
    }
    else if (commandNumber == 3)// Help
    {
        using namespace std;
        cout<<"Here is a list of commands!\n";
        //TODO: List the commands
    }
    else if (commandNumber == 4)//Update
    {
        if(argc < 4)
        {
            std::cout<<"Error! Not enough arguments for create!\n";
            std::cout<<"Use help to view the list of commands!";
            return - 1;
        }
        bool hashOnly = (strcmp(argv[2], "hash-only") == 0);
        try
        {
            std::string archiveName = hashOnly? argv[3]:argv[2];
            size_t directoriesStart = hashOnly? 4:3;
            std::vector<fs::path> directories;
            parseDirectories(directoriesStart,argc,argv,directories);
        }
        catch(const std::exception& e)
        {
            std::cerr << e.what() << '\n';
            return -1;
        }
        
    }
}