/********************************************************
*	Strategy.h : ���Խӿ��ļ�                           *
*	������                                              *
*	zhangyf07@gmail.com                                 *
*	2010.8                                              *
*********************************************************/

#ifndef STRATEGY_H_
#define	STRATEGY_H_

#include "Point.h"

extern "C" Point* getPoint(const int M, const int N, const int* top, const int* _board, 
	const int lastX, const int lastY, const int noX, const int noY);

extern "C" void clearPoint(Point* p);

void clearArray(int M, int N, int** board);

void copy_board(int **&board, int** src);
void copy_top(int* top, const int* src);
bool gameover(int** board, int* top, int x, int y, int player);
bool is_win(int** board, int* top, int x, int y, int player) ;
bool is_essential(int **board, int x, int y, int player);
int check(int** board);
int check(int** board,int* top,int player);
/*
	�������Լ��ĸ�������
*/


#endif