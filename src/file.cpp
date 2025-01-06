#include "file.hpp"

void File::writeMetaData(std::ofstream &out)//TODO::Better exception handling
{
    if(!out.is_open())
    {
        throw std::invalid_argument("file not opened");
    }
    out.write(reinterpret_cast<const char*>(&last_modified),sizeof(time_point));
    out.write(reinterpret_cast<const char*>(&id), sizeof(uint32_t));
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
    in.read(reinterpret_cast<char*>(&id), sizeof(uint32_t));
    in.read(reinterpret_cast<char*>(&size),sizeof(uint64_t));
    size_t nameSize = 0;
    
    name.reserve(nameSize);
    in.read(reinterpret_cast<char*>(&nameSize),sizeof(size_t));
    in.read(reinterpret_cast<char*>(&name),sizeof(nameSize));

    size_t dataSize = 0;
    in.read(reinterpret_cast<char*>(&dataSize),sizeof(size_t));
    chunks.reserve(dataSize);
    for (size_t i = 0; i < dataSize; i++)
    {
        in.read(reinterpret_cast<char*>(&chunks[i].first),sizeof(uint64_t));
        in.read(reinterpret_cast<char*>(&chunks[i].second),sizeof(uint64_t));
    }
}