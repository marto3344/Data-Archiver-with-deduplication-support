#define ARCHIVES_DATA_PATH "../data/archives_data/"
#define STORAGE_METADATA "../data/storage_metadata.dat"
#define STORAGE_BUCKETLIST "../data/storage_bucket_list.dat"
#define STORAGE_CHAINS "../data/storage.dat"
#define DATA_PATH "../data"
#include<filesystem>
#include<set>
#include<fstream>
#include<string>
#include<stack>
#include "storageManager.hpp"
#include "archive.hpp"
#include "file.hpp"
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
        removeOverlappingPaths(filteredDirectories, dirs);
        a.CreateFromDirectoryList(filteredDirectories, bucketList, storage, hashOnly);
        storage.close();
        bucketList.close();
        writeMetadata();
        a.writeToFile(out);
        out.close();
        StorageStatistic();
    }

    void ExtraxtArchive(const std::string &name, const fs::path &targetPath, const std::set<fs::path>& relativePaths)
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
        {   std::vector<fs::path>filteredPaths;
            removeOverlappingPaths(filteredPaths,relativePaths);
            a.ExtractArchive(targetPath,filteredPaths, bucketList, storage);
            storage.close();
            bucketList.close();
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
            fs::path simplifiedPath = simplifyPath(archivePath);
           
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
        updatedArchive.CreateFromDirectoryList(filteredDirectories,bucketList,storage,hashOnly);
        std::ofstream out(archive.string(), std::ios::binary);
        if (!out.is_open())
        {
            std::cout << "Cannot update the archive!Please try again later!\n";
            return;
        }
        updatedArchive.writeToFile(out);
        curr.markAsRemoved(bucketList,storage);
        storage.close();
        bucketList.close();
        out.close();

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
        std::cout << "Total files stored in storage: " << totalFiles << '\n';
        std::cout << "Total chunks stored in storage: " << bucketListSize << '\n';
    }

    void DeleteStorage()
    {

        fs::path archivesData(ARCHIVES_DATA_PATH);
        if (fs::exists(archivesData))
            ;
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
        std::ofstream out(STORAGE_BUCKETLIST, std::ios::trunc | std::ios::binary);
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
        std::ofstream chainsFile(storageChainsPath, std::ios::trunc | std::ios::binary);
        if (!chainsFile.is_open())
            throw std::runtime_error("Error initializing storage information!");
        uint8_t dummyByte = 0x00;
        for (int i = 0; i < 2; ++i)
        {
            chainsFile.write(reinterpret_cast<const char *>(&dummyByte), sizeof(uint8_t));
        }
        chainsFile.close();
    }
    std::string topDirPath(const fs::path& p)
    {
        std::string pathString = p.string();
        std::string topDirLabel;
        if(pathString[0] != '/')
            topDirLabel.push_back(pathString[0]);
        for (int i = 1; i < pathString.size(); i++)
        {
            if(pathString[i] == '/')
                break;
            topDirLabel.push_back(pathString[i]);
        }
        return topDirLabel;
    }
    fs::path trimPath(const fs::path& p)
    {
        std::string pathString = p.string();
        int len = 1;
        if(pathString[0]=='/')
            len++;
        for (int i = 1; i < pathString.size(); i++)
        {
            if (pathString[i] == '/')
                break;
            len++;
        }
        return fs::path(pathString.erase(0, len));
    }
    fs::path simplifyPath(const fs::path &p)
    {
       if(p.empty())
            return p;
       std::string path = p.string();
       std::stack<std::string> st;
       std::string dir = "";
       for(size_t i = 0; i<path.size(); ++i)
       {
         if(path[i]!='/')
            dir.push_back(path[i]);
         
         if(path[i]=='/' || i == path.size() -1)
         {
            if(dir != ".." && dir != "." && dir !="")
            {
                dir+='/';
                st.push (dir);
                dir.clear();
            }
            else if ( dir == "..")
            {
                if(!st.empty())
                    st.pop();
                dir.clear();
            }
            else if (dir == ".")
                dir.clear(); 
         }
       } 
       path.clear();
       while(!st.empty())
       {
           path.insert(0, st.top());
           st.pop();
       }
       if(!path.empty() && path[path.size() - 1] == '/')
       {
            path.pop_back();
       }
       path.insert(0,"/");
       return fs::path(path);
    }
};