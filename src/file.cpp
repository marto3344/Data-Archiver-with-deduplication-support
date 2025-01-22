#include "file.hpp"
#include"fileChunk.hpp"
#include "storageManager.hpp"
void File::serialize(std::ostream &out) const//TODO::Better exception handling
{
    if(!out.good())
    {
        throw std::invalid_argument("Error! Output stream is bad!");
    }
    out.write(reinterpret_cast<const char*>(&last_modified),sizeof(fs::file_time_type));
    out.write(reinterpret_cast<const char*>(&size),sizeof(uintmax_t));
    size_t nameSize = name.size();

    out.write(reinterpret_cast<const char*>(&nameSize),sizeof(size_t));
    out.write(reinterpret_cast<const char*>(&name),sizeof(nameSize));

    size_t dataSize = chunk_list.size();
    out.write(reinterpret_cast<const char*>(&dataSize),sizeof(size_t));
    for (size_t i = 0; i < dataSize; i++)
    {
        out.write(reinterpret_cast<const char*>(&chunk_list[i].first),sizeof(uint64_t));
        out.write(reinterpret_cast<const char*>(&chunk_list[i].second),sizeof(uint64_t));
    }
    

}

void File::deserialize(std::istream &in)
{
    if(!in.good())
    {
        throw std::invalid_argument("Error! Input stream is bad!");
    }
    in.read(reinterpret_cast<char*>(&last_modified),sizeof(fs::file_time_type));
    in.read(reinterpret_cast<char*>(&size),sizeof(uintmax_t));
    size_t nameSize = 0;

    in.read(reinterpret_cast<char*>(&nameSize),sizeof(size_t));
    name.reserve(nameSize);
    in.read(reinterpret_cast<char*>(&name),sizeof(nameSize));

    size_t dataSize = 0;
    in.read(reinterpret_cast<char*>(&dataSize),sizeof(size_t));
    chunk_list.reserve(dataSize);
    for (size_t i = 0; i < dataSize; i++)
    {
        uint64_t hash, chunkId;

        in.read(reinterpret_cast<char*>(&hash),sizeof(uint64_t));
        in.read(reinterpret_cast<char*>(&chunkId),sizeof(uint64_t));
        chunk_list.push_back({hash,chunkId});
    }
}

bool File::extractFile(const fs::path &path, std::fstream &in) const
{
    return false;
}

bool File::updateFile(const fs::path &targerFile, std::ifstream &in)
{

    return false;
}

bool File::storeFile(const fs::path &file,std::fstream& bucketList, std::fstream &stoarge, const bool hashOnly) 
{
    if(hashFile(file,bucketList,stoarge, hashOnly))
    {
        StorageManager::totalFiles++;
        return true;
    };
    return false; 
}

void File::setName(const std::string &filename)
{
    name = filename;
}

bool File::hashFile(const fs::path &filePath, std::fstream& bucketList,std::fstream& storage,  const bool hashOnly)
{
    std::ifstream file(filePath, std::ios::binary);
    if(!file.is_open())
        return false;
    this->name = filePath.filename().string();
    this->last_modified = fs::last_write_time(filePath);
    this->size = fs::file_size(filePath);
    //Compute chunksize
    const uint32_t buffer_size = 1<<16;
    
    while (file.good()&&!file.eof()) {

        std::vector<uint8_t> buffer(buffer_size,0x00);
        FileChunk curr(buffer_size);
        file.read(reinterpret_cast<char *>(buffer.data()), buffer_size);
        curr.moveChunkData(buffer);
        curr.hashChunk();
        curr.storeChunk(storage,bucketList, hashOnly);
        chunk_list.push_back({curr.getHash(),curr.getId()});
    }
    bool result = !file.good()&&file.eof();
    file.close();
    return result;

}

