#pragma once

#ifndef _ARCHIVE_HEADER_INCLUDED_
#define _ARCHIVE_HEADER_INCLUDED_
#include <iostream>
#include<string>
#include<string>
#include<set>
#include "file.hpp"
namespace fs =std::filesystem;

class Archive
{
public:
    Archive():root(nullptr){};
    Archive (const Archive & other);
    Archive (Archive&& rhs);
    ~Archive() noexcept{freeRec(root);};
    Archive& operator= (const Archive& other);
    Archive& operator=(Archive && rhs);
    void CreateFromDirectoryList(std::vector<fs::path>& paths,std::fstream& bucketList, std::fstream& stoarge, const bool hashOnly);
    void ExtractArchive(const fs::path &targetPath, const std::vector<fs::path>& relativePaths, std::fstream& bucketList, std::fstream& stoarge )const;
    void writeToFile(std::ofstream &out) const;
    void readFromFile(std::ifstream &in);
    bool empty() const {return !root;};
    void dfsPrint() const;
    fs::file_time_type getDateArchived()const{return date_archived;};
    void setDateArchived(const fs::file_time_type& dArchived);
    void markAsRemoved(std::fstream& bucketList, std::fstream& stoarge);

private:
    struct archiveNode
    {
        public:
        fs::file_time_type last_modified;
        std::vector<File*>files;
        std::string dirLabel;
        std::vector<archiveNode*>children;
        archiveNode():last_modified(fs::file_time_type::clock::now()),files(std::vector<File*>()),dirLabel(""),children(std::vector<archiveNode*>()){
           
        };
        archiveNode(const std::string dirLabel,const std::vector<File*>&files);
        ~archiveNode() noexcept;
    };

    fs::file_time_type date_archived;
    archiveNode* root;
    std::string name;
    void dfsRec(const archiveNode* root) const;
    void freeRec(archiveNode*&);
    void copyRec (archiveNode*& root, const archiveNode* otherRoot);
    void writeRec(const archiveNode* curr, std::ofstream& out) const;
    void readRec(archiveNode*& curr,std::ifstream& in);
    void CreateFromDirectory(archiveNode*& curr,fs::path& dirPath,std::fstream& bucketList, std::fstream& stoarge, const bool hashOnly);
    void extractRec(const archiveNode* curr, const fs::path& targetPath, std::fstream& storage, std::fstream& bucketList) const;
    const archiveNode* findStartingNode(const fs::path& path)const;
    const archiveNode* findRec(const archiveNode *curr, const fs::path &relativePath, fs::path::iterator& it) const;
    const archiveNode* findTopDirNode(const fs::path& relativePath) const;
    void markRec(const archiveNode* curr,std::fstream& bucketList, std::fstream& stoarge);
    fs::path trimPath(const fs::path p) const;
};
#endif
