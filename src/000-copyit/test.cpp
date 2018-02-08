#include <htest/htest.h>
#include <iostream>
#include <string>

using namespace std;

DEFINE_TEST(Test)
{
    CCALL_TEST(Test_test);
    return 0;
}


DEFINE_TEST(Test_test)
{
    BEGIN_TEST______("1");

    ________END_TEST("1");

    return 0;
}


/* end of file */
