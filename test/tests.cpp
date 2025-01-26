
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
#define SAMPLE_ARCHIVE "../../test/sample"
#define SAMPLE_ARCHIVE_TREE_PATH "../../data/archives_data/sampleArchive.dat"
TEST_SUITE("File chunk class test")
{
    TEST_CASE("Default constructor")
    {
        FileChunk f;
        CHECK(f.getFilesCount()==1);
        CHECK(f.getChunkSize()==0);
    }
    TEST_CASE("Constructing with vector")
    {
        std::vector<uint8_t>sample_data= {0,1,2,11,3,5,8,255};
        FileChunk f(sample_data);
        CHECK(f.getChunkSize()==sample_data.size());
        sample_data.pop_back();
        CHECK(f.getChunkSize()!=sample_data.size());
    }
    TEST_CASE("Hash tets")
    {     
        std::vector<uint8_t>sample_data= {0,1,2,11,3,5,8,255};
        FileChunk f(sample_data);
        FileChunk g (sample_data);
        f.hashChunk();
        g.hashChunk();
        CHECK(f.getHash()==g.getHash());
       
    }
    TEST_CASE("write chunk data test")
    {
        std::vector<uint8_t> sample_data = {0, 1, 2, 11, 3, 5, 8, 255};
        FileChunk f(sample_data);
        FileChunk nullChunk;
        CHECK(!f.compareChunkData(nullChunk));
        std::ofstream out("output.dat", std::ios::binary | std::ios::trunc);
        f.writeChunkData(out);
        out.close();

        std::ifstream in("output.dat", std::ios::binary);
        std::vector<uint8_t> fileData(sample_data.size(), 0);
        in.read(reinterpret_cast<char *>(fileData.data()), sample_data.size());
        in.close();
        FileChunk g(fileData);
        f.hashChunk();
        g.hashChunk();
        CHECK(f.getHash()==g.getHash());
        CHECK(f.getChunkSize()==g.getChunkSize());
        CHECK(f.compareChunkData(g));
        CHECK(!g.compareChunkData(nullChunk));
    }
    TEST_CASE("serializiation and deserialization")
    {
        std::vector<uint8_t> sample_data = {0, 1, 2, 11, 3, 5, 8, 255};
        FileChunk f(sample_data);
        f.setId(0);
        f.hashChunk();
        std::ofstream out("output.dat", std::ios::binary | std::ios::trunc);
        f.serialize(out);
        out.close();
        FileChunk g;
        std::ifstream in ("output.dat",std::ios::binary);
        g.deserialize(in);
        in.close();
        CHECK(f==g);       

    }
}

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
        CHECK(StorageManager::checkStorageSetup());
    }
    TEST_CASE("Test create from empty storage")
    {
        std::string archiveName = "sampleArchive";
        std::set<fs::path>paths;
        CHECK(fs::exists(SAMPLE_ARCHIVE));
        paths.insert(SAMPLE_ARCHIVE);
        StorageManager::CreateArchive(0,archiveName,paths);
        CHECK(StorageManager::ArchiveExists(SAMPLE_ARCHIVE_TREE_PATH ));
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
        CHECK(f.empty());
    };
    TEST_CASE("operator== test")
    {
        Archive sample;
        fs::path sampleArchivePath(SAMPLE_ARCHIVE_TREE_PATH );
        CHECK(fs::exists(sampleArchivePath));
        std::ifstream file (sampleArchivePath,std::ios::binary);
        sample.readFromFile(file);
        file.close();
        CHECK(!sample.empty());
        CHECK(sample == sample);

        Archive emptyArchive;
        emptyArchive.setDateArchived(sample.getDateArchived());
        emptyArchive.setName(sample.getName());
        CHECK(emptyArchive != sample);
        
    }
    TEST_CASE("Check Archive read"){
        Archive sample;
        fs::path sampleArchivePath(SAMPLE_ARCHIVE_TREE_PATH );
        CHECK(fs::exists(sampleArchivePath));
        std::ifstream file (sampleArchivePath,std::ios::binary);
        sample.readFromFile(file);
        file.seekg(std::ios::beg);
        Archive sampleCopy;
        sampleCopy.readFromFile(file);
        file.close();
        CHECK(sample==sampleCopy);
    }
    TEST_CASE("Check write")
    {
        Archive sample;
        fs::path sampleArchivePath(SAMPLE_ARCHIVE_TREE_PATH );
        CHECK(fs::exists(sampleArchivePath));
        uintmax_t initialSize = fs::file_size(sampleArchivePath);
        std::ifstream file (sampleArchivePath,std::ios::binary);
        sample.readFromFile(file);
        file.close();
        std::ofstream out(sampleArchivePath,std::ios::binary);
        sample.writeToFile(out);
        out.close();
        Archive sample2;
        std::ifstream rewritedFile (sampleArchivePath,std::ios::binary);
        sample2.readFromFile(rewritedFile);
        rewritedFile.close();
        uintmax_t newSize = fs::file_size(sampleArchivePath);
        CHECK(newSize==initialSize);
        CHECK(sample == sample2);
    }
    TEST_CASE("Test copy constructor ")
    {
        Archive sample;
        fs::path sampleArchivePath(SAMPLE_ARCHIVE_TREE_PATH );
        CHECK(fs::exists(sampleArchivePath));
        std::ifstream file (sampleArchivePath,std::ios::binary);
        sample.readFromFile(file);
        file.close();
        sample.setName("sampleArchive");
        Archive sampleCopy (sample);
        CHECK(sample == sampleCopy);
        sampleCopy.setName("sample2");
        CHECK(sample.getName()!=sampleCopy.getName());
        CHECK(sample !=sampleCopy);
    }
};

int main(){
    doctest::Context().run();    
}