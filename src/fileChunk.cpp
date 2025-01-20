#include "fileChunk.hpp"
#include<iostream>
const uint32_t FileChunk::chunkSize = 1<<16; //64KB
extern "C" {
    #include "xxhash.h"
}
FileChunk::FileChunk()
{
    chunk_data.reserve(chunkSize);
}
void FileChunk::readFromFile(std::ifstream &in)
{
    if(!in.is_open())
    {
        throw std::invalid_argument("file is not opened"); 
    }
    in.read(reinterpret_cast<char*>(&last_modified),sizeof(time_point));
    in.read(reinterpret_cast<char*>(&filesCount),sizeof(uint32_t));
    in.read(reinterpret_cast<char*> (&hash),sizeof(uint64_t));
    in.read(reinterpret_cast<char*>(&chunk_id),sizeof(uint64_t));
    size_t dataSize = 0;
    in.read(reinterpret_cast<char*> (&dataSize),sizeof(size_t));
    chunk_data.reserve(dataSize);
    for(size_t i = 0; i < dataSize; ++i)
    {
        uint8_t currByte;
        in.read(reinterpret_cast<char*>(&currByte), sizeof(uint8_t));
        chunk_data.push_back(currByte);
    }
}

void FileChunk::writeToFile(std::ofstream &out) const
{
    if(!out.is_open())
    {
        throw std::invalid_argument("file is not opened"); //TODO: Better exception message
    }

    out.write(reinterpret_cast<const char*> (&last_modified),sizeof(time_point));
    out.write(reinterpret_cast<const char*> (&filesCount),sizeof(uint32_t));
    out.write(reinterpret_cast<const char *> (&hash),sizeof(uint64_t));
    out.write(reinterpret_cast<const char*> (&chunk_id), sizeof(uint64_t));
    size_t dataSize = chunk_data.size();
    out.write(reinterpret_cast<const char*>(&dataSize),sizeof(size_t));
    for (size_t i = 0; i < dataSize; i++)
    {
        out.write(reinterpret_cast<const char*> (&chunk_data[i]), sizeof(uint8_t));
    }
}

void FileChunk::moveChunkData(std::vector<uint8_t> &data)
{
    chunk_data = std::move(data);
}

bool FileChunk::hashChunk()
{
    this->hash = XXH64(chunk_data.data(),FileChunk::chunkSize,0);
    return true; 
}

void FileChunk::storeChunk(std::ofstream &storage, uint32_t capacity, uint32_t&size)
{
    std::cout<<hash<<'\n';
    uint32_t bucketPos = hash%capacity;
}
