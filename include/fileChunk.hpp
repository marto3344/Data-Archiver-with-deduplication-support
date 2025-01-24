#pragma once

#ifndef _FILE_CHUNK_HEADER_INCLUDED_
#define _FILE_CHUNK_HEADER_INCLUDED_

#include<cstddef>
#include<fstream>
#include<vector>

class FileChunk{
    public:
    FileChunk();
    FileChunk(uint32_t chunkSize):chunkSize(chunkSize){
        chunk_data = std::vector<uint8_t>(chunkSize,0);
        filesCount = 1;
    };
    void deserialize(std::istream& in);
    void serialize (std::ostream& out)const;
    void writeChunkData(std::ofstream& out, uintmax_t size) const;

    void moveChunkData(std::vector<uint8_t>& data);
    void hashChunk();
    bool compareChunkData(const FileChunk& other) const;
    void storeChunk(std::fstream& storage,std::fstream& bucketList, const bool hashOnly);
    uint64_t getHash() const {return hash;};
    uint64_t getId() const {return chunk_id;};
    uint32_t getChunkSize(){return chunkSize;};
    private:
    uint32_t chunkSize;
    uint32_t filesCount;
    uint64_t hash;
    uint64_t chunk_id;
    std::vector<uint8_t> chunk_data;

};
#endif