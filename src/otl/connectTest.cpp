/*
 * connectTest.cpp
 *
 *  
 *
 *  Created on: 2016-5-7
 *      Author: Hong
 */
#include <iostream>
#include <boost/format.hpp>

/* otl */
#define OTL_ODBC
#define	OTL_ODBC_SELECT_STM_EXECUTE_BEFORE_DESCRIBE
#define OTL_ODBC_UNIX
#include <otl/otlv4.h>

using namespace std;

#define OTL_LOGIN_1		"UID=root;PWD=0591;DSN=people"
#define OTL_LOGIN_2		"root/0591@people"

/*================ main ==============*/
int ConnectTest_Test(int argc, char *argv[]);
int ConnectTest(int argc, char *argv[])
{
	int ret;

	ret = ConnectTest_Test(argc, argv);

	return ret;
}

int ConnectTest_Test(int argc, char *argv[])
{
	otl_connect	db;
	otl_connect::otl_initialize();

	try {
		db.rlogon(OTL_LOGIN_1);
		cout << "connect mysql success" << endl;
	} catch (otl_exception &exp) {
		cerr << exp.msg << endl;
	}

	db.logoff();

	return 0;
}

/* end of file */
