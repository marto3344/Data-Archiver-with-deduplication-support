#include "storageManager.hpp"
#define ARCHIVES_DATA_PATH "../data/archives_data/"
#define STORAGE_METADATA "../data/storage_metadata.dat"
#define STORAGE_BUCKETLIST "../data/storage_bucket_list.dat"
#include<filesystem>
#include<set>
#include<fstream>
namespace fs =std::filesystem;

void StorageManager::CreateArchive(const bool &hashOnly, const std::string &name, std::set<fs::path> &dirs)
{
    fs::path archivesData (ARCHIVES_DATA_PATH);
    if(!fs::exists(archivesData))
    {
        fs::create_directory(archivesData);
    }
    initializeStorage();
    fs::path filePath (ARCHIVES_DATA_PATH);
    std::string filename = name;
    filename.append(".dat");
    filePath.append(filename);
    if(fs::exists(filePath))
    {
        std::cout<<"Archive with that name already exists! Please use another!"<<'\n';
        return;
    }
    std::ofstream out (filePath.string(),std::ios::binary | std::ios::app);
  
    for (auto it = dirs.begin(); it != dirs.end(); ++it)
    {
       const fs::path& currFile = *it;
       std::cout << currFile.string() << std::endl;
    }
    
    if(fs::exists(filePath))
    {
        std::cout<<"Archive created successfully"<<'\n';
        
    }
    //Calculate block properties
    //Store it depending on hash-only?
    out.close();
}

void StorageManager::initializeStorage()
{
    fs::path metadataPath (STORAGE_METADATA);

    std::ofstream out(metadataPath, std::ios::trunc | std::ios::binary);
    if(!out.is_open())
        throw std::runtime_error("Cannot initialize storage information!");

    uint64_t files = 0;
    uint64_t chunks = 0;
    out.write(reinterpret_cast<const char*> (&files),sizeof(uint64_t));
    out.write(reinterpret_cast<const char*> (&chunks),sizeof(uint64_t));
    out.close();
    initializeBucketList();
}

void StorageManager::initializeBucketList() 
{
    std::ofstream out(STORAGE_BUCKETLIST, std::ios::trunc | std::ios::binary);
    uint64_t emptyBucketVal = 0;
    uint32_t size = 0;
    uint32_t capacity = 1<<17; //This is for 1MB bucket list size
    out.write(reinterpret_cast<const char*>(&size),sizeof(uint32_t));
    out.write(reinterpret_cast<const char*>(&capacity),sizeof(uint32_t));
    for (int i = 0; i < capacity; i++)
    {
        out.write(reinterpret_cast<const char*>(&emptyBucketVal),sizeof(uint64_t));
    }
    out.close();
}
