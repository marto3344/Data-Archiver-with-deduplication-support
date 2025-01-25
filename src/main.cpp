#include<iostream>
#include<filesystem>
#include<set>
#include <cstring>
#include <exception>
#include<string>
#include "storageManager.hpp"


namespace fs  = std::filesystem;
void parseArchivePaths(size_t start,const size_t size,const char* input[],std::set<fs::path>&paths)
{
    for(size_t i = start; i <size; i++)
    {
        std::string str = input[i];
        fs::path p;
    
        if(str[0]=='/' || str[0] == '\\')
        {
            if(str.size()>1)
                str.erase(0,1);
            else{
                p = fs::path("/");
                paths.insert(p);
                return;
            }
        }
        else if(str.find("./") == 0 || str.find(".\\")==0)
        {
            if(str.size()>2)
                str.erase(0,2);
            else{
                p = fs::path("/");
                paths.insert(p);
                return;
            }
        }
        p = fs::path(str);
        paths.insert(fs::weakly_canonical(p));
    }
}
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
    const size_t COMMANDS_COUNT = 8;
    const char* commandsList [COMMANDS_COUNT] = {"check", "create", "extract","help", "update","initialize","delete","info"};

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
        if(argc<3)
        {
            std::cout<<"You should provide archive name than target path!\n";
            return -1;
        }
        std::string name = argv[2];
        if(argc < 4){
            std::cout<<"You should provide target path!";
            return -1;
        }
        if(!fs::exists(argv[3]))
        {
            std::cout<< "The path you provided does not exists! Do you want to create a dir in the target path? (Type y/n)\n";
            char responce;
            std::cin>>responce;
            if(responce=='y')
            {
                try
                {          
                    fs::create_directory(argv[3]);
                    if(fs::exists(argv[3]))
                        std::cout<<"Directory created successfully! Now trying to extracting ... \n";
                    else{
                        std::cout<<"Somethig went wrong during directory creation! Please try again! \n";
                        return -1;
                    }
                }
                catch(...)
                {
                    std::cout<<"Somethig went wrong during directory creation! Please try again! \n";
                    return -1;
                }
                
            }
            else if (responce=='n')
            {
                return 0;
            }
            else{
                std::cout<<"Error! Unrecognized symbol: "<<responce<<'\n';
                return -1;
            }

        }
        try
        {
            fs::path targetPath(fs::canonical(argv[3]));
            if(!fs::exists(targetPath))
            {
                 std::cout<< "Invalid path: "<<argv[3]<<'\n';
                 std::cout<<"Make sure the path exists! Also use quotations if needed!\n";
                 return -1;
            }
            std::set<fs::path> relativePaths;
            if(argc>4)
            {
                parseArchivePaths(4,argc,argv,relativePaths);
            }
            StorageManager::ExtraxtArchive(name, targetPath, relativePaths);
        }
        catch(const std::exception& e)
        {
            std::cout<<"Error! Message:\n";
            std::cerr << e.what() << '\n';
            return -1;
        }
        catch(...)
        {
            std::cout<<"Something went wrong!Please try again!\n";
            return -1;
        }
        
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
            std::cout<<"Error! Not enough arguments for update!\n";
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
            StorageManager::UpdateArchive(hashOnly,archiveName,directories);
        }
        catch(const std::exception& e)
        {
            std::cerr << e.what() << '\n';
            return -1;
        }
        
    }
    else if (commandNumber== 5)//Initialize
    {
        try
        {
            std::cout<<"Warning! If you run this command all the previous data will be lost! Are you sure? (Type y/n)\n";
            char responce;
            std::cin>>responce;
            if(responce=='y')
            {
                StorageManager::InitializeStorage();
                std::cout<<"Storage initialized successfully!";
            }
            else if (responce=='n')
            {
                return 0;
            }
            else{
                std::cout<<"Error! Unrecognized symbol: "<<responce<<'\n';
                return -1;
            }
        }
        catch(const std::exception& e)
        {
            std::cerr << e.what() << '\n';
            return -1;
        }
        
    }
    else if (commandNumber == 6)//Delete
    {
        try
        {
            std::cout<<"Warning! If you run this command all the storage data will be deleted lost! Are you sure? (Type y/n)\n";
            char responce;
            std::cin>>responce;
            if(responce=='y')
            {
               StorageManager::DeleteStorage();
                std::cout<<"Storage deleted successfully!";
            }
            else if (responce=='n')
            {
                return 0;
            }
            else{
                std::cout<<"Error! Unrecognized symbol: "<<responce<<'\n';
                return -1;
            }
        }
        catch(...)
        {
            std::cout<<"Error! Something unexpected happened! Please try again!\n";
            return -1;
        }
        
    }
    else if (commandNumber == 7)//Info
    {
        if(argc<3)
        {
            std::cout<<"Error!Missing arguments!\n The correct format is info <archive-name>\n";
            return -1;
        }
        std::string name = argv[2];
        StorageManager::ArchiveInfo(name);
    }
    return 0;
}