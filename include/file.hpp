#pragma once

#ifndef _FILE_HEADER_INCLUDED_
#define _FILE_HEADER_INCLUDED_
#include<filesystem>
#include<iostream>
#include<vector>
#include<fstream>
#include<cstddef>
#include<string>
#include<vector>

namespace fs = std::filesystem;

class File{
    public:
    File() = default;
    
    void serialize(std::ostream&out) const;
    void deserialize(std::istream& in);

    bool extractFile(const fs::path& path, std::fstream& in) const;
    bool updateFile(const fs::path& targerFile, std::ifstream& in);
    bool storeFile (const fs::path& file,std::fstream& bucketList, std::fstream& stoarge, const bool hashOnly );
    

    private:
    fs::file_time_type last_modified;
    uintmax_t size;
    std::string name;
    std::vector<std::pair<uint64_t,uint64_t>>chunk_list;//{hash_value, chunk_id}
    bool hashFile (const fs::path &filePath, std::fstream& bucketList,std::fstream& storage,  const bool hashOnly);


};
#endif