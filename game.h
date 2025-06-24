#include <stdio.h>
#include <stdbool.h>
#include <ctype.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>

#ifndef GAME_H
#define GAME_H

#define BOARD_SIZE  25
#define BUFFER_SIZE 256
#define FILE_NAME "Result.csv"

// データを格納する構造体
typedef struct {
	char exist; 
    int num;	
} CARD;


typedef struct {
    char play_time[BUFFER_SIZE];
    char server_name[BUFFER_SIZE];
    int server_score;
    char client_name[BUFFER_SIZE];
    int client_score;
    char winner_name[BUFFER_SIZE];
} GameRecord;

//プロトタイプ宣言
bool isAlphaNum(const char *str);
int getTime(char *date, size_t size);
int pickUp();
int setBoard(CARD* data);															//配列データを変更する
int displayBoard(const CARD* data);													//配列データ読み取り専用
int selectCell(const CARD* data);													//配列データ読み取り専用										
int openCell(const CARD* data, int card1, int card2);								//配列データ読み取り専用
int gradeCell(const CARD* data, int card1, int card2);								//配列データ読み取り専用
int removeCell(CARD* data, int card1, int card2);									//配列データを変更する
int makeFile(const char* play_time, const char* server_name,int server_score, 
				const char* client_name, int client_score, const char* winner_name);

#endif