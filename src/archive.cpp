#include "archive.hpp"
#include<exception>

void Archive::writeToFile(std::ofstream &out) const
{
    if(!out.is_open())
    {
        throw std::invalid_argument("File is not opened!Can not read archive data");
    }
    writeRec(root,out);
}

void Archive::readFromFile(std::ifstream &in)
{
    if(!in.is_open())
    {
        throw std::invalid_argument("File is not opened!Can not read archive data");
    }
    root->readFromFile(in);
}

void Archive::freeRec(archiveNode * node)
{
    if(!node)
    {
        return;
    }
    for(auto& child:node->next)
    {
        freeRec(child);
    }
    for(auto& file:node->files)
    {
        delete file;
    }   
    delete node;
}

void Archive::writeRec(const archiveNode *curr, std::ofstream &out) const 
{

    if(!out.is_open())
    {
        std::cout<<"Error! Can't write the archive data! Archive file is not opened!";
        return;
    }
    if(!curr)
        return;
    size_t filesSize =curr->files.size();
    out.write(reinterpret_cast<const char*>(&filesSize), sizeof(size_t));
    for (size_t i = 0; i < filesSize; i++)
    {
        curr->files[i]->writeMetaData(out);
    }
    size_t LabelSize =curr->dirLabel.size();
    out.write(reinterpret_cast<const char*>(&LabelSize),sizeof(size_t));
    out.write(reinterpret_cast<const char*>(&curr->dirLabel),LabelSize);
    size_t childrenSize = curr->next.size();
    out.write(reinterpret_cast<const char*>(&childrenSize), sizeof(size_t));
    for (size_t i = 0; i < childrenSize; i++)
    {
        writeRec(curr->next[i],out);
    }
}

