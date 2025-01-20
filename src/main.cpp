#include<iostream>
#include<filesystem>
#include<set>
#include <cstring>
#include <exception>
#include "storageManager.hpp"


namespace fs  = std::filesystem;

void parseDirectories(size_t start, const size_t end, const  char* input [], std::set<fs::path>& directories){
    for (size_t i = start; i < end; i++)
    {   
        fs::path p (input[i]);
        if(!fs::exists(p))
        {   
            std::string errorMsg = "Invalid path: ";
            errorMsg.append(input[i]);
            errorMsg+='\n';
            errorMsg.append("Make sure the path exists! Also use quotations if needed!");
            throw std::invalid_argument(errorMsg);
        }
        directories.insert(fs::canonical(p));    
    }
}

int findCommand (const char ** commandList, const size_t listSize, const char* command)
{
    int left = 0;
    int right = listSize - 1;
    for (int  i = 0; i < listSize; i++)
    {
        int compare = strcmp(command, commandList[i]);
        if(compare == 0)
            return i;
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
    const size_t COMMANDS_COUNT = 6;
    const char* commandsList [COMMANDS_COUNT] = {"check", "create", "extract","help", "update","initialize"};

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
        try
        {
            bool hashOnly = (strcmp(argv[2], "hash-only") == 0);
            std::string archiveName = hashOnly? argv[3]:argv[2];
            size_t directoriesStart = hashOnly? 4:3;
            std::set<fs::path> directories;
            parseDirectories(directoriesStart,argc,argv,directories);
            StorageManager::CreateArchive(hashOnly,archiveName,directories);

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
        try
        {
            bool hashOnly = (strcmp(argv[2], "hash-only") == 0);
            std::string archiveName = hashOnly? argv[3]:argv[2];
            size_t directoriesStart = hashOnly? 4:3;
            std::set<fs::path> directories;
            parseDirectories(directoriesStart,argc,argv,directories);
        }
        catch(const std::exception& e)
        {
            std::cerr << e.what() << '\n';
            return -1;
        }
        
    }
    else if (commandNumber== 5)
    {
        try
        {
            std::cout<<"Warning! If you run this command all the previous data will be lost! Are you sure? (Type y/n)\n";
            char c;
            std::cin>>c;
            if(c=='y')
            {
                StorageManager::InitializeStorage();
                std::cout<<"Storage initialized successfully!";
            }
            else if (c=='n')
            {
                return 0;
            }
            else{
                std::cout<<"Error! Unrecognized symbol: "<<c<<'\n';
                return -1;
            }
        }
        catch(const std::exception& e)
        {
            std::cerr << e.what() << '\n';
            return -1;
        }
        
    }
}