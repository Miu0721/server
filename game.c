
#include "game.h"

/*****************************************************************************
* 関数名(function name)：isAlphaNum
*
* 内容　：名前に半角英数字以外が含まれているかチェックする
	(explanation) Checks whether the name contains any characters other 
				  than alphanumeric (half-width letters and digits).

* 引数　：const char *str
*
* 戻り値：false, true
******************************************************************************/
bool isAlphaNum(const char *str) {
    while (*str) {
        if (!isalnum((unsigned char)*str)) {
            return false;
        }
        str++;
    }
    return true;
}

/*****************************************************************************
* 関数名(function name)：getTime
*
* 内容　：ゲームをスタートした日時を獲得する。
*		(explanation) Gets the game start date and time.
* 引数　：char *date, size_t size
*
* 戻り値：なし
******************************************************************************/
int getTime(char *date, size_t size) {
	
    time_t t = time(NULL);													 // 現在の時刻を取得
    strftime(date, size, "%Y-%m-%d %H:%M:%S", localtime(&t));				 // 現在のローカルタイムを取得し、指定されたフォーマットで文字列に変換する
	
	return 0;
}

/*****************************************************************************
* 関数名(function name)：pickUp
*
* 内容　：先攻か後攻をランダムで決める
*	(explanation) Randomly decide turn order (first or second)  	
* 引数　：なし
*
* 戻り値：int num
******************************************************************************/
int pickUp(){

	int num; 
	
	//rand関数の初期化
	srand(time(NULL)); 
	
	//0か１をランダムで格納する
	num = rand()% 2 + 0;  

	return num;
}

/*****************************************************************************
* 関数名(function name)：setBoard
*
* 内容　：基盤の情報を初期化する
*		引き取った構造体配列のexistメンバにカードが存在有の情報を配置
*		numメンバに0~11数字を２つずつと99の文字をランダムで配置する
*	(explanation) Initializes the board: sets exist to show card presence,
	 and randomly assigns two of each number (0–11).

* 引数　：CARD* data
*
* 戻り値：なし
******************************************************************************/
int setBoard(CARD* data){
	
	int i;																
	int temp;
	int numbers[BOARD_SIZE];
	
	//カードをすべて存在有に設定（有＝*、無＝空白)
    for (i = 0; i < BOARD_SIZE; i++) {
        data[i].exist = '*'; 											// existフィールドを '* ' に設定
    }
	
	
	// numbers配列に0~11 の数字をそれぞれ ２個ずつ、12 を 1 個用意
    for (i = 0; i < BOARD_SIZE - 1; i++) {
        numbers[i] = i % 12; 											// 0~11 の数字を繰り返す
    }
	
    numbers[BOARD_SIZE - 1] = 12; 										// 最後の要素に 99 を設定
	
	srand(time(NULL));													// 乱数生成器にシードを設定
	
	//numbers配列の数字をシャッフルする
	for (size_t i = BOARD_SIZE - 1; i > 0; i--) {						//Fisher-Yates シャッフルアルゴリズム
        size_t j = rand() % (i + 1);
        temp = numbers[i];
        numbers[i] = numbers[j];
        numbers[j] = temp;
    }
	
	// シャッフルされた数字を data に格納する
    for (i = 0; i < BOARD_SIZE; i++) {
        data[i].num = numbers[i];
    }
		for (i = 0; i < BOARD_SIZE; i++) {
		if(data[i].num == 12 ){
			data[i].exist = ' ';										//12の数字は存在無にする
		}
    }
	return 0;
}

/*****************************************************************************
* 関数名(function name)：displayBoard
*
* 内容　：ボードの盤面を表示（めくられているか否かの表示）
*	(explanation) Display board state (face-up or face-down)

* 引数　：CARD* data
*
* 戻り値：なし
******************************************************************************/
int displayBoard(const CARD* data){
	
	int i;											//カウンタ用変数
	int j = 2;										//x座標出力時のカウンタ用変数
	
	
	printf("\n");
	printf(  "    1  2  3  4  5\n");					//y座標の数字入力
	printf(  "  +---------------+\n1 |");				
	
    for (i = 0; i < 25; i++) {
        printf("%2c ", data[i].exist); 				// existフィールドの値を出力
        
        if ((i + 1) % 5 == 0) {
            printf("|\n"); 							// 5列ごとに改行を出力
        	if( j <= 5 ){
        		printf("%d |", j);					//x座標の数字出力
        	}
        	j++;
        }
    	
    }
	printf(  "  +---------------+");
	printf("\n");
	
	return 0;
}

/*****************************************************************************
* 関数名(function name)：selectCell
*
* 内容　：xとy座標の入力処理を行い、マスの位置を特定する。
*			選択されたマスのカードが正しいかどうかの判断も行う。
*	(Explanation) Handles x/y input, identifies board position, and checks if 
				  the selected card is valid.
		
* 引数　：CARD* data
*
* 戻り値：int location
******************************************************************************/
int selectCell(const CARD* data){
	
	int x = 0;
	int y = 0;
	char buffer[50];
	int location;
	int valid;
	int i;
	
	while(1){
		
		 // X座標の入力
		 while (1) {
			 printf("X座標の数字を入力してください。(1-5)：");
			 
		 	fgets(buffer, sizeof(buffer), stdin); 			 						// 入力処理
			buffer[strcspn(buffer, "\n")] = '\0';									//改行文字をnullにする
		
			if (buffer[0] == '\0') {												// 改行のみ入力された場合
			    printf("入力に誤りがあります。1～5の数字を入力してください。\n");
			     continue;
			 }
			 
		 	valid = 1;																//数字フラグを立てる
			for( i = 0; i < strlen(buffer); i++){
				if( !isdigit((unsigned char)buffer[i]) ){							//数字かどうか一文字ずつ判別する
					valid =0;														//数字以外が入力された場合フラグを0にする
					break;
				}
			}
			
			if(valid && sscanf(buffer,"%d", &x) == 1 &&  x >= 1 && x <= 5 ){		//数字であり、1~5の数字である場合（sscanfで配列を文字列として格納する)
				break;
			}
			else{
				printf("入力に誤りがあります。\n1～5の数字を入力してください。\n");
			}
		 }
		
		// Y座標の入力
		 while (1) {
			 printf("Y座標の数字を入力してください。(1-5)：");
			 
		 	fgets(buffer, sizeof(buffer), stdin); 			 						// 入力処理
			buffer[strcspn(buffer, "\n")] = '\0';									//改行文字をnullにする
		
			if (buffer[0] == '\0') {												// 改行のみ入力された場合
			    printf("入力に誤りがあります。1～5の数字を入力してください。\n");
			     continue;
			 }
			 
		 	valid = 1;																//数字フラグを立てる
			for( i = 0; i < strlen(buffer); i++){
				if( !isdigit((unsigned char)buffer[i]) ){							//数字かどうか一文字ずつ判別する
					valid = 0;														//数字以外が入力された場合フラグを0にする
					break;
				}
			}
			
			if(valid && sscanf(buffer,"%d", &y) == 1 &&  y >= 1 && y <= 5 ){		//数字であり、1~5の数字である場合（sscanfで配列を文字列として格納する)
				break;
			}
			else{
				printf("入力に誤りがあります。1～5の数字を入力してください。\n");
			}
		 }
		
		location = x + ((y-1) * 5) -1;													//xとyの座標からマスの位置を求め、格納
		
		if( data[location].exist  == ' ' ){											//選択されたカードが存在するか否か判断する(' ' : 存在しない)
			printf( "すでにめくられているカードです。\n");
			printf( "残っているカードを選択してください\n");
			continue;
		}
		else{
			break;
		}
	}
	return location;
}

/*****************************************************************************
* 関数名(function name)：openCell
*
* 内容　：選択されたカードをめくった盤面の表示
*			引数でマスの位置を引き取り、そのカードの数字を表示する
*	(explanation) Shows the board with the selected card flipped, displaying 
				  its number based on the given position.

* 引数　：const CARD* data, int card1, int card2
*
* 戻り値：なし
******************************************************************************/
int openCell(const CARD* data, int card1, int card2){
	
	int i;											//カウンタ用変数
	int j = 2;										//x座標出力時のカウンタ用変数
	
	
	printf("\n");
	printf(  "    1  2  3  4  5\n");				//y座標の数字入力
	printf(  "  +---------------+\n1 |");			
	
    for (i = 0; i < 25; i++) {
    	
    	if( card1 == i ) {							//１枚目カードのマスの位置が一致した場合
			printf("%2d ", data[i].num);			//１枚目カードの数字を出力
    	}
    	else if( card2 == i ) {						//２枚目カードのマスの位置が一致した場合
			printf("%2d ", data[i].num);			//２枚目カードの数字を出力
    	}
    	else {
        	printf("%2c ", data[i].exist); 			// existフィールドの値を出力
    	}
        if ((i + 1) % 5 == 0) {
            printf("|\n"); 							// 5列ごとに改行を出力
        	if( j <= 5 ){
        		printf("%d |", j);					//x座標の数字出力
        	}
        	j++;
        }
    	
    }
	printf(  "  +---------------+");				
	printf("\n");
	
	return 0;
}

/*****************************************************************************
* 関数名(function name)：gradeCell
*
* 内容　：２枚のカードの数字が同一か判別する
*			２枚のカードのマスの位置を引数とする
* (explanation) Checks if the numbers on two cards (given their positions) match.
* 引数　：const CARD* data, int card1, int card2
*
* 戻り値：int 1,int 0
******************************************************************************/
int gradeCell(const CARD* data, int card1, int card2){
	
	if( data[card1].num == data[card2].num){
		return 1;
	}
	
	return 0;
}

/*****************************************************************************
* 関数名(function name)：removeCell
*
* 内容　：盤面から獲得されたカードを削除する
*			'*'(存在有)→' '(存在無)に変更する
*	(Explanation) Removes acquired cards by changing '*' to ' ' on the board.

		
* 引数　：CARD* data, int card1, int card2
*
* 戻り値：なし
******************************************************************************/
int removeCell(CARD* data, int card1, int card2){
	
	data[card1].exist = ' ';
	data[card2].exist = ' ';
	
	return 0;
}

/*****************************************************************************
* 関数名(function name)：makeFile
*
* 内容　：CSVファイルにゲームの結果情報を書き込む																																							
*	(Explanation) Outputs game results to a CSV file.		
* 引数　：const char* play_time, const char* server_name,int server_score, 
*			const char* client_name, int client_score, const char* winner_name
*
* 戻り値：なし
******************************************************************************/
int makeFile(const char* play_time, const char* server_name,int server_score, 
				const char* client_name, int client_score, const char* winner_name){
	
	int record_count = 0;											//データ数
	static GameRecord record[10];									//履歴を格納する
	GameRecord new_record;											//追加する履歴データ
	int i;															//カウンタ
	char header[256];												//ヘッダー行格納変数
	FILE *fp;														//ファイルポインタ
	
	//引数のデータを格納する
	strcpy( new_record.play_time, play_time );
	new_record.play_time[strcspn(new_record.play_time, "\n")] = '\0';		//改行文字をnullにする
	
	strcpy( new_record.server_name, server_name );
	new_record.server_name[strcspn(new_record.server_name, "\n")] = '\0';
	
	new_record.server_score = server_score;
	
	strcpy( new_record.client_name, client_name );
	new_record.client_name[strcspn(client_name, "\n")] = '\0';
	
	new_record.client_score = client_score;
	
	strcpy( new_record.winner_name, winner_name );
	new_record.winner_name[strcspn(winner_name, "\n")] = '\0';
	
	fp = fopen(FILE_NAME, "r");											//ファイルオープン
	if (fp != NULL) {													//すでにファイルと履歴が存在している場合
        
		fgets(header, sizeof(header), fp);								//ヘッダー行スキップ
	
		while(fscanf(fp, "%20[^,],%d,%20[^,],%d,%20[^,],%64[^\n]\n",	//既存データの読み込みとrecord配列へデータを格納する
							record[record_count].server_name,
							&record[record_count].server_score,
							record[record_count].client_name,
							&record[record_count].client_score,
							record[record_count].winner_name,
							record[record_count].play_time) == 6){		//6つの項目を正しく読み取った場合、ループを続行
			record_count++;													//履歴数のインクリメント
			if(record_count >= 10){											//履歴数が10に達した場合breakする
				break;		
			}
		}
		fclose(fp);														//ファイルを閉じる
	}
	
	
	if ( record_count >= 10 ){										//データが10個を超えた場合、一番古いデータを削除する
		
    	for (i = 1; i < record_count; i++) {
        	record[i - 1] = record[i];
    	}
    	record_count--;												//履歴数デクリメント
    }
	
	record[record_count] = new_record;								//新しいデータを履歴配列に格納する
    record_count++;													//インクリメント
	
	fp = fopen(FILE_NAME, "w");										//ファイルオープン
    if (fp == NULL) {												//ファイルが開けない場合の異常処理
        printf("ファイルが開けませんでした。\n");
		return -1;
    }
	
	fprintf(fp, "Server Name,Server Score,Client Name,Client Score,Winner Name,Play Time\n");	//ヘッダー行書込み
	
    for (i = 0; i < record_count; i++) {													// データ行の書き込み
	        fprintf(fp, "%s,%d,%s,%d,%s,%s\n",
	                record[i].server_name,
	                record[i].server_score,
	                record[i].client_name,
	                record[i].client_score,
	                record[i].winner_name,
		        	record[i].play_time);
    }
    
    fclose(fp);													//ファイルクローズ
	
	return 0;
}
