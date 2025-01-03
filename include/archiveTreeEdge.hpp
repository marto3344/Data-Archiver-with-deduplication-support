#pragma once

#ifndef _ARCHIVE_EDGE_INCLUDED_
#define _ARCHIVE_EDGE_INCLUDED_

#include<string>
#include<fstream>
#include<cstddef>
#include<chrono>

class archiveEdge{
    public:
    uint64_t next;
    std::string label;
};

#endif