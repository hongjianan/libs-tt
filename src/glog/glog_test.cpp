
#include "htest/htest.h"

#include <glog/logging.h>

int GlogTest_glog(int argc, char* argv[])
{
    LOG(INFO) << "run GlogTest_glog";

    return 0;
}

DEFINE_TEST(GlogTest)
{
    return GlogTest_glog(argc, argv);
}
