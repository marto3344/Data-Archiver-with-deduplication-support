#include "storageManager.hpp"
#define ARCHIVES_DATA_PATH "../data/archives_data/"
#define STORAGE_METADATA "../data/storage_metadata.dat"
#define STORAGE_BUCKETLIST "../data/storage_bucket_list.dat"
#define STORAGE_CHAINS "../data/storage.dat"
#include<filesystem>
#include<set>
#include<fstream>
#include "file.hpp"
namespace fs =std::filesystem;
uint64_t StorageManager::totalChunks = 0;
uint64_t StorageManager:: totalFiles = 0;
uint64_t StorageManager::bucketListSize = 0;
uint64_t StorageManager::bucketListCapacity = 1<<17;//Default capacity for 1MB list size
void StorageManager::CreateArchive(const bool &hashOnly, const std::string &name, std::set<fs::path> &dirs)
{
    fs::path archivesData (ARCHIVES_DATA_PATH);
    if(!fs::exists(archivesData))
    {
        fs::create_directory(archivesData);
    }
    fs::path archivePath (ARCHIVES_DATA_PATH);
    std::string filename = name;
    // if(ArchiveExists(archivePath))
    // {
    //     std::cout<<"Archive with that name already exists! Please use another!"<<'\n';
    //     return;
    // }
    // std::ofstream out (filePath.string(),std::ios::binary | std::ios::app);
    readMetadata();
    //Archive a;
    //filter overlapping directories -> ?
    //a.CreateFromDirectories();
    //a.deserialize();
    // --
    fs::path testPath("D:\\razni\\hash-test\\18_Martin_Stoyanov_12B.pptx");
    std::fstream storage(STORAGE_CHAINS, std::ios::in | std::ios::out | std::ios::binary);
    std::fstream bucketList (STORAGE_BUCKETLIST,std::ios::in | std::ios::out | std::ios::binary);
    File f;
    f.storeFile(testPath,bucketList,storage,hashOnly);
    storage.close();
    bucketList.close();
    //--
    StorageStatistic();
    writeMetadata();
    filename.append(".dat");
    archivePath.append(filename);
    
    // if(fs::exists(filePath))
    // {
    //     std::cout<<"Archive created successfully"<<'\n';
        
    // }
    // out.close();
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
    std::cout<<"Total chunks stored in storage: "<<StorageManager::totalChunks<<'\n';
}

void StorageManager::readMetadata()
{
    std::ifstream in (STORAGE_METADATA);
    if(!in.is_open())
    {
        throw std::runtime_error("Cannot read storage meata data!");
    }
    in.read(reinterpret_cast<char*>(&StorageManager::totalFiles),sizeof(uint64_t));
    in.read(reinterpret_cast<char*>(&StorageManager::totalChunks),sizeof(uint64_t));
    in.read(reinterpret_cast<char*>(&StorageManager::bucketListSize),sizeof(uint64_t));
    in.read(reinterpret_cast<char*>(&StorageManager::bucketListCapacity),sizeof(uint64_t));
    in.close();
}

void StorageManager::writeMetadata()
{
    std::ofstream out(STORAGE_METADATA);
    if(!out.is_open())
    {
        throw std::runtime_error("Cannot write storage meta data");
    }
    out.write(reinterpret_cast<const char*>(&StorageManager::totalFiles),sizeof(uint64_t));
    out.write(reinterpret_cast<const char*>(&StorageManager::totalChunks),sizeof(uint64_t));
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