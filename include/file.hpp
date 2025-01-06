#pragma once

#ifndef _FILE_HEADER_INCLUDED_
#define _FILE_HEADER_INCLUDED_
#include<filesystem>
#include<iostream>
#include<vector>
#include<fstream>
#include<cstddef>
#include<string>
#include <chrono>
#include<vector>

namespace fs = std::filesystem;
using time_point = std::chrono::system_clock::time_point;
class File{
    public:
    File() = default;
    
    void writeMetaData(std::ofstream&out) const;
    void readMetaData(std::ifstream& in);

    bool extractFile(const fs::path& path, std::ifstream& in) const;
    bool updateFile(const fs::path& targerFile, std::ifstream& in);
    bool storeFile (std::ifstream& in ) const;

    private:
    time_point last_modified;
    uint32_t id;
    uint64_t size;
    std::string name;
    std::vector<std::pair<uint64_t,uint64_t>>chunks;//{hash_value, chunk_id}

};
#endif