
#pragma once

#ifndef _ARCHIVE_NODE_INCLUDED_
#define _ARCHIVE_NODE_INCLUDED_
#include "file.hpp"
#include "archiveTreeEdge.hpp"
#include<fstream>
#include<vector>

class archiveNode
{
    public:
    std::vector<File*>files;
    std::vector<std::pair<archiveNode*,archiveEdge>>next;
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