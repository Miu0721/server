#include <stdio.h>
#include <string.h>
#include "net.h"
#include "game.h"

//マクロ定義
#define NAME_SIZE  30
#define BOARD_SIZE  25
#define WINNING_SCORE  14
#define DRAW_SCORE  12


/*****************************************************************************
* 関数名(function name)：main
*
* 内容　：サーバと接続する。
*		どちらかのスコアが14枚、またはお互いのスコアが12枚になるまで、
*		盤面情報と点数を送受信する。その後、結果を表示する。
*		ユーザがゲームの終了を選択しない間、上記の処理を繰り返す。
*		Connects to the server.　Continues sending and receiving board 
		information and scores until one player reaches 14 cards or both 
		players reach 12 cards.　After that, the result is displayed.　This 
		process repeats as long as the user does not choose to exit the game.

* 引数　：なし
*
* 戻り値：int 0, int -1
*
*作成日:2024/08/13
*
*作成者:田村美香
******************************************************************************/
int main(void)
{
	int sock;																//ソケット変数
	char client_name[NAME_SIZE];											//クライアントのユーザー名
	char server_name[NAME_SIZE];											//サーバーのユーザー名
	int len;																//ユーザー名の文字数格納
	int my_score, other_score;												//自分のスコアと、相手のスコア
	int switch_play;														//ゲームの状況を切替る用の変数
	int card1, card2;														//1枚目と２枚目の選択したカード位置の格納する変数
	int result;																//選択された２枚のカードが合致しているかの結果
	char replay[3];															//リプレイ情報の格納
	int replay_flg;															//リプレイのフラグ
	int fin;																//相手のリプレイ情報を格納する変数
	CARD data[BOARD_SIZE];													//盤面用構造体の宣言
	
	
	
	sock = connectClient();													//サーバーとの接続処理
	
	if( sock < 0 ){															//接続の有無
		printf( "接続に失敗しました\n" );
		return -1;
	}
	
	recv(sock, server_name, NAME_SIZE, 0);									//相手のユーザー名を受信
	
	//ユーザー名の入力
	while(1){															
		
		printf("ユーザー名を入力してください : ");
		fgets( client_name, 21, stdin);									//ユーザー名入力処理
		client_name[strcspn(client_name, "\n")] = '\0';							// 改行文字をヌル文字に置き換える
		len = strlen(client_name);												//名前の長さを取得
		
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
		
		if( ! isAlphaNum( client_name ) ){													//半角英数字の判別(半角英数字以外の文字が含まれている場合falseを返す)
			printf( "入力に誤りがあります。半角英数字20文字未満で入力してください。\n");
			continue;
		}

		if( strcmp( server_name, client_name ) == 0){
			printf( "相手と名前が同一です。異なる名前を入力しください。\n");				//自分と相手のユーザー名が同一の場合while文先頭に戻る
			continue;
		}
		
		break;
	}
	
	send(sock, client_name, NAME_SIZE, 0);								//自分のユーザー名を送信
	
	//ゲーム説明の出力
	printf("神経衰弱へようこそ！\n");
	printf("ゲームは、1ターンごとに2枚のカードをめくる形で進行します。\n盤面の座標を指定してカードを選びましょう。\n");
	printf("もし、めくった2枚のカードの絵柄が一致すれば、\nそのカードをもう一度めくることができます。\n");
	printf("1ターン目以降は、相手がめくったカードの位置と絵柄が盤面に表示されるので、\nそれを参考にしてカードを選んでも構いません。\n");
	printf("最初に14枚のカードを獲得したプレイヤーが勝者となります!\n\n");
		
	
	//ゲーム開始
	while(1){															
		
		my_score = 0;													//双方の点数初期化
		other_score = 0;
		replay_flg = 0;													//リプレイフラグの初期化
		
		recv(sock, data, sizeof(data), 0);								//初期化された盤面の受信
		recv(sock, &switch_play, sizeof(switch_play), 0);					//先攻or後攻の受信
		
		if( switch_play == 0 ){
			printf( "先攻は%sさん、後攻は%sさんで始めます。\n",client_name, server_name);	//先攻の場合の出力
		}
		else if( switch_play == 1 ){
			printf( "先攻は%sさん、後攻は%sさんで始めます。\n",server_name, client_name );	//後攻の場合の出力
			printf( "まずは%sさんのターンです。\n\n",server_name); 
		}
		
		while(1){
			
			if( replay_flg == 1 ){															//リプレイ時にbreakする
				break;
			}
			
			while( switch_play == 0 || switch_play == 1 ){
				
				if( switch_play == 1 ){														//後攻または、相手のターンが終わった時の処理
					
					recv(sock, &card1, sizeof(card1), 0);									//データの受取
					recv(sock, &card2, sizeof(card2), 0);
					recv(sock, &switch_play, sizeof(switch_play), 0);
					recv(sock, &other_score, sizeof(other_score), 0);
					
					printf("\n%sさんが引いたカード結果:\n",server_name);
					openCell( data,card1, card2 );											//相手がめくったカードの盤面情報出力
					
					if( data[card1].num == data[card2].num ){								//相手の獲得したカードを盤面から削除する
						removeCell( data, card1, card2 );
					}
					
					continue;
				}
									
				printf( "%sさんのスコア：%d\n", client_name, my_score);					//お互いのスコアの表示
				printf( "%sさんのスコア：%d\n\n", server_name, other_score);
				printf("%sさんのターンです。\n", client_name);
				
				displayBoard( data );													//盤面表示
				
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
				
				result = gradeCell( data, card1, card2 );								//めくった2枚のカードが同じ数字が判断する
				
				if( result == 0 ){														//2枚のカードの数字が異なっていた場合
					
					switch_play = 1;													//switch_playを相手のターンに変える
					
					printf( "ハズレです... 合計：%d枚\n", my_score);					//結果表示
					printf( "\n%sさんのターンに移ります。\n", server_name);		
					send(sock, &card1, sizeof(card1), 0);								//データの送信
					send(sock, &card2, sizeof(card2), 0);
					send(sock, &switch_play, sizeof(switch_play), 0);
					send(sock, &my_score, sizeof(my_score), 0);
				}
				
				else{
					
					my_score += 2;														//獲得カードの追加
					printf( "2枚GET! 合計：%d枚\n", my_score);							//結果表示
					removeCell( data, card1, card2 );									//カードを盤面から削除する
					
					if((my_score  ==  WINNING_SCORE) || (my_score ==  DRAW_SCORE && other_score == DRAW_SCORE)){	//決着がつき、ゲームが終了した場合
						
						switch_play = -1;														//ゲームを終了する
						
						send(sock, &card1, sizeof(card1), 0);									//データの送信
						send(sock, &card2, sizeof(card2), 0);
						send(sock, &switch_play, sizeof(switch_play), 0);
						send(sock, &my_score, sizeof(my_score), 0);
					}
					else{
						printf( "再度、カードを引くことができます。\n");
						
						
						send(sock, &card1, sizeof(card1), 0);								//データの送信
						send(sock, &card2, sizeof(card2), 0);
						send(sock, &switch_play, sizeof(switch_play), 0);
						send(sock, &my_score, sizeof(my_score), 0);
					}
				}
			}
			
			//結果表示
			printf("勝敗が決定しました。\n結果は...\n");
			
			if(other_score < my_score){
				printf("%sさんの勝利です！\n",client_name);									
			}
			else if(my_score < other_score){
				printf("%sさんの勝利です！\n",server_name);
			}
			else if( my_score == other_score ){
				printf("引き分けです！\n");
			}
			
			recv(sock, &fin, sizeof(fin), 0);											//相手のリプレイ情報の受取
			if( fin == -1 ){															//相手がリプレイを希望しない場合
				printf("ゲームを終了します。\n");	
				close(sock);															//接続されたソケットを閉じる
				return 0;
			}
			
			while(1){
		
				printf("ゲームを続けますか？（ y / n ）:");
				fgets(replay, sizeof(replay), stdin);										//リプレイするかどうかの入力
				
				replay[strcspn(replay, "\n")] = '\0';										// 改行文字をヌル文字に置き換える
				
				if( strcmp(replay,"n") == 0 || strcmp(replay,"N") == 0 ){					//リプレイを希望しない場合
					
					switch_play = -1;		
					send(sock, &switch_play, sizeof(switch_play), 0);						//リプレイ情報の送信
					
					printf("ゲームを終了します。\n");
					close(sock);															//接続されたソケットを閉じる
					return 0;
				}
				else if( strcmp( replay, "y" ) == 0 || strcmp( replay, "Y" ) == 0 ){		//リプレイを希望する場合
					
					switch_play = 1;		
					send(sock, &switch_play, sizeof(switch_play), 0);						//リプレイ情報の送信
					replay_flg = 1;															//リプレイフラグを立てる
					break;
				}
				else{
					printf( "入力に誤りがあります。yかnを入力してください\n");
					while ( getchar()!= '\n');												//クリアバッファ
				}
			}
		}
	}
	return 0;
	
}