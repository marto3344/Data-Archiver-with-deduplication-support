#include "archive.hpp"
#include<exception>

void Archive::readFromFile(std::ifstream &in)
{
    if(!in.is_open())
    {
        throw std::invalid_argument("File is not opened!Can not read archive data");
    }
    
}

void Archive::freeRec(archiveNode * node)
{
    
    
}
