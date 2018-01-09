#include <iostream>
#include <string>
#include "cpp/Message.pb.h"

using namespace std;

int main(int argc, char *argv[])
{
	//GOOGLE_PROTOBUF_VERIFY_VERSION;
	
	message::Header header;
	header.set_length(10);
	header.set_uri_type(20);
	header.set_ret_code(30);
	
	string str;
	header.SerializeToString(&str);
	cout << "pack:" << "===" << str << "===" << endl;
	
	message::Header unpack;
	unpack.ParseFromString(str);
	cout << unpack.length() << " " << unpack.uri_type() << " " << unpack.ret_code() << endl;
	
	
	return 0;
}
