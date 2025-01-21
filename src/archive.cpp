#include "archive.hpp"
#include<exception>

Archive::Archive(const Archive &other)
{
    copyRec(root,other.root);
}

Archive::Archive(Archive &&rhs)
{
    std::swap(root,rhs.root);
}

Archive &Archive::operator=(const Archive &other)
{
    if(this != &other)
    {
        freeRec(root);
        copyRec(root,other.root);
    }
    return *this;
}

Archive &Archive::operator=(Archive &&rhs)
{
    if(this != &rhs)
    {
        std::swap(root,rhs.root);
    }
    return *this;
}

void Archive::CreateFromDirectoryList(std::vector<fs::path> &paths,std::fstream& bucketList, std::fstream& stoarge, const bool hashOnly)
{
    root = new archiveNode();
    root->dirLabel = '/';
    root->children.reserve(paths.size());
    try
    {
        for (int i = 0; i < paths.size(); i++)
        {
            root->children.push_back(nullptr);
            CreateFromDirectory(root->children[i],paths[i],bucketList,stoarge,hashOnly);
        }      
    }
    catch(const std::exception& e)
    {
        std::cerr << e.what() << '\n';
    }
    catch(...)
    {
        std::cout<<"Somethig went wrong creting the archives";
    }
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
    delete node;
    node = nullptr;
}

void Archive::copyRec(archiveNode *&root, const archiveNode *otherRoot)
{
    if(!otherRoot)
        return;
    root = new archiveNode(otherRoot->dirLabel,otherRoot->files);
    root->children.reserve(otherRoot->children.size());

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

void Archive::CreateFromDirectory(archiveNode*& curr, fs::path &dirPath,std::fstream& bucketList, std::fstream& stoarge, const bool hashOnly)
{
    if(dirPath.empty()||!fs::exists(dirPath) || !fs::is_directory(dirPath))
    {
        return;
    }
    std::vector<fs::path>subDirectories;
    std::vector<fs::path>files;
    for(const auto& entry:fs::directory_iterator(dirPath))
    {
        if(fs::is_regular_file(entry))
        {
            files.push_back(entry);
        }
        else if (fs::is_directory(entry))
        {
            subDirectories.push_back(entry);
        }
    }
    curr = new archiveNode();
    curr->dirLabel = dirPath.string();
    curr->last_modified = fs::last_write_time(dirPath);
    curr->files = std::vector<File*>(files.size(),nullptr);
    for(int i =0;i<files.size(); i++)
    {
        File* f = new File();
        if(f->storeFile(files[i],bucketList,stoarge,hashOnly))
        {
            curr->files.push_back(f);
        }
        else{//TODO: Better exception safety;
            delete f;
            std::string message ="Error! Can't store the file: ";
            message.append(files[i].string());
            message+='\n';
            throw std::runtime_error(message);
        }
    }
    curr->children= std::vector<archiveNode*>(subDirectories.size(),nullptr);
    for (int i = 0; i < subDirectories.size(); i++)
    {
        CreateFromDirectory(curr->children[i],subDirectories[i],bucketList,stoarge,hashOnly);
    }
    
}

Archive::archiveNode::archiveNode(const std::string dirLabel, const std::vector<File *> &files)
{
    
    this->dirLabel = dirLabel;
    this->files.reserve(files.size());
    for (size_t i = 0; i < files.size(); i++)
    {
        File* f = new File(*files[i]);
        this->files.push_back(f);
    }
    children = std::vector<archiveNode*>();
}

Archive::archiveNode::~archiveNode() noexcept
{
    for (size_t i = 0; i < files.size(); i++)
    {
        delete files[i];
    }
}
