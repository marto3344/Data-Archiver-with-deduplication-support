#pragma once

#ifndef _ARCHIVE_HEADER_INCLUDED_
#define _ARCHIVE_HEADER_INCLUDED_
#include <iostream>
#include<string>
#include <chrono>
#include<string>
#include "archiveTreeNode.hpp"
using time_point = std::chrono::system_clock::time_point;
class Archive
{
public:
    Archive();
    Archive(const std::string& name);
    void writeToFile(std::ofstream &out) const;
    void readFromFile(std::ifstream &in);

private:
    time_point date_archived;
    std::string name;
    archiveNode* root;
};

#endif
