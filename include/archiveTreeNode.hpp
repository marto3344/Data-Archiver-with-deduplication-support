
#pragma once

#ifndef _ARCHIVE_NODE_INCLUDED_
#define _ARCHIVE_NODE_INCLUDED_
#include "file.hpp"
#include "archiveTreeEdge.hpp"
#include<fstream>
#include<vector>
#include <chrono>
using time_point = std::chrono::system_clock::time_point;
class archiveNode
{
    public:
    std::vector<archiveEdge*> nextNodes;
    std::vector<File*>files;

};

#endif