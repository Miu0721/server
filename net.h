#include <netinet/in.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifndef NET_H
#define NET_H

//マクロ定義
#define PORT (52000)
#define MY_SERVER_IP_ADDRESS ("127.0.0.1")

//プロトタイプ宣言
int connectServer(int *listen_fd);
int connectClient();

#endif