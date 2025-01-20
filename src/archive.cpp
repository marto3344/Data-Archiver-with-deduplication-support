#include "archive.hpp"
#include<exception>

Archive::Archive(const Archive &other)
{
    copyRec(root,other.root);
}

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
    if(root)
        freeRec(root);
    readRec(root,in);
}

void Archive::freeRec(archiveNode*& node)
{
    if(!node)
    {
        return;
    }
    for(auto& child:node->children)
    {
        freeRec(child);
    }
    for(auto& file:node->files)
    {
        delete file;
    }   
    delete node;
    node = nullptr;
}

void Archive::copyRec(archiveNode *&root, const archiveNode *otherRoot)
{
    if(!otherRoot)
        return;
    //TODO:: Implement copying
}

void Archive::writeRec(const archiveNode *curr, std::ofstream &out) const 
{
    if(!curr)
        return;
    size_t filesSize =curr->files.size();
    out.write(reinterpret_cast<const char*>(&filesSize), sizeof(size_t));
    for (size_t i = 0; i < filesSize; i++)
    {
        curr->files[i]->serialize(out);
    }
    size_t LabelSize =curr->dirLabel.size();
    out.write(reinterpret_cast<const char*>(&LabelSize),sizeof(size_t));
    out.write(reinterpret_cast<const char*>(&curr->dirLabel),LabelSize);
    size_t childrenSize = 0;
    std::vector<int>childrenIndexes;
    for (int i = 0; i < curr->children.size(); i++)
    {
        if(curr->children[i])
        {
            childrenIndexes.push_back(i);
            childrenSize++;
        }
    }  
    out.write(reinterpret_cast<const char*>(&childrenSize), sizeof(size_t));
    for (size_t i = 0; i < childrenSize; i++)
    {
        writeRec(curr->children[childrenIndexes[i]],out);
    }
}

void Archive::readRec(archiveNode *&curr, std::ifstream &in)
{
    root = new archiveNode();
    size_t filesSize = 0;
    in.read(reinterpret_cast<char*>(&filesSize),sizeof(size_t));
    root->files.reserve(filesSize);
    for (int i = 0; i < filesSize; i++)
    {
        File* f = new File;
        f->deserialize(in);
        root->files.push_back(f);
    }
    size_t labelSize = 0;
    in.read(reinterpret_cast<char*>(&labelSize), sizeof(size_t));
    curr->dirLabel.reserve(labelSize);
    in.read(reinterpret_cast<char*>(&curr->dirLabel),sizeof(labelSize));
    size_t childrenSize = 0;
    in.read(reinterpret_cast<char*>(&childrenSize),sizeof(size_t));
    curr->children.reserve(childrenSize);
    for (int i = 0; i < childrenSize; i++)
    {
        curr->children.push_back(nullptr);
        readRec(curr->children[i],in);
    }
}
