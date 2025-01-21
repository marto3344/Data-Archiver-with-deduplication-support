#include "fileChunk.hpp"
#include "storageManager.hpp"
#include<iostream>
//const uint32_t FileChunk::chunkSize = 1<<16; //64KB
extern "C" {
    #include "xxhash.h"
}
FileChunk::FileChunk()
{
    chunk_data.reserve(chunkSize);
}
void FileChunk::deserialize(std::istream &in)
{
    if(!in.good())
    {
        throw std::invalid_argument("Bad stream!"); 
    }
    in.read(reinterpret_cast<char*>(&chunkSize),sizeof(uint32_t));
    in.read(reinterpret_cast<char*>(&filesCount),sizeof(uint32_t));
    in.read(reinterpret_cast<char*> (&hash),sizeof(uint64_t));
    in.read(reinterpret_cast<char*>(&chunk_id),sizeof(uint64_t));
  
    for(size_t i = 0; i < chunkSize; ++i)
    {
        uint8_t currByte;
        in.read(reinterpret_cast<char*>(&currByte), sizeof(uint8_t));
        chunk_data.push_back(currByte);
    }
}

void FileChunk::serialize(std::ostream &out) const
{
    out.write(reinterpret_cast<const char*> (&chunkSize), sizeof(uint32_t));
    out.write(reinterpret_cast<const char*> (&filesCount),sizeof(uint32_t));
    out.write(reinterpret_cast<const char *> (&hash),sizeof(uint64_t));
    out.write(reinterpret_cast<const char*> (&chunk_id), sizeof(uint64_t));
    for (size_t i = 0; i < chunkSize; i++)
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

bool FileChunk::compareChunkData(const FileChunk &other) const
{
    if(chunk_data.size() != other.chunk_data.size())
        return false;
    for (size_t i = 0; i < other.chunk_data.size(); i++)
    {
        if(chunk_data[i] != other.chunk_data[i])
            return false;
    }   
    return true;
}

void FileChunk::storeChunk(std::fstream &storage, std::fstream &bucketList, const bool hashOnly)
{
    if (!storage.is_open() || !bucketList.is_open())
    {
        throw std::runtime_error("Error! Can't store the chunk!");
    }
    uint32_t bucketPos = (hash % StorageManager::bucketListCapacity) * sizeof(uint64_t);

    bucketList.seekg(bucketPos);
    uint64_t listHead;
    bucketList.read(reinterpret_cast<char *>(&listHead), sizeof(uint64_t));
    if (listHead != 0)
    {
        storage.seekg(listHead);
        for (;;)
        {
            uint64_t nextChunk;
            FileChunk currChunk;
            if (storage.eof())
                break;
            currChunk.deserialize(storage);
            if (currChunk.hash == this->hash)
            {
                if (hashOnly)
                {
                    this->chunk_id = currChunk.chunk_id;
                    return;
                }
                if (compareChunkData(currChunk))
                {
                    this->chunk_id = currChunk.chunk_id;
                    return;
                }
            }
            storage.read(reinterpret_cast<char *>(&nextChunk), sizeof(uint64_t));
            if (nextChunk == 0)
                break;
            storage.seekg(nextChunk);
        } // End for
    } // End if
    //Add to the begin of the chain
 
    storage.seekp(0, std::ios::end);
    uint64_t newHead = storage.tellp();
    StorageManager::totalChunks++;
    StorageManager::bucketListSize++;
    this->chunk_id = StorageManager::totalChunks;
    serialize(storage);
    storage.write(reinterpret_cast<const char *>(&listHead), sizeof(uint64_t));
    bucketList.seekp(bucketPos);
    bucketList.write(reinterpret_cast<const char *>(&newHead), sizeof(uint64_t));
}
