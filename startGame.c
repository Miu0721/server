/*****************************************************************************
* 関数名(function name)：startGame
*
* 内容　：サーバと接続する。どちらかのスコアが14枚、またはお互いのスコアが12枚に
			なるまで、盤面情報と点数を送受信する。その後、結果を表示する。ユーザ
			がゲームの終了を選択しない間、上記の処理を繰り返す。
	
	(explanation) connecting with server. The board information and scores are 
	sent and received until either side's score reaches 14 cards or each other's
	score reaches 12 cards. The results are then displayed. The above process 
	is repeated while the user - does not choose to end the game.
*			
* 引数　：なし
*
* 戻り値：0：正常終了　　-1　:　異常終了
******************************************************************************/


#include <stdio.h> 
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
	
#include "game.h"
#include "net.h"

#define NAME_SIZE 30
#define BOARD_SIZE 25
#define WINNING_SCORE 14
#define DRAW_SCORE 12


int startGame(){

    int sock;           /* connectServerの戻り値を入れる変数*/
	int server_fd;		/*リスニングソケット格納変数*/
    char client_name[NAME_SIZE];		/* ユーザーネームを入れる配列 */
    char server_name[NAME_SIZE];
	int len;            /* ユーザーネームの長さを測るときに使う変数*/
    int client_fd;      /* client側にmessagを送るときに使う変数 */
    int pick_up;        /* pickUp関数の戻り値を入れる変数　*/
    int switch_play; 	/* 先攻後攻を変えるとき使う変数*/
	char buffer[1024];	/* client側からのメッセージを受け取ると用の配列*/
	int result;			/* gradeCellの戻り値を入れる変数*/
	int my_score; 		/*　点数を格納する配列*/
	int card1; 			/* １枚目に選択されたカード*/
	int card2; 			/* 2枚目に選択されたカード*/
	int other_score; 
	int reply_flg = 0; 		/* リプレイフラグの初期化と宣言 */
	char reply[10]; 			/* リプレイ選択の際に使う配列 */ 
	char winner[NAME_SIZE];
	char date[64];			/*プレイ時間を格納する配列*/
	
	CARD data[BOARD_SIZE];
	
	//関数の呼び出し// 戻り値を戻り値をint sockに代入

	sock = connectServer(&server_fd); 


    if(sock < 0){
        printf("接続に失敗しました。");

        return -1;
    }

	while(1){															
		
		printf("ユーザー名を入力してください : ");
		fgets( server_name, 21, stdin);														//ユーザー名入力処理
		server_name[strcspn(server_name, "\n")] = '\0';										// 改行文字をヌル文字に置き換える
		len = strlen(server_name);															//名前の長さを取得
		
		if( len == 0 ){
			printf( "入力に誤りがあります。半角英数字20文字未満で入力してください。\n");	//改行のみの場合while文先頭に戻る
			continue;
		}
		else if( len > 19 ){
			printf( "入力に誤りがあります。半角英数字20文字未満で入力してください。\n");	//20文字以上の場合while文先頭に戻る
			int c;
			while ((c = getchar()) != '\n' && c != EOF);														//クリアバッファ
			continue;
		}
		else
		
		if( ! isAlphaNum( server_name ) ){													//半角英数字の判別(半角英数字以外の文字が含まれている場合falseを返す)
			printf( "入力に誤りがあります。半角英数字20文字未満で入力してください。\n");
			continue;
		}
		
		break;
	}
	

	// ユーザー名をclient側に送信
	send(sock, server_name, NAME_SIZE, 0);

	// client側のユーザー名を受け取る
	recv(sock, client_name, NAME_SIZE, 0); 
	
	//ゲーム説明
	printf("神経衰弱へようこそ！\n");
	printf("ゲームは、1ターンごとに2枚のカードをめくる形で進行します。\n盤面の座標を指定してカードを選びましょう。\n");
	printf("もし、めくった2枚のカードの絵柄が一致すれば、\nそのカードをもう一度めくることができます。\n");
	printf("1ターン目以降は、相手がめくったカードの位置と絵柄が盤面に表示されるので、\nそれを参考にしてカードを選んでも構いません。\n");
	printf("最初に14枚のカードを獲得したプレイヤーが勝者となります!\n\n");

	while(1){
		//関数の呼び出し
		getTime( date, sizeof(date) );
		
		//関数の呼び出し
		setBoard( data );
		
		// 点数の初期化
		my_score = 0;
		other_score = 0; 
		reply_flg = 0; 		/* リプレイフラグの初期化と宣言 */
		
		//盤面を共有
		send(sock, data, sizeof(data), 0);

 

		//関数の呼び出しと代入
		switch_play = pickUp(); 
		
		send(sock, &switch_play, sizeof(switch_play), 0);
		
		if( switch_play == 1 ){

			printf( "先攻は%sさん、後攻は%sさんで始めます。\n", server_name, client_name);

		}else if( switch_play == 0 ){

			printf( "先攻は%sさん、後攻は%sさんで始めます。\n",client_name, server_name );

			printf( "まずは%sさんのターンです。\n",client_name); 

		}

		while(1){

		    if(reply_flg == 1){
		        break;
		    }

			//ゲームの中にいる間、
			while(switch_play == 0 || switch_play == 1){
			
				if(switch_play == 0){
					
					recv(sock, &card1, sizeof(card1), 0);
					recv(sock, &card2, sizeof(card2), 0);
					recv(sock, &switch_play, sizeof(switch_play), 0);
					recv(sock, &other_score, sizeof(other_score), 0);
					
					printf("\n%sさんが引いたカード結果:\n",client_name);
					openCell(data, card1, card2);
					
					if( data[card1].num == data[card2].num ){								//相手の獲得したカードを盤面から削除する
						removeCell(data, card1, card2 );
					}
					
					continue;
				}
				
				printf( "%sさんのスコア：%d\n", client_name, my_score);
				printf( "%sさんのスコア：%d\n\n", server_name, other_score);
				printf("%sさんのターンです。\n", server_name);
									
				displayBoard(data);														//盤面表示
				card1 = selectCell(data);												//1枚目カード選択
				openCell( data,card1, -1 );												//盤面の開示
					
				while(1){
						
					card2 = selectCell(data);											//2枚目カード選択
					openCell( data,card1, card2 );										//盤面の開示
					
					if( card1 == card2){
						printf("同じカードが選択されています。\n異なるカードを入力してください。\n");
						continue;
					}
					break;
				}
				
				//選択された二枚のカードの絵柄があっている確認	//戻り値をresult()に代入
				result = gradeCell( data, card1, card2 );	
				
				// もし、絵柄が当たっていなかったら
				if(result == 0){
					
					printf( "ハズレです... 合計：%d枚\n", my_score);					//結果表示
					printf( "\n%sさんのターンに移ります。\n", server_name);		
					
					switch_play = 0;													//switch_playを相手のターンに変える
					
					//clientのために、switch_playに0を代入する。
					send(sock, &card1, sizeof(card1), 0);	
					send(sock, &card2, sizeof(card2), 0);
					send(sock, &switch_play, sizeof(switch_play), 0);
					send(sock, &my_score, sizeof(my_score), 0);
					
					
				}
			
				//もし、絵柄が当たっていたら
				else{
					
					//得点獲得
					my_score += 2; 
					printf( "2枚GET! 合計：%d枚\n", my_score);							//結果表示
					
					//スコアが勝利条件に達したら、
					if((my_score  ==  WINNING_SCORE) || (my_score ==  DRAW_SCORE && other_score == DRAW_SCORE)){	//決着がつき、ゲームが終了した場合
							
						switch_play = -1;
					
						send(sock, &card1, sizeof(card1), 0);									//データの送信
						send(sock, &card2, sizeof(card2), 0);
						send(sock, &switch_play, sizeof(switch_play), 0);
						send(sock, &my_score, sizeof(my_score), 0);
					
					}
					else{
						printf( "再度、カードを引くことができます。\n");
						
						//関数呼び出し
						removeCell( data, card1, card2 );
						send(sock, &card1, sizeof(card1), 0);								//データの送信
						send(sock, &card2, sizeof(card2), 0);
						send(sock, &switch_play, sizeof(switch_play), 0);
						send(sock, &my_score, sizeof(my_score), 0);
					}
				}
			}
		
						
			//勝者を決定する
			printf("勝敗が決定しました。\n結果は...\n");
			
			if(other_score < my_score){
				printf("%sさんの勝利です！\n", server_name);	
				strcpy( winner, server_name );
			}
			else if(my_score < other_score){
				printf("%sさんの勝利です！\n",client_name);
				strcpy( winner, client_name );
			}
			else if( my_score == other_score ){
				printf("引き分けです！\n");
				strcpy( winner, "DRAW" );
			}
					
			//関数の呼び出し
			makeFile(date, server_name, my_score, client_name, other_score, winner);

			while(1){

				//リプレイ選択
				printf("ゲームを続けますか？[Yes:y/No;n];");
				fgets(reply, sizeof(reply), stdin);	
				size_t len = strlen(reply);
				
				if (len > 0 && reply[len - 1] == '\n') {
					reply[len - 1] = '\0';												// 改行文字をヌル文字に置き換える
				 }										
				
				if( strlen(reply) == 1 && tolower(reply[0]) == 'n'  ){					//リプレイを希望しない場合
					
					switch_play = -1;		
					send(sock, &switch_play, sizeof(switch_play), 0);						//リプレイ情報の送信
					
					printf("ゲームを終了します。\n");
					shutdown(server_fd, SHUT_RDWR);											//リスニングソケットを閉じる
					close(sock);															//接続されたソケットを閉じる
					return 0;
				}

					// 入力値がYかyの時
				else if( strlen(reply) == 1 && tolower(reply[0]) == 'y' ){		//リプレイを希望する場合
					
					switch_play = 1;		
					send(sock, &switch_play, sizeof(switch_play), 0);
					recv(sock, &switch_play, sizeof(switch_play), 0); 
					
					if(switch_play == 1){

					//リプレイ情報の送信
					reply_flg = 1;															//リプレイフラグを立てる
					break;
					}
					
					if(switch_play == -1){
						printf("ゲームを終了します。\n");
						shutdown(server_fd, SHUT_RDWR);											//リスニングソケットを閉じる
						close(sock);															//接続されたソケットを閉じる
						return 0;
					}
						
				}
					// 入力値に誤りがある場合
				else{
					printf( "入力に誤りがあります。yかnを入力してください\n");
					continue;
				}
			
			}
		}
		
	}
return 0; 
} 

