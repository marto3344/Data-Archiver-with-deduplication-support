#pragma once

#ifndef _FILE_HEADER_INCLUDED_
#define _FILE_HEADER_INCLUDED_

#include<iostream>
#include<vector>
#include<fstream>
#include<cstddef>
#include<string>
#include <chrono>
using time_point = std::chrono::system_clock::time_point;
class File{

    private:
    time_point last_modified;
    uint32_t id;
    uint32_t size;
    //uint64_t chunk_list_head;//Pointer to a linked list
    std::string name;
    std::vector<uint64_t>chunks;

};

#endif