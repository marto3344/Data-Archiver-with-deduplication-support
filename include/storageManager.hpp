#pragma once

#ifndef _STORAGE_HEADER_INCLUDED_
#define _STORAGE_HEADER_INCLUDED_
#include<iostream>
#include<filesystem>
#include<vector>
#include<fstream>
#include<set>
namespace fs =std::filesystem;
namespace StorageManager{
    void CreateArchive(const bool& hashOnly,const std::string& name,const std::set<fs::path>& dirs);
    void ExtraxtArchive(const std::string& name, const fs::path& targetPath, const fs::path& archivePath);
    void CheckArchive();
    void UpdateArchive();
    void InitializeStorage();
    void StorageStatistic();
    void DeleteStorage();
    extern uint64_t lastChunkId;
    extern uint64_t totalFiles;
    extern uint64_t bucketListSize;
    extern uint64_t bucketListCapacity;
    void removeOverlappingPaths(std::vector<fs::path>&filteredPaths, const std::set<fs::path>&paths);
    void readMetadata();
    void writeMetadata();
    bool ArchiveExists(const fs::path& filepath);
    bool checkStorageSetup();
    void initializeBucketList();
    void initializeStorageChains();

};
#endif