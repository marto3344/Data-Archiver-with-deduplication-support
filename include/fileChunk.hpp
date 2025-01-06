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
    FileChunk() = default;
    void readFromFile(std::ifstream& in);
    void writeToFile (std::ofstream& out)const;

    uint64_t calculateHash();
    private:
    time_point last_modified;
    uint32_t filesCount;
    uint32_t size;
    uint64_t hash;
    uint64_t chunk_id;
    std::vector<uint8_t> chunk_data;

};

#endif