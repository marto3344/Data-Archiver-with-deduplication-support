
#define DOCTEST_CONFIG_IMPLEMENT
#include "doctest.h"
#include<vector>
#include<set>
#include<string>
#include "archive.hpp"
#include "storageManager.hpp"
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
        
        std::vector<fs::path> filteredPaths;
        StorageManager::removeOverlappingPaths(filteredPaths,paths);
        CHECK(filteredPaths.size() == 3);
        CHECK(filteredPaths[0].string() == "D:/A");
        CHECK(filteredPaths[1].string() == "D:/B" );
        CHECK(filteredPaths[2].string() == "D:/C");
    }
}
TEST_SUITE ("ARCHIVE CLASS TEST")
{
    TEST_CASE("Default Constructor")
    {
        Archive f;
        CHECK(f.empty() == true);
    };

};

int main(){
    doctest::Context().run();
}