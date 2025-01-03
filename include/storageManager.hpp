#pragma once

#ifndef _STORAGE_HEADER_INCLUDED_
#define _STORAGE_HEADER_INCLUDED_
#include<iostream>
#include<filesystem>
#include<vector>
#include<fstream>
namespace fs =std::filesystem;
class StorageManager{
    public:
    StorageManager() = delete;
    static void CreateArchive(const bool& hashOnly,const std::string& name,std::vector<fs::path>& dirs);
    static void ExtraxtArchive(){};
    static void CheckArchive(){};
    static void UpdateArchive(){};
    private:
    double getLoadFactor() const {return 0;};
    static void initializeFiles(){};
    static bool ArchiveExists() {return false;};


};
#endif