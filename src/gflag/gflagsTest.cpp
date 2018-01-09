/*
 * main.cpp
 *
 *  Created on: 2014-10-31
 *      Author: hong
 */
#include "config.h"
#ifdef LIB_GFLAGS_TEST

#include <iostream>
#include <gflags/gflags.h>

DEFINE_string(name, "default name", "you must set name!");
DEFINE_int32(age, 0, "you must set age!");

int GflagsTest(int argc, char* argv[])
{
	/* arrayArg */
    gflags::SetUsageMessage("Person set usage");
    gflags::SetVersionString("V1.0.0");
    gflags::ParseCommandLineFlags(&argc, &argv, true);

    int age = FLAGS_age;
    std::cout << "name: " << FLAGS_name << std::endl;
    std::cout << "age: "  << FLAGS_age << ", " << age << std::endl;

	return 0;
}

#endif /* LIB_GFLAGS_TEST */
/* end file */
