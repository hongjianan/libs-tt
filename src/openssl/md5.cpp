#include <htest/htest.h>
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <openssl/md5.h>

using namespace std;

DEFINE_TEST(Md5)
{
    CCALL_TEST(Md5_test);
    return 0;
}

DEFINE_TEST(Md5_test)
{
    FILE *fd = fopen("test", "r");
    MD5_CTX c;
    unsigned char md5[17] = { 0 };
    if (fd == NULL) {
        cout << "open failed" << endl;
        return -1;
    }
    int len;
    unsigned char *pData = (unsigned char*) malloc(1024 * 1024);
    if (!pData) {
        cout << "malloc failed" << endl;
        return -1;
    }
    MD5_Init(&c);
    while (0 != (len = fread(pData, 1, 1024 * 1024, fd)))
    {
        MD5_Update(&c, pData, len);
    }
    MD5_Final(md5, &c);
    for (int i = 0; i < 16; i++) {
        printf("%02x", md5[i]);
    }
    printf("\n");
    fclose(fd);
    free(pData);

    return 0;
}

/* end of file */
