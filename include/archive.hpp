#pragma once

#ifndef _ARCHIVE_HEADER_INCLUDED_
#define _ARCHIVE_HEADER_INCLUDED_
#include <iostream>
#include<string>
#include <chrono>
#include<string>
#include "file.hpp"

using time_point = std::chrono::system_clock::time_point;
class Archive
{
public:
    Archive():root(nullptr){};
    Archive (const Archive & other);
    Archive (Archive&& rhs);
    Archive& operator= (const Archive& other);
    Archive& operator=(Archive && rhs);


    void writeToFile(std::ofstream &out) const;
    void readFromFile(std::ifstream &in);
    bool empty() const {return !root;};
private:
    struct archiveNode
    {
        public:
        std::vector<File*>files;
        std::string dirLabel;
        std::vector<archiveNode*>children;
    
    };

    time_point date_archived;
    archiveNode* root;
    std::string name;
    void freeRec(archiveNode*&);
    void copyRec (archiveNode*& root, const archiveNode* otherRoot);
    void writeRec(const archiveNode* curr, std::ofstream& out) const;
    void readRec(archiveNode*& curr,std::ifstream& in);
};

#endif
