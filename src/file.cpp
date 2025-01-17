#include "file.hpp"
#include"fileChunk.hpp"

void File::writeMetaData(std::ofstream &out) const//TODO::Better exception handling
{
    if(!out.is_open())
    {
        throw std::invalid_argument("file not opened");
    }
    out.write(reinterpret_cast<const char*>(&last_modified),sizeof(time_point));
    out.write(reinterpret_cast<const char*>(&size),sizeof(uint64_t));
    size_t nameSize = name.size();

    out.write(reinterpret_cast<const char*>(&nameSize),sizeof(size_t));
    out.write(reinterpret_cast<const char*>(&name),sizeof(nameSize));

    size_t dataSize = chunks.size();
    out.write(reinterpret_cast<const char*>(&dataSize),sizeof(size_t));
    for (size_t i = 0; i < dataSize; i++)
    {
        out.write(reinterpret_cast<const char*>(&chunks[i].first),sizeof(uint64_t));
        out.write(reinterpret_cast<const char*>(&chunks[i].second),sizeof(uint64_t));
    }
    

}

void File::readMetaData(std::ifstream &in)
{
    if(!in.is_open())
    {
        throw std::invalid_argument("file not opened");
    }
    in.read(reinterpret_cast<char*>(&last_modified),sizeof(time_point));
    in.read(reinterpret_cast<char*>(&size),sizeof(uint64_t));
    size_t nameSize = 0;

    in.read(reinterpret_cast<char*>(&nameSize),sizeof(size_t));
    name.reserve(nameSize);
    in.read(reinterpret_cast<char*>(&name),sizeof(nameSize));

    size_t dataSize = 0;
    in.read(reinterpret_cast<char*>(&dataSize),sizeof(size_t));
    chunks.reserve(dataSize);
    for (size_t i = 0; i < dataSize; i++)
    {
        uint64_t hash, chunkId;

        in.read(reinterpret_cast<char*>(&hash),sizeof(uint64_t));
        in.read(reinterpret_cast<char*>(&chunkId),sizeof(uint64_t));
        chunks.push_back({hash,chunkId});
    }
}

bool File::extractFile(const fs::path &path, std::ifstream &in) const
{
    return false;
}

bool File::updateFile(const fs::path &targerFile, std::ifstream &in)
{

    return false;
}

bool File::storeFile(const fs::path &file, std::ofstream &stoarge, uint32_t bucketListCapacity, uint32_t& bucketListSize) 
{
    return hashFile(file,stoarge,bucketListCapacity,bucketListSize); 
}

bool File::hashFile(const fs::path &filePath, std::ofstream &storage, uint32_t bucketListCapacity, uint32_t &bucketListSize)
{
    std::ifstream file(filePath, std::ios::binary);
    if(!file.is_open())
        return false;

    const uint32_t buffer_size = FileChunk::getChunkSize();
    
    uintmax_t filesize = fs::file_size(filePath);
   
    size_t total_chunks = 0;
    
    while (file.good()&&!file.eof()) {

        std::vector<uint8_t> buffer(buffer_size,0x00);
        FileChunk curr;
        file.read(reinterpret_cast<char *>(buffer.data()), buffer_size);
        curr.moveChunkData(buffer);
        curr.hashChunk();
        std::cout<<"hash: value:"<<curr.getHash()<<'\n';
        total_chunks++;
    }
    std::cout<<total_chunks;
    bool result = !file.good()&&file.eof();
    file.close();
    return result;

}

