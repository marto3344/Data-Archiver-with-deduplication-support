
#pragma once

#ifndef _ARCHIVE_NODE_INCLUDED_
#define _ARCHIVE_NODE_INCLUDED_
#include "file.hpp"
#include "archiveTreeEdge.hpp"
#include<fstream>
#include<vector>
#include<string>
#include<chrono>
using time_point = std::chrono::system_clock::time_point;
class archiveNode
{
    public:
    std::vector<File*>files;
    std::string dirLabel;
    std::vector<archiveNode*>next;
    
    archiveNode()= default;
    archiveNode (const archiveNode& other);
    archiveNode (archiveNode&& rhs);
    void readFromFile(std::ifstream& in);
    void writeToFile(std::ofstream& out) const;

    private:
    void free();
    void copy();
};

#endif