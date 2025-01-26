#include<filesystem>
#include<set>
#include<fstream>
#include<string>
#include<stack>
#include "storageManager.hpp"
#include "archive.hpp"
#include "file.hpp"
#define SRC (fs::path(__FILE__))
#define ARCHIVES_DATA_PATH (SRC.append( "../../data/archives_data/").string())
#define STORAGE_METADATA (SRC.append("../../data/storage_metadata.dat").string())
#define STORAGE_BUCKETLIST (SRC.append("../../data/storage_bucket_list.dat").string())
#define STORAGE_CHAINS (SRC.append("../../data/storage.dat").string())
#define DATA_PATH (SRC.append("../../data").string())
namespace fs =std::filesystem;
namespace StorageManager
{
    uint64_t lastChunkId = 0;
    uint64_t totalFiles = 0;
    uint64_t bucketListSize = 0;
    uint64_t bucketListCapacity = 1 << 17; // Default capacity for 1MB list size
    void CreateArchive(const bool &hashOnly, const std::string &name, const std::set<fs::path> &dirs)
    {
        if (!checkStorageSetup())
        {
            std::cout << "Run command \'initialize\' to create all necessary files!\n";
            return;
        }
        readMetadata();
        std::string filename = name;
        filename.append(".dat");
        fs::path archivePath(ARCHIVES_DATA_PATH);
        archivePath.append(filename);
        Archive a;
        if (ArchiveExists(archivePath))
        {
            std::cout << "Archive with that name already exists! Please use another!" << '\n';
            return;
        }
        std::ofstream out(archivePath.string(), std::ios::binary);
        if (!out.is_open())
        {
            std::cout << "Cannot create the archive!";
            return;
        }

        std::fstream storage(STORAGE_CHAINS, std::ios::in | std::ios::out | std::ios::binary);
        std::fstream bucketList(STORAGE_BUCKETLIST, std::ios::in | std::ios::out | std::ios::binary);
        std::vector<fs::path> filteredDirectories;
        try
        {
            removeOverlappingPaths(filteredDirectories, dirs);
            bool result = a.CreateFromDirectoryList(filteredDirectories, bucketList, storage, hashOnly);
            writeMetadata();
            if(result)
            {
                storage.close();
                bucketList.close();
                a.writeToFile(out);
                out.close();
                StorageManager::StorageStatistic();
                std::cout<<"Archive created successfully!\n";

            }
            else{
                out.close();
                fs::remove(archivePath);
                std::cout<<"Operation terminated!\n";
            }
        }
        catch(...)
        {
            std::cout<<"Something went wrong!";
            storage.close();
            bucketList.close();
            out.close();
        }
        
      
    }

    void ExtraxtArchive(const std::string &name, const fs::path &targetPath, const std::set<fs::path>& archivePaths)
    {
        if (!checkStorageSetup())
        {
            std::cout << "Run command \'initialize\' to create all necessary files!\n";
            return;
        }
        std::string filename = name;
        filename.append(".dat");
        fs::path archive(ARCHIVES_DATA_PATH);
        archive.append(filename);
        if (!ArchiveExists(archive))
        {
            std::cout << "Archive with that name was not found in storage!\n";
            return;
        }
        std::ifstream in(archive, std::ios::binary);
        if (!in.is_open())
        {
            std::cout << "Could not open the archive inforamtion file!\n";
            return;
        }
        Archive a;
        try
        {
            a.readFromFile(in);
            in.close();
        }
        catch (const std::exception &e)
        {
            if (in.is_open())
                in.close();
            std::cout<<"Error durig reading archive information!Error message:\n";
            std::cerr << e.what() << '\n';
        }
        catch (...)
        {
            std::cout << "Something went wrong during archive fetch!\n";
            if (in.is_open())
                in.close();
        }
        std::fstream storage(STORAGE_CHAINS, std::ios::in | std::ios::out | std::ios::binary);
        std::fstream bucketList(STORAGE_BUCKETLIST, std::ios::in | std::ios::out | std::ios::binary);
        try
        {   
            
            a.ExtractArchive(targetPath,archivePaths, bucketList, storage);
            storage.close();
            bucketList.close();
            std::cout<<"Archive extracted successfully!\n";
        }
        catch (const std::exception &e)
        {
            std::cerr << e.what() << '\n';
            storage.close();
            bucketList.close();
        }
        catch (...)
        {
            std::cout << "Something went wrong during extraction!\n";
            storage.close();
            bucketList.close();
        }
    }

    void CheckArchive(const std::string &name, const fs::path &targetPath, const fs::path &archivePath)
    {
         if (!checkStorageSetup())
        {
            std::cout << "Run command \'initialize\' to create all necessary files!\n";
            return;
        }
        std::string filename = name;
        filename.append(".dat");
        fs::path archive(ARCHIVES_DATA_PATH);
        archive.append(filename);
        if (!ArchiveExists(archive))
        {
            std::cout << "Archive with that name was not found in storage!\n";
            return;
        }
        std::ifstream in(archive, std::ios::binary);
        if (!in.is_open())
        {
            std::cout << "Could not open the archive inforamtion file!\n";
            return;
        }
        Archive a;
        try
        {
            a.readFromFile(in);
        }
        catch (const std::exception &e)
        {
            if (in.is_open())
                in.close();
            std::cerr << e.what() << '\n';
        }
        catch (...)
        {
            std::cout << "Something went wrong during archive fetch!\n";
            if (in.is_open())
                in.close();
        }
        std::fstream storage(STORAGE_CHAINS, std::ios::in | std::ios::out | std::ios::binary);
        std::fstream bucketList(STORAGE_BUCKETLIST, std::ios::in | std::ios::out | std::ios::binary);
        try
        {
            
           
        }
        catch (const std::exception &e)
        {
            std::cerr << e.what() << '\n';
            storage.close();
            bucketList.close();
        }
        catch (...)
        {
            std::cout << "Something went wrong during check!\n";
            storage.close();
            bucketList.close();
        }
    }

    void UpdateArchive(const bool &hashOnly, const std::string &name, const std::set<fs::path> &dirs)
    {
        if (!checkStorageSetup())
        {
            std::cout << "Run command \'initialize\' to create all necessary files!\n";
            return;
        }
        std::string filename = name;
        filename.append(".dat");
        fs::path archive(ARCHIVES_DATA_PATH);
        archive.append(filename);
        if (!ArchiveExists(archive))
        {
            std::cout << "Archive with that name was not found in storage!\n";
            return;
        }
        std::ifstream in(archive, std::ios::binary);
        if (!in.is_open())
        {
            std::cout << "Could not open the archive inforamtion file!\n";
            return;
        }
        Archive curr;
        try
        {
            curr.readFromFile(in);
            in.close();
        }
        catch (const std::exception &e)
        {
            if (in.is_open())
                in.close();
            std::cerr << e.what() << '\n';
        }
        catch (...)
        {
            std::cout << "Something went wrong during archive fetch!\n";
            if (in.is_open())
                in.close();
        }
        std::fstream storage(STORAGE_CHAINS, std::ios::in | std::ios::out | std::ios::binary);
        std::fstream bucketList(STORAGE_BUCKETLIST, std::ios::in | std::ios::out | std::ios::binary);
        std::vector<fs::path> filteredDirectories;
        removeOverlappingPaths(filteredDirectories, dirs);
        Archive updatedArchive;
        std::ofstream out(archive.string(), std::ios::binary| std::ios::app);
        out.seekp(std::ios::beg);
        if (!out.is_open())
        {
            std::cout << "Cannot update the archive!Please try again later!\n";
            storage.close();
            bucketList.close();
            return;
        }
        try
        {
            bool res = updatedArchive.CreateFromDirectoryList(filteredDirectories,bucketList,storage,hashOnly);
            writeMetadata();
            if(!res)
            {
                std::cout<<"Archive wasn't updated!\n";
                out.close();
                return;
            }   
            updatedArchive.setDateArchived(curr.getDateArchived());
            updatedArchive.writeToFile(out);
            curr.markAsRemoved(bucketList,storage);
            out.close();
            bucketList.close();
            storage.close();
            return;
        }
        catch(...)
        {
            std::cout<<"Something unexpected happened!\n";
            storage.close();
            bucketList.close();
            out.close();
            return;
        }
        std::cout<<"Archive updated successfully!\n";

    }

    void ArchiveInfo(const std::string &name)
    {
        if (!checkStorageSetup())
        {
            std::cout << "Run command \'initialize\' to create all necessary files!\n";
            return;
        }
        std::string filename = name;
        filename.append(".dat");
        fs::path archive(ARCHIVES_DATA_PATH);
        archive.append(filename);
        if (!ArchiveExists(archive))
        {
            std::cout << "Archive with that name was not found in storage!\n";
            return;
        }
        std::ifstream in(archive, std::ios::binary);
        if (!in.is_open())
        {
            std::cout << "Could not open the archive inforamtion file!\n";
            return;
        }
        Archive curr;
        try
        {
            curr.readFromFile(in);
            in.close();
        }
        catch (const std::exception &e)
        {
            if (in.is_open())
                in.close();
            std::cerr << e.what() << '\n';
        }
        catch (...)
        {
            std::cout << "Something went wrong during archive fetch!\n";
            if (in.is_open())
                in.close();
        }
        curr.dfsPrint();
    }

    void InitializeStorage()
    {
        fs::path dataFolder(DATA_PATH);
        if (!fs::exists(dataFolder))
        {
            fs::create_directory(dataFolder);
        }
        fs::path archivesData(ARCHIVES_DATA_PATH);
        if (!fs::exists(archivesData))
        {
            fs::create_directory(archivesData);
        }
        fs::path metadataPath(STORAGE_METADATA);
        writeMetadata();
        initializeBucketList();
        initializeStorageChains();
    }

    void StorageStatistic()
    {
        if (!checkStorageSetup())
        {
            std::cout << "Run command \'initialize\' to create all necessary files!\n";
            return;
        }
        readMetadata();
        std::cout << "Total files stored in storage: " << totalFiles << '\n';
        std::cout << "Total chunks stored in storage: " << bucketListSize << '\n';
        std::cout<<"Last added  chunk id: "<<lastChunkId<<'\n';
        std::cout<<"Bucket list capacity: "<<bucketListCapacity<<'\n';
        std::cout<<"Load factor: "<<(double)bucketListSize/(double)bucketListCapacity<<'\n';
    }

    void DeleteStorage()
    {

        fs::path archivesData(ARCHIVES_DATA_PATH);
        if (fs::exists(archivesData))
        {
            for (const auto &entry : fs::directory_iterator(archivesData))
            {
                if (fs::is_regular_file(entry.path()))
                {
                    fs::remove(entry.path());
                }
            }
        }
        InitializeStorage();
    }

    void removeOverlappingPaths(std::vector<fs::path> &filteredPaths, const std::set<fs::path> &paths)
    {
        if(paths.empty())
            return;
        for (const fs::path &p : paths)
        {
            bool isSubDir = false;
            for (const fs::path &existing : filteredPaths)
            {
                if (p.string().find(existing.string()) == 0)
                {
                    isSubDir = true;
                    break;
                }
            }
            if (!isSubDir)
            {
                filteredPaths.push_back(p);
            }
        }
    }

    void readMetadata()
    {
        std::ifstream in(STORAGE_METADATA, std::ios::binary);
        if (!in.is_open())
        {
            throw std::runtime_error("Cannot read storage meata data!");
        }
        in.read(reinterpret_cast<char *>(&totalFiles), sizeof(uint64_t));
        in.read(reinterpret_cast<char *>(&lastChunkId), sizeof(uint64_t));
        in.read(reinterpret_cast<char *>(&bucketListSize), sizeof(uint64_t));
        in.read(reinterpret_cast<char *>(&bucketListCapacity), sizeof(uint64_t));
        in.close();
    }

    void writeMetadata()
    {
        std::ofstream out(STORAGE_METADATA, std::ios::binary);
        if (!out.is_open())
        {
            throw std::runtime_error("Cannot write storage meta data");
        }
        out.write(reinterpret_cast<const char *>(&totalFiles), sizeof(uint64_t));
        out.write(reinterpret_cast<const char *>(&lastChunkId), sizeof(uint64_t));
        out.write(reinterpret_cast<const char *>(&bucketListSize), sizeof(uint64_t));
        out.write(reinterpret_cast<const char *>(&bucketListCapacity), sizeof(uint64_t));
        out.close();
    }

    bool ArchiveExists(const fs::path &archivePath)
    {
        return fs::exists(archivePath);
    }

    bool checkStorageSetup()
    {
        if (!fs::exists(DATA_PATH))
        {
            std::cout << "data directory is missing!\n";
            return false;
        }
        if (!fs::exists(ARCHIVES_DATA_PATH))
        {
            std::cout << "archives_data directory is missing!\n";
            return false;
        }
        if (!fs::exists(STORAGE_BUCKETLIST))
        {
            std::cout << "data/storage_bucket_list.dat file is missing!\n";
            return false;
        }
        if (!fs::exists(STORAGE_CHAINS))
        {
            std::cout << "data/storage.dat file is missing!\n";
            return false;
        }
        if (!fs::exists(STORAGE_METADATA))
        {
            std::cout << "data/storage_metadata.dat file is missing!\n";
            return false;
        }
        return true;
    }

    void initializeBucketList()
    {
        if(fs::exists(STORAGE_BUCKETLIST))
        {
            fs::remove(STORAGE_BUCKETLIST);
        }
        std::ofstream out(STORAGE_BUCKETLIST, std::ios::binary);
        if (!out.is_open())
            throw std::runtime_error("Coudn't initialize storage bucket list!");
        uint64_t emptyBucketVal = 0;
        for (int i = 0; i < bucketListCapacity; i++)
        {
            out.write(reinterpret_cast<const char *>(&emptyBucketVal), sizeof(uint64_t));
        }
        out.close();
    }
    void initializeStorageChains()
    {
        fs::path storageChainsPath(STORAGE_CHAINS);
        if(fs::exists(storageChainsPath))
        {
            fs::remove(storageChainsPath);
        }
        std::ofstream chainsFile(storageChainsPath, std::ios::binary);
        if (!chainsFile.is_open())
            throw std::runtime_error("Error initializing storage information!");
        uint8_t dummyByte = 0x00;
        for (int i = 0; i < 2; ++i)
        {
            chainsFile.write(reinterpret_cast<const char *>(&dummyByte), sizeof(uint8_t));
        }
        chainsFile.close();
    }
    void printTime(const fs::file_time_type& time_point)//Sources: https://stackoverflow.com/questions/61030383/how-to-convert-stdfilesystemfile-time-type-to-time-t
    {
        auto sctp = std::chrono::time_point_cast<std::chrono::system_clock::duration>(
        time_point - std::filesystem::file_time_type::clock::now() + std::chrono::system_clock::now());
        std::time_t time = std::chrono::system_clock::to_time_t(sctp);
    
        std::cout<<std::put_time(std::localtime(&time), "%d-%m-%Y %H:%M");
    }
    void printAllArchives()
    {
        if (!checkStorageSetup())
        {
            std::cout << "Run command \'initialize\' to create all necessary files!\n";
            return;
        }
        fs::path archivesDir(ARCHIVES_DATA_PATH);
        if(fs::is_empty(archivesDir))
        {
            std::cout<<"No archives found in storage!\n";
            return;
        }
        std::cout<<"Here is a list of archives:\n";
        for (const fs::path& dirEntry :fs::directory_iterator(archivesDir))
        {
            if(fs::is_regular_file(dirEntry))
            {
                std::cout<<dirEntry.stem().string()<<'\n';
            }
        }
        
    }
};