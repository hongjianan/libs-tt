/*
 * define.h
 *
 *  
 *
 *  Created on: 2017年9月26日
 *      Author: Hong
 */

#ifndef HCOMMON_LIB_DEFINE_H_
#define HCOMMON_LIB_DEFINE_H_

#include <iostream>

// declare test function
#define CCALL_TEST_ARG(func, argc, argv) \
    int func(int argc, const char *argv[]); \
    func(argc, argv);

#define CCALL_TEST(func)    CCALL_TEST_ARG(func, argc, argv)
    

// define test function
#define DEFINE_TEST(func) \
    int func(int argc, const char *argv[])


// seperator
#define _SEP_TEST(test) \
    std::cout << "========== " << test << " ==========" << std::endl;

#define SEP_TEST(test) \
    std::cout << "========== " << test << " ==========" << std::endl << std::endl;


#define BEGIN_TEST______(test) \
    std::cout << "========== begin " << test << " ==========" << std::endl; {

#define ________END_TEST(test) \
    } std::cout << "========== end " << test << " ==========" << std::endl << std::endl;

#endif /* HCOMMON_LIB_DEFINE_H_ */
/* end of file */
