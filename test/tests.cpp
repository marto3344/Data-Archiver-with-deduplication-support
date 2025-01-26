
#define DOCTEST_CONFIG_IMPLEMENT
#include "doctest.h"
#include<vector>
#include<set>
#include<string>
#include<filesystem>
#include "archive.hpp"
#include "storageManager.hpp"
#include "file.hpp"
#include "fileChunk.hpp"
namespace fs = std::filesystem;
#define ARCHIVES_DATA_PATH "../../data/archives_data/"
#define STORAGE_METADATA "../../data/storage_metadata.dat"
#define STORAGE_BUCKETLIST "../../data/storage_bucket_list.dat"
#define STORAGE_CHAINS "../../data/storage.dat"
#define DATA_PATH "../../data"
#define SAMPLE_ARCHIVE "../../test/sample"
TEST_SUITE("Storage basic methods part1")
{
    TEST_CASE("Test Initialize")
    {
        StorageManager::InitializeStorage();
        StorageManager::readMetadata();
        CHECK(StorageManager::bucketListSize == 0);
        CHECK(StorageManager::bucketListCapacity == (1<<17));
        CHECK(StorageManager::lastChunkId == 0);
        CHECK(StorageManager::totalFiles == 0);
        CHECK(StorageManager::checkStorageSetup());   
    }
    TEST_CASE("Test delete")
    {
        StorageManager::DeleteStorage();
        CHECK(fs::is_empty(ARCHIVES_DATA_PATH));
        CHECK(StorageManager::checkStorageSetup());
    }
    TEST_CASE("Test create from empty storage")
    {
        std::string archiveName = "sampleArchive";
        std::set<fs::path>paths;
        CHECK(fs::exists(SAMPLE_ARCHIVE));
        paths.insert(SAMPLE_ARCHIVE);
        StorageManager::CreateArchive(0,archiveName,paths);
        CHECK(fs::exists("../../data/archives_data/sampleArchive.dat"));
        CHECK(StorageManager::ArchiveExists("../../data/archives_data/sampleArchive.dat"));
        if(fs::is_empty(SAMPLE_ARCHIVE))
        {
            CHECK(StorageManager::bucketListSize == 0);
            CHECK(StorageManager::bucketListCapacity == (1<<17));
            CHECK(StorageManager::lastChunkId == 0);
            CHECK(StorageManager::totalFiles == 0);
        }
        else{
            CHECK(StorageManager::bucketListSize > 0);
            CHECK(StorageManager::bucketListCapacity == (1<<17));
            CHECK(StorageManager::lastChunkId > 0);
            CHECK(StorageManager::totalFiles > 0);
        }
    }
}
TEST_SUITE("Storage manager helper methods")
{
   
    TEST_CASE("Remove overlapping paths")
    {
        std::set<fs::path> paths;
        paths.insert("D:/A");
        paths.insert("D:/A/B"); 
        paths.insert("D:/A/C");
        paths.insert("D:/B");
        paths.insert("D:/B/X");
        paths.insert("D:/C");
        paths.insert("C:/");
        
        std::vector<fs::path> filteredPaths;
        StorageManager::removeOverlappingPaths(filteredPaths,paths);
        CHECK(filteredPaths.size() == 4);
        CHECK(filteredPaths[0].string() == "C:/" );
        CHECK(filteredPaths[1].string() == "D:/A");
        CHECK(filteredPaths[2].string() == "D:/B" );
        CHECK(filteredPaths[3].string() == "D:/C");
    }

}
TEST_SUITE ("ARCHIVE CLASS TEST")
{
    TEST_CASE("Default Constructor")
    {
        Archive f;
        CHECK(f.empty() == true);
    };
    TEST_CASE("operator== test")
    {
        Archive sample;
        fs::path sampleArchivePath("../../data/archives_data/sampleArchive.dat");
        CHECK(fs::exists(sampleArchivePath));
        std::ifstream file (sampleArchivePath,std::ios::binary);
        sample.readFromFile(file);
        file.close();
        CHECK(!sample.empty());
        CHECK(sample == sample);
        
    }
    TEST_CASE("Test copy constructor ")
    {
        Archive sample;
        fs::path sampleArchivePath("../../data/archives_data/sampleArchive.dat");
        CHECK(fs::exists(sampleArchivePath));
        std::ifstream file (sampleArchivePath,std::ios::binary);
        sample.readFromFile(file);
        file.close();
        sample.setName("sampleArchive");
        Archive sample2 (sample);
        CHECK(sample == sample2);
        sample2.setName("sample2");
        CHECK(sample.getName()!=sample2.getName());
        CHECK(sample !=sample2);
    }
    

};

int main(){
    doctest::Context().run();
}