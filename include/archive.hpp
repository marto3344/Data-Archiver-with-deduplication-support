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
    Archive() = default;
    Archive (const Archive & other);
    Archive (Archive&& rhs);

    void writeToFile(std::ofstream &out) const;
    void readFromFile(std::ifstream &in);

private:
    struct archiveNode
    {
        public:
        std::vector<File*>files;
        std::string dirLabel;
        std::vector<archiveNode*>next;
    
    };

    time_point date_archived;
    archiveNode* root = nullptr;
    std::string name;
    void freeRec(archiveNode*);
    void copyRec (archiveNode*);
    void writeRec(const archiveNode* curr, std::ofstream& out) const;
};

#endif
