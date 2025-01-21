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
    FileChunk();
    void deserialize(std::istream& in);
    void serialize (std::ostream& out)const;
    

    void moveChunkData(std::vector<uint8_t>& data);
    bool hashChunk();
    bool compareChunkData(const FileChunk& other) const;
    void storeChunk(std::fstream& storage,std::fstream& bucketList, const bool hashOnly);
    uint64_t getHash() const {return hash;};
    uint64_t getId() const {return chunk_id;};
    static uint32_t getChunkSize(){return chunkSize;};
    private:
    static const uint32_t chunkSize;
    uint32_t filesCount;
    uint64_t hash;
    uint64_t chunk_id;
    std::vector<uint8_t> chunk_data;

};
#endif