#include "archive.hpp"
#include "storageManager.hpp"
#include<exception>

Archive::Archive(const Archive &other)
{
    date_archived = other.date_archived;
    name = other.name;
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

bool Archive::operator==(const Archive &other) const
{
    return (name==other.name)&&(date_archived == other.date_archived)&&compareRec(root,other.root);
}

bool Archive::CreateFromDirectoryList(std::vector<fs::path> &paths,std::fstream& bucketList, std::fstream& stoarge, const bool hashOnly)
{
    root = new archiveNode();
    root->dirLabel = "";
    root->children.reserve(paths.size());
    date_archived = fs::file_time_type::clock::now();
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
        stoarge.close();
        bucketList.close();
        std::cerr << e.what()<<'\n';
        return false;
    }
    catch(...)
    {
        stoarge.close();
        bucketList.close();
        std::cout<<"Somethig went wrong during this operation!";
        return false;
    }
    return true;
}

void Archive::ExtractArchive(const fs::path &targetPath, const std::set<fs::path>& archivePaths, std::fstream &bucketList, std::fstream &storage) const
{
    if(archivePaths.empty() || (*archivePaths.begin()).string() == "/")
    {
        extractRec(root,targetPath,storage,bucketList);
    }
    else{
        std::vector<fs::path> filteredPaths;
        StorageManager::removeOverlappingPaths(filteredPaths,archivePaths);
        std::unordered_set<std::string>pathsToFind;
        std::vector<const archiveNode*>startingNodes;
        for (int i = 0; i < filteredPaths.size(); i++)
        {   
            pathsToFind.insert(filteredPaths[i].string());
        }
        findStartingNodes(root,startingNodes,pathsToFind);
        if(startingNodes.size()< filteredPaths.size())
        {
            throw::std::invalid_argument("Some of the input paths are incorect or missing in the archive! ");
        }
        for (int i = 0; i < startingNodes.size(); i++)
        {
            extractRec(startingNodes[i],targetPath,storage,bucketList);
        }
        
    }
}

void Archive::writeToFile(std::ofstream &out) const
{
    if(!out.is_open())
    {
        throw std::invalid_argument("File is not opened!Can not read archive data");
    }
    out.write(reinterpret_cast<const char*>(&date_archived),sizeof(fs::file_time_type));
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
    in.read(reinterpret_cast<char*>(&date_archived),sizeof(fs::file_time_type));
    readRec(root,in);
}

void Archive::dfsPrint() const
{   
    std::cout<<"Date archived: ";
    StorageManager::printTime(date_archived);
    std::cout<<'\n';
    for(int i = 0; i < root->children.size();i++)
    {
        dfsRec(root->children[i]);
    }
}

void Archive::setDateArchived(const fs::file_time_type &dArchived)
{
    date_archived = dArchived;
}

void Archive::markAsRemoved(std::fstream &bucketList, std::fstream &storage)
{
    markRec(root,bucketList,storage);
}

void Archive::dfsRec(const archiveNode *root) const
{
    if(!root)
        return;
    std::cout<<root->dirLabel<<":\n";
    for (int i = 0; i <root->files.size() ; i++)
    {
        if(root->files[i])
        {
            std::cout<<root->files[i]->getName()<<' ';
        }
    }
    std::cout<<'\n';
    for (size_t i = 0; i < root->children.size(); i++)
    {
        dfsRec(root->children[i]);
    }
    
}

void Archive::freeRec(archiveNode *&node)
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
    root = new archiveNode(otherRoot->last_modified,otherRoot->dirLabel,otherRoot->files);
    root->children.reserve(otherRoot->children.size());
    for (int i = 0; i < otherRoot->children.size(); i++)
    {
        root->children.push_back(nullptr);
        copyRec(root->children[i],otherRoot->children[i]);
    }  
}

void Archive::writeRec(const archiveNode *curr, std::ofstream &out) const 
{
    if(!curr)
        return;
    out.write(reinterpret_cast<const char*>(&curr->last_modified),sizeof(fs::file_time_type));
    size_t filesSize =curr->files.size();
    out.write(reinterpret_cast<const char*>(&filesSize), sizeof(size_t));
    for (size_t i = 0; i < filesSize; i++)
    {
        curr->files[i]->serialize(out);    
    }
    size_t LabelSize =curr->dirLabel.size();
    out.write(reinterpret_cast<const char*>(&LabelSize),sizeof(size_t));
    out.write(reinterpret_cast<const char*>(curr->dirLabel.data()),LabelSize);
    size_t childrenSize = curr->children.size();
    out.write(reinterpret_cast<const char*>(&childrenSize), sizeof(size_t));
    for (size_t i = 0; i < childrenSize; i++)
    {
        writeRec(curr->children[i],out);
    }
}

void Archive::readRec(archiveNode *&curr, std::ifstream &in)
{
    if(in.eof()||!in.good())
        return;
    curr = new archiveNode();
    size_t filesSize = 0;
    in.read(reinterpret_cast<char*>(&curr->last_modified),sizeof(fs::file_time_type));
    in.read(reinterpret_cast<char*>(&filesSize),sizeof(size_t));
    curr->files.reserve(filesSize);
    for (int i = 0; i < filesSize; i++)
    {
        File* f = new File;
        f->deserialize(in);
        curr->files.push_back(f);
    }
    size_t labelSize = 0;
    in.read(reinterpret_cast<char*>(&labelSize), sizeof(size_t));
    curr->dirLabel = std::string(labelSize, '\0');
    in.read(reinterpret_cast<char*>(curr->dirLabel.data()),labelSize);
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
    std::vector<fs::path>dirFiles;
    for(const auto& entry:fs::directory_iterator(dirPath))
    {
        if(fs::is_regular_file(entry))
        {
            dirFiles.push_back(entry);
        }
        else if (fs::is_directory(entry))
        {
            subDirectories.push_back(entry);
        }
    }
    curr = new archiveNode();
    curr->dirLabel = dirPath.string();
    curr->last_modified = fs::last_write_time(dirPath);
    curr->files.reserve(dirFiles.size());
    for(int i =0;i<dirFiles.size(); i++)
    {
        File* f = new File();
        if(f->storeFile(dirFiles[i],bucketList,stoarge,hashOnly))
        {
            curr->files.push_back(f);
        }
        else{
            if(f)
                delete f;
            std::string message ="Can't store the file: ";
            message.append(dirFiles[i].string());
            throw std::runtime_error(message);
        }
    }
    curr->children= std::vector<archiveNode*>(subDirectories.size(),nullptr);
    for (int i = 0; i < subDirectories.size(); i++)
    {
        CreateFromDirectory(curr->children[i],subDirectories[i],bucketList,stoarge,hashOnly);
    }
    
}

void Archive::extractRec(const archiveNode *curr, const fs::path& targetPath, std::fstream& storage, std::fstream& bucketList) const
{
    if(!curr)
        return;
    fs::path currTargetDir = targetPath;
    currTargetDir.append(fs::path(curr->dirLabel).filename().string());
    if(!fs::exists(currTargetDir))
    {
        fs::create_directory(currTargetDir);
    }
    for (int i = 0; i<curr->files.size(); i++)
    {
        fs::path filePath = currTargetDir;
        filePath.append(curr->files[i]->getName());
        std::ofstream file (filePath,std::ios::binary);
        if(!file.is_open())
        {
            std::cout<<"Coudn't extract file: "<<curr->files[i]->getName()<<'\n';
            return;
        }
        curr->files[i]->extractFile(file,storage,bucketList);
        file.close();
    }
    for(int i = 0; i < curr->children.size();i++)
    {
        extractRec(curr->children[i],currTargetDir,storage,bucketList);
    }
}



void Archive::findStartingNodes(const archiveNode* curr,std::vector<const archiveNode*>&nodes,std::unordered_set<std::string>& paths) const
{
   if(!curr)
   {
        return;
   }
   if(paths.find(curr->dirLabel)!= paths.end())
   {
        nodes.push_back(curr);
        return;
   }
   for (int i = 0; i < curr->children.size(); i++)
   {
        findStartingNodes(curr->children[i],nodes,paths);
   }
   
}


void Archive::markRec(const archiveNode *curr, std::fstream &bucketList, std::fstream &storage)
{
    if(!curr)
        return;
    for(int i = 0; i < curr->files.size(); ++i)
    {
        curr->files[i]->markFileDeleted(bucketList,storage);
    }
    for(int i = 0 ;i<curr->children.size();++i)
    {
        markRec(curr->children[i],bucketList,storage);
    }
    
}

bool Archive::compareRec(const archiveNode *root, const archiveNode *otherRoot) const
{
    if(!root || !otherRoot)
        return root==otherRoot;

    if(root->files.size()!=otherRoot->files.size() || root->children.size() != otherRoot->children.size())
        return false;
    if(root->dirLabel != otherRoot->dirLabel || root->last_modified != otherRoot->last_modified)
        return false;
    for (int i = 0; i < root->files.size(); i++)
    {
        if(!(*(root->files[i]) == *(otherRoot->files[i])))
        {
            return false;
        }
    }
    bool result = true;
    for (int i = 0; i < root->children.size(); i++)
    {
        result = result && compareRec(root->children[i],otherRoot->children[i]);
    }
    return result;
}

Archive::archiveNode::archiveNode(const fs::file_time_type& last_modified,const std::string dirLabel, const std::vector<File *> &files)
{
    this->last_modified = last_modified;
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
