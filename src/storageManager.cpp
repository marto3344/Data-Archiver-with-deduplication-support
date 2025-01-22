#define ARCHIVES_DATA_PATH "../data/archives_data/"
#define STORAGE_METADATA "../data/storage_metadata.dat"
#define STORAGE_BUCKETLIST "../data/storage_bucket_list.dat"
#define STORAGE_CHAINS "../data/storage.dat"
#include<filesystem>
#include<set>
#include<fstream>
#include "storageManager.hpp"
#include "archive.hpp"
#include "file.hpp"
namespace fs =std::filesystem;
uint64_t StorageManager::lastChunkId = 0;
uint64_t StorageManager:: totalFiles = 0;
uint64_t StorageManager::bucketListSize = 0;
uint64_t StorageManager::bucketListCapacity = 1<<17;//Default capacity for 1MB list size
void StorageManager::CreateArchive(const bool &hashOnly, const std::string &name, std::set<fs::path> &dirs)
{
    readMetadata();
    std::string filename = name;
    filename.append(".dat");
    fs::path archivePath (ARCHIVES_DATA_PATH);
    archivePath.append(filename);
    Archive a;
    //--Fast deserialization test--
    // std::ifstream in (archivePath.string(),std::ios::binary);
    // a.readFromFile(in);
    // a.dfsPrint();
    // in.close();
    //return;
    if(ArchiveExists(archivePath))
    {
        std::cout<<"Archive with that name already exists! Please use another!"<<'\n';
        return;
    }
    std::ofstream out (archivePath.string(),std::ios::binary);
    if(!out.is_open())
    {
        std::cout<<"Cannot create the archive!";
        return;
    }
  
    std::fstream storage(STORAGE_CHAINS, std::ios::in | std::ios::out | std::ios::binary);
    std::fstream bucketList (STORAGE_BUCKETLIST,std::ios::in | std::ios::out | std::ios::binary);
    std::vector<fs::path> oneDirectory;
    oneDirectory.push_back(*dirs.begin());
    a.CreateFromDirectoryList(oneDirectory,bucketList,storage,hashOnly);
    storage.close();
    bucketList.close();
    writeMetadata();
    a.writeToFile(out);
    out.close();
    StorageStatistic();
}

void StorageManager::ExtraxtArchive(const std::string &name, const fs::path &targetPath, const fs::path &archivePath)
{
    std::string filename = name;
    filename.append(".dat");
    fs::path archive(ARCHIVES_DATA_PATH);
    archive.append(filename);
    if(!ArchiveExists(archive))
    {
        std::cout<<"Archive with that name was not found in storage!\n";
        return;
    }
    std::ifstream in(archive,std::ios::binary);
    if(!in.is_open())
    {
        std::cout<<"Could not open the archive inforamtion file!\n";
        return;
    }
    Archive a;
    try
    {
        a.readFromFile(in);    
    }
    catch(const std::exception& e)
    {
        if(in.is_open())
            in.close();
        std::cerr << e.what() << '\n';
    }
    catch(...)
    {
        std::cout<<"Something went wrong during archive fetch!\n";
        if(in.is_open())
            in.close();
    }
    std::fstream storage(STORAGE_CHAINS, std::ios::in | std::ios::out | std::ios::binary);
    std::fstream bucketList (STORAGE_BUCKETLIST,std::ios::in | std::ios::out | std::ios::binary);
    try
    {
       a.ExtractArchive(targetPath,archivePath,bucketList,storage);
    }
    catch(const std::exception& e)
    {
        std::cerr << e.what() << '\n';
        storage.close();
        bucketList.close();
    }
    catch(...)
    {
        std::cout<<"Unlucky :(\n";
        storage.close();
        bucketList.close();
    }
    

}

void StorageManager::InitializeStorage()
{
    fs::path metadataPath (STORAGE_METADATA);
    writeMetadata();
    initializeBucketList();
    initializeStorageChains();
}

void StorageManager::StorageStatistic()
{
    std::cout<<"Total files stored in storage: "<<StorageManager::totalFiles<<'\n';
    std::cout<<"Total chunks stored in storage: "<<StorageManager::bucketListSize<<'\n';
}

void StorageManager::readMetadata()
{
    std::ifstream in (STORAGE_METADATA,std::ios::binary);
    if(!in.is_open())
    {
        throw std::runtime_error("Cannot read storage meata data!");
    }
    in.read(reinterpret_cast<char*>(&StorageManager::totalFiles),sizeof(uint64_t));
    in.read(reinterpret_cast<char*>(&StorageManager::lastChunkId),sizeof(uint64_t));
    in.read(reinterpret_cast<char*>(&StorageManager::bucketListSize),sizeof(uint64_t));
    in.read(reinterpret_cast<char*>(&StorageManager::bucketListCapacity),sizeof(uint64_t));
    in.close();
    
}

void StorageManager::writeMetadata()
{
    std::ofstream out(STORAGE_METADATA, std::ios::binary);
    if(!out.is_open())
    {
        throw std::runtime_error("Cannot write storage meta data");
    }
    out.write(reinterpret_cast<const char*>(&StorageManager::totalFiles),sizeof(uint64_t));
    out.write(reinterpret_cast<const char*>(&StorageManager::lastChunkId),sizeof(uint64_t));
    out.write(reinterpret_cast<const char*>(&StorageManager::bucketListSize),sizeof(uint64_t));
    out.write(reinterpret_cast<const char*>(&StorageManager::bucketListCapacity),sizeof(uint64_t));
    out.close();
}

bool StorageManager::ArchiveExists(const fs::path &archivePath)
{
    return fs::exists(archivePath);
}



void StorageManager::initializeBucketList()
{
    std::ofstream out(STORAGE_BUCKETLIST, std::ios::trunc | std::ios::binary);
    if(!out.is_open())
        throw std::runtime_error("Coudn't initialize storage bucket list!");
    uint64_t emptyBucketVal = 0;
    for (int i = 0; i < StorageManager::bucketListCapacity; i++)
    {
        out.write(reinterpret_cast<const char*>(&emptyBucketVal),sizeof(uint64_t));
    }
    out.close();
}
void StorageManager::initializeStorageChains(){
    fs::path storageChainsPath (STORAGE_CHAINS);
    std::ofstream chainsFile(storageChainsPath,std::ios::trunc | std::ios::binary);
    if(!chainsFile.is_open())
        throw std::runtime_error("Error initializing storage information!");
    uint8_t dummyByte = 0x00;
    for(int i = 0; i<2; ++i)
    {
        chainsFile.write(reinterpret_cast<const char*>(&dummyByte), sizeof(uint8_t));
    }
    chainsFile.close();
}