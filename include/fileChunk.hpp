#pragma once

#ifndef _FILE_CHUNK_HEADER_INCLUDED_
#define _FILE_CHUNK_HEADER_INCLUDED_

#include<cstddef>
#include<fstream>
#include<vector>

class FileChunk{
    public:
    FileChunk();
    FileChunk(const std::vector<uint8_t>& data);
    void deserialize(std::istream& in);
    void serialize (std::ostream& out)const;
    void writeChunkData(std::ofstream& out) const;
    void hashChunk();
    bool compareChunkData(const FileChunk& other) const;
    void storeChunk(std::fstream& storage,std::fstream& bucketList, const bool hashOnly);
    uint64_t getHash() const {return hash;};
    uint64_t getId() const {return chunk_id;};
    void setId(const uint64_t id){chunk_id=id;}; 
    uint32_t getChunkSize()const{return chunk_data.size();};
    uint32_t getFilesCount()const {return filesCount;};
    bool operator==(const FileChunk&other) const;
    bool operator!=(const FileChunk&other)const {return !(*this==other);};
    private:
    uint32_t filesCount;
    uint64_t hash;
    uint64_t chunk_id;
    std::vector<uint8_t> chunk_data;

};
#endif