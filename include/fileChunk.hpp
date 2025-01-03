#pragma once

#ifndef _FILE_CHUNK_HEADER_INCLUDED_
#define _FILE_CHUNK_HEADER_INCLUDED_
#include<cstddef>
#include<fstream>
#include<chrono>
#include<vector>
using time_point = std::chrono::system_clock::time_point;
class FileChunk{
    public:
    uint32_t filesCount;
    uint32_t size;
    uint64_t fileRefs;
    time_point last_modified;
    std::vector<uint8_t> chunk_data;

};

#endif