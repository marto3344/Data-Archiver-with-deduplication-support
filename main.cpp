#include<iostream>
#include<filesystem>
#include <vector>
#include <cstring>

namespace fs  = std::filesystem;

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

int main(int argc, char * argv[])
{
    if(argc < 2)
    {
        std::cout<<"Error! Empty argument list! Pass help as an argument to see the available commands!\n";
        return -1;
    }
    const size_t COMMANDS_COUNT = 5;
    const char* commandsList [COMMANDS_COUNT] = {"check", "create", "extract","help", "update"};

    int commandNumber = findCommand(commandsList,COMMANDS_COUNT, argv[1]);
    std::cout<<commandNumber<<'\n';
    if(commandNumber == -1)
    {
        std::cout<<"Error! Unrecognized command: "<<argv[1]<<" Pass help as an argument to see the available commands!\n";
        return -1;
    }
    if(commandNumber  == 0)//Check
    {

    }
    else if (commandNumber == 1)//Create
    {

    }
    else if (commandNumber == 2)//Extract
    {

    }
    else if (commandNumber == 3)// Help
    {

    }
    else if (commandNumber == 4)//Update
    {

    }
}