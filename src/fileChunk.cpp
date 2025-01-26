#include "fileChunk.hpp"
#include "storageManager.hpp"
#include<iostream>
extern "C" {
    #include "xxhash.h"
}
FileChunk::FileChunk()
{
    filesCount = 1;
    chunk_data = std::vector<uint8_t>();
}

FileChunk::FileChunk(const std::vector<uint8_t> &data)
{
    chunk_data = data;
    filesCount = 1;
}

void FileChunk::deserialize(std::istream &in)
{
    if(!in.good())
    {
        throw std::invalid_argument("Bad stream!"); 
    }
    in.read(reinterpret_cast<char*>(&filesCount),sizeof(uint32_t));
    in.read(reinterpret_cast<char*> (&hash),sizeof(uint64_t));
    in.read(reinterpret_cast<char*>(&chunk_id),sizeof(uint64_t));
    uint32_t size=0;
    in.read(reinterpret_cast<char*>(&size),sizeof(uint32_t));
    chunk_data.resize(size);
    in.read(reinterpret_cast<char*>(chunk_data.data()),size);
}

void FileChunk::serialize(std::ostream &out) const
{
    out.write(reinterpret_cast<const char*> (&filesCount),sizeof(uint32_t));
    out.write(reinterpret_cast<const char *> (&hash),sizeof(uint64_t));
    out.write(reinterpret_cast<const char*> (&chunk_id), sizeof(uint64_t));
    uint32_t size = (uint32_t)chunk_data.size();
    out.write(reinterpret_cast<const char*>(&size),sizeof(uint32_t));
    out.write(reinterpret_cast<const char*>(chunk_data.data()),size);
}

void FileChunk::writeChunkData(std::ofstream &out) const
{
    if(!out.is_open()||!out.good())
        return;
    out.write(reinterpret_cast<const char*>(chunk_data.data()),chunk_data.size());
}

void FileChunk::hashChunk()
{
    this->hash = XXH64(chunk_data.data(),chunk_data.size(),0);
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
    uint64_t nextChunk;
    bucketList.read(reinterpret_cast<char *>(&listHead), sizeof(uint64_t));
    if (listHead != 0)
    {
        FileChunk currChunk;
        storage.seekg(listHead);
        for (;;)
        {   

            if (storage.eof())
                break;
            currChunk.deserialize(storage);
            if (currChunk.hash == this->hash)
            {
                if (hashOnly)
                {
                    this->chunk_id = currChunk.chunk_id;
                    currChunk.filesCount ++;
                    storage.seekg(- (2*sizeof(uint32_t) + 2*sizeof(uint64_t) + currChunk.chunk_data.size()),std::ios::cur);
                    storage.seekp(storage.tellg());
                    storage.write(reinterpret_cast<const char*>(&currChunk.filesCount),sizeof(uint32_t));
                    return;
                }
                if (compareChunkData(currChunk))
                {
                    this->chunk_id = currChunk.chunk_id;
                    currChunk.filesCount ++;
                    storage.seekg(- (2*sizeof(uint32_t) + 2*sizeof(uint64_t) + currChunk.chunk_data.size()),std::ios::cur);
                    storage.seekp(storage.tellg());
                    storage.write(reinterpret_cast<const char*>(&currChunk.filesCount),sizeof(uint32_t));
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
    StorageManager::lastChunkId++;
    StorageManager::bucketListSize++;
    this->chunk_id = StorageManager::lastChunkId;
    serialize(storage);
    storage.write(reinterpret_cast<const char *>(&listHead), sizeof(uint64_t));
    bucketList.seekp(bucketPos);
    bucketList.write(reinterpret_cast<const char *>(&newHead), sizeof(uint64_t));
}


