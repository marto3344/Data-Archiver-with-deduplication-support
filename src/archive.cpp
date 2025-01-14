#include "archive.hpp"
#include<exception>

void Archive::writeToFile(std::ofstream &out) const
{
    if(!out.is_open())
    {
        throw std::invalid_argument("File is not opened!Can not read archive data");
    }
    root->writeToFile(out);
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

Archive::archiveNode::archiveNode(const archiveNode &other)
{
}

Archive::archiveNode::archiveNode(archiveNode &&rhs)
{
}

void Archive::archiveNode::readFromFile(std::ifstream &in)
{
}

void Archive::archiveNode::writeToFile(std::ofstream &out) const
{
}

Archive::archiveNode::~archiveNode()
{
    free();
}

void Archive::archiveNode::free()
{
    for(auto& file:files)
    {
        delete file;
    }
}
