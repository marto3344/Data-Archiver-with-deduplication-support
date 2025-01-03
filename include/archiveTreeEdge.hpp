#pragma once

#ifndef _ARCHIVE_EDGE_INCLUDED_
#define _ARCHIVE_EDGE_INCLUDED_

#include<string>
#include<fstream>
#include<cstddef>
#include<chrono>
using time_point = std::chrono::system_clock::time_point;
struct archiveEdge{
    time_point  lastModified;
    std::string label;
};
#endif