#include "file.hpp"
#include"fileChunk.hpp"
#include "storageManager.hpp"
void File::serialize(std::ostream &out) const
{
    if(!out.good())
    {
        throw std::invalid_argument("Error! Output stream is bad!");
    }
    out.write(reinterpret_cast<const char*>(&last_modified),sizeof(fs::file_time_type));
    out.write(reinterpret_cast<const char*>(&size),sizeof(uintmax_t));
    size_t nameSize = name.size();

    out.write(reinterpret_cast<const char*>(&nameSize),sizeof(size_t));
    out.write(reinterpret_cast<const char*>(name.data()),nameSize);

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
    name = std::string(nameSize,'\0');
    in.read(reinterpret_cast<char*>(name.data()),nameSize);

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

bool File::extractFile(std::ofstream& file,std::fstream& storage,std::fstream& bucketList) const
{
    if(!file.is_open()||!storage.is_open()||!bucketList.is_open())
        return false;
    uintmax_t bytesWritten = 0;
    FileChunk curr;
    uint32_t bucketPos;
    uint64_t listHead;
    uint64_t nextChunk;
    for (int i = 0; i < chunk_list.size(); i++)
    {
        bucketPos = (chunk_list[i].first % StorageManager::bucketListCapacity) * sizeof(uint64_t);
        bucketList.seekg(bucketPos);
        listHead;
        bucketList.read(reinterpret_cast<char *>(&listHead), sizeof(uint64_t));
        if (listHead != 0)
        {
            storage.seekg(listHead);
            for (;;)
            {
                
                if (storage.eof())
                    break;
                curr.deserialize(storage);
                if (curr.getId() == chunk_list[i].second)
                    break;
                storage.read(reinterpret_cast<char *>(&nextChunk), sizeof(uint64_t));
                if (nextChunk == 0)
                    break;
                storage.seekg(nextChunk);
            } 
       } // End if
        uintmax_t chunkSize = std::min(size - bytesWritten, (uintmax_t)curr.getChunkSize());
        curr.writeChunkData(file, chunkSize);
        bytesWritten += chunkSize;
    } 
    return true;
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

void File::markFileDeleted(std::fstream &bucketList, std::fstream &storage)
{
    if(!bucketList.is_open()||!storage.is_open())
    {
        return;
    }
    FileChunk curr;
    uint64_t listHead;
    uint32_t bucketPos;
    uint64_t nextChunk;
    for (int i = 0; i < chunk_list.size(); i++)
    {
        
        bucketPos = (chunk_list[i].first % StorageManager::bucketListCapacity) * sizeof(uint64_t);
        bucketList.seekg(bucketPos);
        
        bucketList.read(reinterpret_cast<char *>(&listHead), sizeof(uint64_t));
        if (listHead != 0)
        {
            storage.seekg(listHead);
            for (;;)
            {
               
                if (storage.eof())
                    break;
                curr.deserialize(storage);
                if (curr.getId() == chunk_list[i].second)
                {
                    uint32_t newFilesCount = curr.getFilesCount();
                    if(newFilesCount)
                        newFilesCount--;
                    storage.seekg(- (2*sizeof(uint32_t) + 2* sizeof(uint64_t) + curr.getChunkSize()),std::ios::cur);
                    storage.seekp(storage.tellg());
                    storage.write(reinterpret_cast<const char*>(&newFilesCount),sizeof(uint32_t));
                    break;
                }
                storage.read(reinterpret_cast<char *>(&nextChunk), sizeof(uint64_t));
                if (nextChunk == 0)
                    break;
                storage.seekg(nextChunk);
            } 
       } // End if
      
    } 
}

bool File::operator==(const File &other) const
{
    return(name == other.name)&&(size == other.size)&&(last_modified ==other.last_modified)&&(chunk_list == other.chunk_list);
}

bool File::hashFile(const fs::path &filePath, std::fstream& bucketList,std::fstream& storage,  const bool hashOnly)
{
    std::ifstream file(filePath, std::ios::binary);
    if(!file.is_open())
        return false;
    this->name = filePath.filename().string();
    this->last_modified = fs::last_write_time(filePath);
    this->size = fs::file_size(filePath);
    uint32_t maxChunkSize = (1<<25);//32MB
    uint32_t avgChunkSize;
    if(size<(1<<19))
    {
        avgChunkSize = size;
    }
    else{
        avgChunkSize = std::min((uint32_t)(size / 10), maxChunkSize );
    }
    uintmax_t bytesRead = 0;
    std::vector<uint8_t>buffer(avgChunkSize);
    uintmax_t chunkSize;
    while (bytesRead<size && file.good()) {
        chunkSize = std::min((uint32_t)(size - bytesRead),avgChunkSize);
        if(chunkSize<avgChunkSize)
        {
            buffer.resize(chunkSize);
        }
        file.read(reinterpret_cast<char*>(buffer.data()),chunkSize);
        FileChunk curr(buffer);
        curr.hashChunk();
        curr.storeChunk(storage,bucketList, hashOnly);
        chunk_list.push_back({curr.getHash(),curr.getId()});
        bytesRead +=curr.getChunkSize();
    }
    if(bytesRead < size)
    {
        file.close();
        std::string message = "Something went wrong during file read! File name: ";
        message.append(name);
        std::cout<<message<<'\n';
        return false;
    }
    bool result = file.good();
    file.close();
    return result;

}
