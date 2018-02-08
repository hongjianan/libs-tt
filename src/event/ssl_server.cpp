#if 0
#include "htest/htest.h"
#include <assert.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>

#include <netinet/in.h>
#include <sys/socket.h>
#include <fcntl.h>

#include <event2/event.h>
#include <event2/listener.h>
#include <event2/buffer.h>
#include <event2/bufferevent.h>
#include <event2/bufferevent_ssl.h>

#include <openssl/ssl.h>
#include <openssl/err.h>
#include <openssl/rand.h>


#define CA_CERT_FILE "/home/jason/ssl/server/ca.crt"
#define SERVER_CERT_FILE "/home/jason/ssl/server/server.crt"
#define SERVER_KEY_FILE "/home/jason/ssl/server/server.key"


class ServerSSL
{
public:
    uint16_t server_port;

    struct event_base*  base;

    SSL_CTX*    ssl_ctx;
};

ServerSSL* g_server_ssl;

SSL* CreateSSL(evutil_socket_t& fd)
{
    SSL_CTX* ctx = NULL;  
    SSL* ssl = NULL; 
    ctx = SSL_CTX_new (SSLv23_method());
    if( ctx == NULL)
    {
        printf("SSL_CTX_new error!\n");
        return NULL;
    }

    // 要求校验对方证书  
    SSL_CTX_set_verify(ctx, SSL_VERIFY_PEER, NULL);  

    // 加载CA的证书  
    if(!SSL_CTX_load_verify_locations(ctx, CA_CERT_FILE, NULL))
    {
        printf("SSL_CTX_load_verify_locations error!\n");
        return NULL;
    }

    // 加载自己的证书  
    if(SSL_CTX_use_certificate_file(ctx, SERVER_CERT_FILE, SSL_FILETYPE_PEM) <= 0)
    {
        printf("SSL_CTX_use_certificate_file error!\n");
        return NULL;
    }

    // 加载自己的私钥  
    if(SSL_CTX_use_PrivateKey_file(ctx, SERVER_KEY_FILE, SSL_FILETYPE_PEM) <= 0)
    {
        printf("SSL_CTX_use_PrivateKey_file error!\n");
        return NULL;
    }

    // 判定私钥是否正确  
    if(!SSL_CTX_check_private_key(ctx))
    {
        printf("SSL_CTX_check_private_key error!\n");
        return NULL;
    }

    // 将连接付给SSL  
    ssl = SSL_new (ctx);
    if(!ssl)
    {
        printf("SSL_new error!\n");
        return NULL;
    }

    SSL_set_fd (ssl, fd);  
    if(SSL_accept (ssl) != 1)
    {
        int icode = -1;
        int iret = SSL_get_error(ssl, icode);
        printf("SSL_accept error! code = %d, iret = %d\n", icode, iret);
        return NULL;
    }

    return ssl;
}


void socket_read_cb(evutil_socket_t fd, short events, void *arg)
{
    SSL* ssl = (SSL*)arg;
    char msg[4096];
    memset(msg, 0, sizeof(msg));  
    int nLen = SSL_read(ssl,msg, sizeof(msg));  
    fprintf(stderr, "Get Len %d %s ok\n", nLen, msg);  
    strcat(msg, "\n this is from server========server resend to client");  
    SSL_write(ssl, msg, strlen(msg));
}

static void listener_cb(struct evconnlistener *listener, evutil_socket_t fd,
                        struct sockaddr *sa, int socklen, void *user_data)
{
    struct bufferevent *b_out, *b_in;
    /* Create two linked bufferevent objects: one to connect, one for the
     * new connection */
    b_in = bufferevent_socket_new(g_server_ssl->base, fd,
        BEV_OPT_CLOSE_ON_FREE|BEV_OPT_DEFER_CALLBACKS);

    SSL *ssl = SSL_new(g_server_ssl->ssl_ctx);
    b_out = bufferevent_openssl_socket_new(g_server_ssl->base, -1, ssl,
            BUFFEREVENT_SSL_CONNECTING, BEV_OPT_CLOSE_ON_FREE|BEV_OPT_DEFER_CALLBACKS);

}

static int init_ssl()
{
    SSL_library_init();
    ERR_load_crypto_strings();
    SSL_load_error_strings();
    OpenSSL_add_all_algorithms();

    int r = RAND_poll();
    if (r == 0) {
        fprintf(stderr, "RAND_poll() failed.\n");
        return 1;
    }
    g_server_ssl->ssl_ctx = SSL_CTX_new(SSLv23_method());

    return 0;
}

int ServerSSLTest_ssl(int argc, const char* argv[])
{
    if (2 != argc) {
        printf("usage: port\n");
        return -1;
    }

    int server_port = atoi(argv[1]);

    g_server_ssl = new ServerSSL();
    g_server_ssl->server_port = server_port;

    init_ssl();

    g_server_ssl->base = event_base_new();
    if (!g_server_ssl->base) {
        printf("event_base_new error.\n");
        return -1;
    }

    struct sockaddr_in sin;
    sin.sin_family = AF_INET;
    sin.sin_addr.s_addr = 0;
    sin.sin_port = htons(g_server_ssl->server_port);

    struct evconnlistener* listener = evconnlistener_new_bind(g_server_ssl->base, listener_cb,
            (void *)g_server_ssl->base, LEV_OPT_REUSEABLE | LEV_OPT_CLOSE_ON_EXEC | LEV_OPT_CLOSE_ON_FREE,
            -1, (struct sockaddr*)&sin,  sizeof(sin));

    if (!listener) {
        fprintf(stderr, "Could not create a listener!\n");
        return -1;
    }

    event_base_dispatch(g_server_ssl->base);
    evconnlistener_free(listener);
    event_base_free(g_server_ssl->base);
    delete g_server_ssl;

    printf("server stop.\n");

    return 0;
}


DEFINE_TEST(ServerSSLTest)
{
    ServerSSLTest_ssl(argc, argv);

    return 0;
}

#endif
