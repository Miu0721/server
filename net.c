
#include "net.h"

/*****************************************************************************
* 関数名(function name)：connectServer
*
* 内容　：ソケットを作成し、クライアント側に接続要求する。その後、接続する。																																							
*   (Explanation) Creates a socket and connects to the server from the client side.
*			
* 引数　：int *listen_fd
*
* 戻り値：int new_socket, int -1
******************************************************************************/
int connectServer(int *listen_fd)
{
    int server_fd, new_socket, valread;					//ファイルディスクリプト、listen後の新しいソケット、readの戻り値（エラー判別）
														//ファイルディスクリプト:ファイルに対するデータの通り道を識別するための目印
    struct sockaddr_in address;							//ソケット通信におけるIPv4アドレスとポート番号を指定するための構造体
    int opt = 1;										//ソケットオプションを設定するため
    int addrlen = sizeof(address);						//アドレス構造体のサイズ
    char buffer[1024] = {0};							//通信データを一時的に格納するためのバッファ。{0}で初期化し、内容をクリアにする

    // ソケットファイルディスクリプタを作成する
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0)					//int socket(int domain, int type, int protocol);
    {
        printf("socket 失敗");
        return -1;
    }

    // ポート52000にソケットをアタッチ	
    if (setsockopt(server_fd, SOL_SOCKET,			//int setsockopt(int sockfd、int level、int optname、const void *optval、socklen_t optlen);
                   SO_REUSEADDR | SO_REUSEPORT, &opt,
                   sizeof(opt)))
    {
        printf("setsockopt 失敗");
        return -1;
    }
	 
	//サーバーのIPアドレスとポートの情報を設定
	address.sin_family = AF_INET;						//プロトコルファミリー
    address.sin_addr.s_addr = INADDR_ANY;				//ソケットアドレスの情報
    address.sin_port = htons(PORT);

    // ポート52000にソケットをバインド（ソケット設定)
    if (bind(server_fd, (struct sockaddr *)&address,		//int bind(int sockfd, const struct sockaddr *addr, socklen_t addrlen);
             sizeof(address)) < 0)							//(struct sockaddr *)この型にキャストしている
    {
        printf("bind 失敗");
        return -1;
    }

    printf("クライアントを待っています。\n");
    if (listen(server_fd, 3) < 0)							//int listen(int sockfd, int backlog); backlog(接続要求を保持する数)
    {
        printf("listen 異常");
        return -1;
    }
	
	*listen_fd = server_fd;									//参照渡しで引数の変数に格納する
	
    if ((new_socket = accept(server_fd, (struct sockaddr *)&address,//int accept(int sockfd, struct sockaddr *addr, socklen_t *addrlen);
                             (socklen_t *)&addrlen)) < 0)
    {
        printf("accept 失敗");
        return -1;
    }

   
    return new_socket;
}

/*****************************************************************************
* 関数名(function name)：connectClient
*
* 内容　：ソケットを作成し、サーバ側に接続要求する。その後、接続する。																																							
*   (explanation) Creates a socket and connects to the server.
*			
* 引数　：無し
*
* 戻り値：int client_fd, int -1
******************************************************************************/

int connectClient()
{
    int status, valread, client_fd;
    struct sockaddr_in serv_addr;
    char buffer[1024] = {0};

    if ((client_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        printf("\n ソケット作成失敗 \n");
        return -1;
    }

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);

    // IPv4/IPv6のアドレスをテキストからバイナリに変換
    if (inet_pton(AF_INET, MY_SERVER_IP_ADDRESS, &serv_addr.sin_addr) <= 0)
    {
        printf(
            "\n　無効またはサポートされてないアドレスです。 \n");
        return -1;
    }

    if ((status = connect(client_fd, (struct sockaddr *)&serv_addr,//(int sockfd, const struct sockaddr *addr, socklen_t addrlen);
                          sizeof(serv_addr))) < 0)
    {
        printf("\n接続失敗\n");
        return -1;
    }

    printf("サーバーに接続しました。\n");
    
    return client_fd;
}

