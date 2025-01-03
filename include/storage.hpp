#pragma once

#ifndef _STORAGE_HEADER_INCLUDED_
#define _STORAGE_HEADER_INCLUDED_
#include<iostream>
#include<filesystem>
#include<vector>
#include<fstream>
namespace fs =std::filesystem;
class Storage{
    public:
    Storage() = delete;
    static void CreateArchive(const bool& hashOnly,const std::string& name,std::vector<fs::path>& dirs) {std::cout<<"Hello";};
    void ExtraxtArchive(){};
    void CheckArchive(){};
    void UpdateArchive(){};
    private:
    double getLoadFactor() const {return 0;};
    void initializeFiles() const {};
    bool ArchiveExists() {return false;};


};
#endif