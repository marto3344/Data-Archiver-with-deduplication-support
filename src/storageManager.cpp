#include "storageManager.hpp"
#define ARCHIVES_DATA_PATH "../data/archives_data/"
#include<filesystem>
#include<set>
namespace fs =std::filesystem;

void StorageManager::CreateArchive(const bool &hashOnly, const std::string &name, std::set<fs::path> &dirs)
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
  
    for (auto it = dirs.begin(); it != dirs.end(); ++it)
    {
       const fs::path& currFile = *it;
       std::cout << currFile.string() << std::endl;
    }
    
    if(fs::exists(filePath))
    {
        std::cout<<"Archive created successfully"<<'\n';
        
    }
    //Calculate block properties
    //Store it depending on hash-only?
    out.close();
}