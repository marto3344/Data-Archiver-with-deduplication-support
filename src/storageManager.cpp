#include "storageManager.hpp"
#define ARCHIVES_DATA_PATH "../data/archives_data/"
#include<filesystem>
namespace fs =std::filesystem;

void StorageManager::CreateArchive(const bool &hashOnly, const std::string &name, std::vector<fs::path> &dirs)
{
    fs::path archivesData (ARCHIVES_DATA_PATH);
    if(!fs::exists(archivesData))
    {
        fs::create_directory(archivesData);
    }

    fs::path filePath (ARCHIVES_DATA_PATH);
    std::string filename = name;
    filename.append(".dat");
    filePath.append(filename);
    if(fs::exists(filePath))
    {
        std::cout<<"Archive with that name already exists! Please use another!"<<'\n';
        return;
    }
    std::ofstream out (filePath.string(),std::ios::binary | std::ios::app);
    if(fs::exists(filePath))
    {
        std::cout<<"Archive created successfully"<<'\n';
        return;
    }
    //Calculate block properties
    //Store it depending on hash-only?
    std::cout<<name;
}