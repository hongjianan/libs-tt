#include <stdio.h>
#include <string.h>
#include "c/Message.pb-c.h"



int main(int argc, char *argv[])
{
	//GOOGLE_PROTOBUF_VERIFY_VERSION;
	
	Message__Header header = MESSAGE__HEADER__INIT;
	header.length = 10;
	header.uri_type = 20;
	header.ret_code = 30;
	
	uint8 str[128];
	size_t size = message__header__pack(&header, &str);
	printf("pack size:%d get_size:%d\n", size, message__header__get_packed_size(&header));
	
	header.SerializeToString(&str);
	cout << "pack:" << "===" << str << "===" << endl;
	
	Message__Header unpack;
	unpack.ParseFromString(str);
	cout << unpack.length() << " " << unpack.uri_type() << " " << unpack.ret_code() << endl;
	
	message__header__unpack();
	
	
	return 0;
}
