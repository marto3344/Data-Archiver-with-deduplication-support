#pragma once

#ifndef _STORAGE_HEADER_INCLUDED_
#define _STORAGE_HEADER_INCLUDED_
#include<iostream>
#include<filesystem>
#include<vector>
#include<fstream>
#include<set>
namespace fs =std::filesystem;
class StorageManager{
    public:
    StorageManager() = delete;
    static void CreateArchive(const bool& hashOnly,const std::string& name,const std::set<fs::path>& dirs);
    static void ExtraxtArchive(const std::string& name, const fs::path& targetPath, const fs::path& archivePath);
    static void CheckArchive();
    static void UpdateArchive();
    static void InitializeStorage();
    static void StorageStatistic();
    static void DeleteStorage();
    static uint64_t lastChunkId;
    static uint64_t totalFiles;
    static uint64_t bucketListSize;
    static uint64_t bucketListCapacity;
    static void removeOverlappingPaths(std::vector<fs::path>&filteredPaths, const std::set<fs::path>&paths);
    private:
    static void readMetadata();
    static void writeMetadata();
    static bool ArchiveExists(const fs::path& filepath);
    static bool checkStorageSetup();
    static void initializeBucketList();
    static void initializeStorageChains();
    


};
#endif