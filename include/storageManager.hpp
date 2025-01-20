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
    static void CreateArchive(const bool& hashOnly,const std::string& name,std::set<fs::path>& dirs);
    static void ExtraxtArchive();
    static void CheckArchive();
    static void UpdateArchive();
    static void InitializeStorage();
    private:
    static bool ArchiveExists() {return false;};
    static void initializeBucketList();


};
#endif