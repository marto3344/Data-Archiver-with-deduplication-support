#pragma once

#ifndef _ARCHIVE_HEADER_INCLUDED_
#define _ARCHIVE_HEADER_INCLUDED_
#include <iostream>
#include<string>
#include<string>
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

    void writeToFile(std::ofstream &out) const;
    void readFromFile(std::ifstream &in);
    bool empty() const {return !root;};
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
    void freeRec(archiveNode*&);
    void copyRec (archiveNode*& root, const archiveNode* otherRoot);
    void writeRec(const archiveNode* curr, std::ofstream& out) const;
    void readRec(archiveNode*& curr,std::ifstream& in);
    void CreateFromDirectory(archiveNode*& curr,fs::path& dirPath,std::fstream& bucketList, std::fstream& stoarge, const bool hashOnly);
};

#endif
