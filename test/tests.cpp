
#define DOCTEST_CONFIG_IMPLEMENT
#include "doctest.h"

#include "archive.hpp"

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