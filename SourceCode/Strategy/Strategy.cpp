#include <iostream>
#include "Point.h"
#include "Strategy.h"
#include "Judge.h"
#include <cfloat>
#include <cstdlib>
#include <vector>
#include <ctime>
#include <cassert>
#include <cmath>
using namespace std;

#define ME 2
#define OPPO 1
#define PLAYER_SUM 3
#define EMPTY 0

#define TIME_LIMIT 2.0
// #define DEBUG_2

// #ifdef DEBUG
//     #define DEBUG_PRINT(x) printf x
// #else
//     #define DEBUG_PRINT(x) do {} while (0)
// #endif

// #ifdef DEBUG_2
//     #define DEBUG_PRINT(x) std::cout << x << std::endl
// #else
//     #define DEBUG_PRINT(x) do {} while (0)
// #endif

const int c = 1.42;
int** Board;
const int* Top;
int board_M;
int board_N;
int no_X;
int no_Y;
int last_X;
int last_Y;
const int max_MCT_steps = 990000;
int r_count=0;//rollout数目

int scale[4]={6,4,3,2};// 效果放大倍率
class Node {
public:
	int p; 
	int ch[12]; 
	int ch_num; 
	int next_extend[12];
	int next_extend_num;
	int x;
	int y;
	int player;
	long long visit;
	long long win;
	vector<int>weight;
	int total_weight;
	int** board;
	int top[12];
	Node() {
		p = ch_num = visit = win = 0;
		next_extend_num = board_N;
		x = y = -1;
		player = EMPTY;
		for (int i = 0; i < 12; i++) ch[i] = 0;
		for (int i = 0; i < board_N; i++) {
			next_extend[i] = i;
		}
	}
	~Node() {
		if (board != nullptr) {
			clearArray(board_M, board_N, board);
		}
	}
};

void copy_board(int **&board, int** src) {
	board = new int *[board_M];
	for (int i = 0; i < board_M; i++) {
		board[i] = new int[board_N];
	}
	for (int i = 0; i < board_M; i++) {
		for (int j = 0; j < board_N; j++) {
			board[i][j] = src[i][j];
		}
	}
}
void copy_top(int* top, const int* src) {
	for (int i = 0; i < board_N; i++) {
		top[i] = src[i];
	}
}

bool gameover(int** board, int* top, int x, int y, int player) {
	int** board_ptrs = new int*[board_M];
	copy_board(board_ptrs, board);
	if (player == ME) {
		bool win = machineWin(x, y, board_M, board_N, board_ptrs);
		bool tie = isTie(board_N, top);
		clearArray(board_M, board_N, board_ptrs);
		return (win || tie);
	}
	else if (player == OPPO) {
		bool win = userWin(x, y, board_M, board_N, board_ptrs);
		bool tie = isTie(board_N, top);
		clearArray(board_M, board_N, board_ptrs);
		return (win || tie);
	}
	clearArray(board_M, board_N, board_ptrs);
	return false;
}
bool is_win(int** board, int* top, int x, int y, int player) {
	int** board_ptrs;
	copy_board(board_ptrs, board);
	bool ans = false;
	if (player == ME)
		ans =  machineWin(x, y, board_M, board_N, board_ptrs);
	else if (player == OPPO)
		ans =  userWin(x, y, board_M, board_N, board_ptrs);
	clearArray(board_M, board_N, board_ptrs);
	return ans;
}
bool is_essential(int **board, int x, int y, int player) {
	int** board_ptrs;
	copy_board(board_ptrs, board);
	board_ptrs[x][y] = player;
	bool me_win = machineWin(x, y, board_M, board_N, board_ptrs);
	bool oppo_win = userWin(x, y, board_M, board_N, board_ptrs);
	clearArray(board_M, board_N, board_ptrs);
	if(player==ME) return me_win;
	else return oppo_win;
}
int check(int** board) {
	for (int i = 0; i < board_N; i++) {
		int tmp_x = Top[i] - 1;
		int tmp_y = i;
		
		if (tmp_x >= 0 && (is_essential(board, tmp_x, tmp_y, ME)||is_essential(board, tmp_x, tmp_y, OPPO))){
			cerr<<"c"<<endl;
			cerr<<"tmp x:"<<tmp_x<<" tmp y:"<<tmp_y<<endl;
			cerr<<"me_win:"<<is_essential(board, tmp_x, tmp_y, ME)<<" oppo win:"<<is_essential(board, tmp_x, tmp_y, OPPO)<<endl;
			return tmp_y;
		}
			
	}
	return -1;
}
int check(int** board, int* top,int player) {
	for (int i = 0; i < board_N; i++) {
		int tmp_x = top[i] - 1;
		int tmp_y = i;
		if (tmp_x >= 0 && (is_essential(board, tmp_x, tmp_y, player)));
			return tmp_y;
	}
	
	return -1;
}

class MCTTree {
public:
	int capacity;
	Node* nodes;
	int size;
	int root;
	MCTTree(int capacity_) : capacity(capacity_), nodes(new Node[capacity_]), size(1), root(0) {
		copy_board(nodes[0].board, Board);
		copy_top(nodes[0].top, Top);
		nodes[0].player = OPPO;
		nodes[0].x = last_X;
		nodes[0].y = last_Y;
		nodes[0].total_weight = 0;
		int j = 0;
		for(int i = 0; i < board_N; i++){
			if(nodes[0].top[i] > 0){
				nodes[0].next_extend[j++] = nodes[0].next_extend[i];
			}
		}
		nodes[0].next_extend_num = j;
		nodes[0].weight.clear();
		for (int i = 0; i < board_N; i++) {
			nodes[0].weight.push_back(board_N * board_N -(board_N / 2 - i) * (board_N / 2 - i));
			nodes[0].total_weight += nodes[0].weight[i];
		}
	}
	~MCTTree() {
		// DEBUG_PRINT("delete");
		if (nodes) {
			// DEBUG_PRINT("release capacity" << " " << capacity << " " << nodes);
			delete[] nodes;
			nodes = nullptr;
			// DEBUG_PRINT("release capacity" << " " << capacity);
		}
	}
	bool gameover(int rank) {
		int** board_ptrs;
		copy_board(board_ptrs, nodes[rank].board);
		if (nodes[rank].player == ME) {
			bool win = machineWin(nodes[rank].x, nodes[rank].y, board_M, board_N, board_ptrs);
			bool tie = isTie(board_N, nodes[rank].top);
			clearArray(board_M, board_N, board_ptrs);
			return (win || tie);
		}
		else if (nodes[rank].player == OPPO) {
			bool win = userWin(nodes[rank].x, nodes[rank].y, board_M, board_N, board_ptrs);
			bool tie = isTie(board_N, nodes[rank].top);
			clearArray(board_M, board_N, board_ptrs);
			return (win || tie);
		}
		clearArray(board_M, board_N, board_ptrs);
		return false;
	}
    bool is_win(int rank) {// nodes[rank] win
        int** board_ptrs;
        copy_board(board_ptrs, nodes[rank].board);
        bool ans = false;
        if (nodes[rank].player == ME)
            ans =  machineWin(nodes[rank].x, nodes[rank].y, board_M, board_N, board_ptrs);
        else if (nodes[rank].player == OPPO)
            ans =  userWin(nodes[rank].x, nodes[rank].y, board_M, board_N, board_ptrs);
        clearArray(board_M, board_N, board_ptrs);
        return ans;
    }

	int add_child(int rank, int x, int y) {
		int ch_rank = nodes[rank].ch_num;
		nodes[rank].ch[ch_rank] = size;
		Node& ch_node = nodes[size];
		ch_node.p = rank;
		ch_node.x = x;
		ch_node.y = y;
		ch_node.player = PLAYER_SUM - nodes[rank].player;
		copy_board(ch_node.board, nodes[rank].board);
		copy_top(ch_node.top, nodes[rank].top);
		ch_node.board[x][y] = ch_node.player;
		ch_node.top[y]--;
		if (x - 1 == no_X && y == no_Y) ch_node.top[y]--;
		int j = 0;
		for(int i = 0; i < board_N; i++){
			if(ch_node.top[i] > 0){
				ch_node.next_extend[j++] = ch_node.next_extend[i];
			}
		}
		ch_node.next_extend_num = j;
		ch_node.weight.clear();
		ch_node.total_weight = 0;
		for (int i = 0; i < board_N; i++) {
			ch_node.weight.push_back(board_N * board_N - (board_N / 2 - i) * (board_N / 2 - i) );
			ch_node.total_weight += ch_node.weight[i];
		}
		size++;
		nodes[rank].ch_num++;
		return size - 1;
	}
	double ucb(int rank) {
		long long p_visit = nodes[nodes[rank].p].visit;
		long long this_visit = nodes[rank].visit;
		long long this_win = nodes[rank].win;
		double win_rate = ((double)this_win) / this_visit;
		double ans = win_rate + c * sqrt(2 * log((double)p_visit) / this_visit);
		return ans;
	}
	int select() {
		int r = root;
		while (nodes[r].next_extend_num == 0) {
			double max_score = -DBL_MAX;
			int ch_rank = 0;
			for (int i = 0; i < nodes[r].ch_num; i++) {
				int rank = nodes[r].ch[i];
				double ucb_score = ucb(rank);
				if (ucb_score > max_score) {
					max_score = ucb_score;
					ch_rank = rank;
				}
			}
			r = ch_rank;
		}
		return r;
	}
	int expand(int rank) {
		for(int i=0; i<nodes[rank].next_extend_num;i++){
			int y_=nodes[rank].next_extend[i];
			if(nodes[rank].top[y_]<=0){
				nodes[rank].next_extend_num--;
				swap(nodes[rank].next_extend[i], nodes[rank].next_extend[nodes[rank].next_extend_num]);
			}
		}
		int index = rand() % nodes[rank].next_extend_num;
		int y = nodes[rank].next_extend[index];
		nodes[rank].next_extend_num--;
		swap(nodes[rank].next_extend[index], nodes[rank].next_extend[nodes[rank].next_extend_num]);
		// }
		int x = nodes[rank].top[y] - 1;
		int r = add_child(rank, x, y);
		return r;
	}
	void backward(int rank, bool is_win) {
		int r = rank;
		int cnt = 0;
		if(r_count>3){
			while (r != 0) {
				if (cnt % 2 == 0) nodes[r].win += is_win;
				else nodes[r].win += !is_win;
				nodes[r].visit++;
				cnt++;
				r = nodes[r].p;
			}
			nodes[root].visit++;
		}
		else{
			while (r != 0) {
				if (cnt % 2 == 0) nodes[r].win += is_win*scale[r_count];
				else nodes[r].win += !is_win*scale[r_count];
				nodes[r].visit+=scale[r_count];
				cnt++;
				r = nodes[r].p;
			}
			nodes[root].visit+=scale[r_count];
		}
	}
	bool rollout(int rank) {
		r_count=0;
		int **tmp_board;
		int tmp_top[12];
		copy_board(tmp_board, nodes[rank].board);
		copy_top(tmp_top, nodes[rank].top);
		int x = nodes[rank].x;
		int y = nodes[rank].y;
		int step_x = x;
		int step_y = y;
		int step_player = nodes[rank].player;	
		vector<int> now_weight = nodes[rank].weight;
		auto tot_weight = nodes[rank].total_weight;
		for (int i = 0; i < board_N; i++) {
			if (tmp_top[i] <= 0) {
				tot_weight -= now_weight[i];
			}
		}
		while(!::gameover(tmp_board, tmp_top, step_x, step_y, step_player)){
			r_count++;
			step_player = PLAYER_SUM - step_player;
			bool is_necessary = false;
			for (int i = 0; i < board_N; i++) {
				int tmp_x = tmp_top[i] - 1;
				int tmp_y = i;
				if (tmp_x >= 0 && (is_essential(tmp_board, tmp_x, tmp_y, ME)||is_essential(tmp_board, tmp_x, tmp_y, OPPO)))
				{
					step_y = tmp_y;
					step_x=tmp_x;
					tmp_top[step_y]--;
					if (step_x - 1 == no_X && step_y == no_Y) tmp_top[step_y]--;
					if (tmp_top[step_y] <= 0) {
						tot_weight -= now_weight[step_y];
					}
					tmp_board[step_x][step_y] = step_player;
					is_necessary = true;
					break;
				}
			}
			if (is_necessary){
				continue;
			}
			int w=rand() % tot_weight;
			int index = 0;
			while (index < board_N){
				while (tmp_top[index] <= 0){
					index++;
					if (index == board_N){// DEBUG_PRINT("test");
						exit(-1);
					}
					continue;
				}
				if(w < now_weight[index]){
					break;
				}
				w -= now_weight[index];
				index++;
			}
			if (index == board_N){
				// // DEBUG_PRINT("test");
				// for (int i = 0; i < board_N; i++) {
				// 	// DEBUG_PRINT("now weight: %d top: %d\n", now_weight[i], tmp_top[i]);
				// }
				// // DEBUG_PRINT("tot_weight: %d\n, w: %d\n", tot_weight, w);
				exit(-1);
			}
			step_y = index;
			step_x = tmp_top[step_y] - 1;
			tmp_top[step_y]--;
			if (step_x - 1 == no_X && step_y == no_Y) tmp_top[step_y]--;
			if (tmp_top[step_y] <= 0){
				tot_weight -= now_weight[step_y];
			}
			tmp_board[step_x][step_y] = step_player;
		}
		bool last_win = ::is_win(tmp_board, tmp_top, step_x, step_y, step_player);
		int last_player = step_player;
		clearArray(board_M,board_N,tmp_board);
		if (nodes[rank].player == last_player && last_win) return true;
		else return false;
	}
	int check(int rank) {
		for (int i = 0; i < board_N; i++) {
			int tmp_x = nodes[rank].top[i] - 1;
			int tmp_y = i;
			if (tmp_x >= 0 && (is_essential(nodes[rank].board, tmp_x, tmp_y, ME)||is_essential(nodes[rank].board, tmp_x, tmp_y, OPPO)))
				return tmp_y;
		}
		return -1;
	}
	int select_2(int rank){
		double max_win_rate = -DBL_MAX;
		int ans_rank = rank;
		for (int i = 0; i < nodes[rank].ch_num; i++) {
			int choice_rank = nodes[rank].ch[i];
			double win_rate = ((double)nodes[choice_rank].win) / (nodes[choice_rank].visit);
			if (win_rate > max_win_rate) {
				max_win_rate = win_rate;
				ans_rank = choice_rank;
			}
		}
		return ans_rank;
	}
};

extern "C" Point* getPoint(const int M, const int N, const int* top, const int* _board, 
	const int lastX, const int lastY, const int noX, const int noY){
	int x = -1, y = -1;
	int** board = new int*[M];
	for(int i = 0; i < M; i++){
		board[i] = new int[N];
		for(int j = 0; j < N; j++){
			board[i][j] = _board[i * N + j];
		}
	}
	srand(time(0));
	double t0 = clock();
	board_M = M, board_N = N, Board = board, Top = top, no_X = noX, no_Y = noY, last_X = lastX, last_Y = lastY;
	y=check(Board);
	if(y!=-1){
		cerr<<"essential: "<<"x="<<Top[y] - 1<<" y="<<y<<endl;
	}
	if(last_X==-1&&last_Y==-1){
		y=rand()%board_N;
		while(Top[y]<=0) y=rand()%board_N;
	}
	
	if(y==-1){
		MCTTree tree(1000000);
		for (int i = 0; i < max_MCT_steps; i++){
			int r1 = tree.select();
            if(tree.gameover(r1)){
                bool if_win=tree.is_win(r1);
                tree.backward(r1,if_win);
            }
            else{
                int r2 = tree.expand(r1);
                bool if_win = tree.rollout(r2);
                tree.backward(r2, if_win);
            }			
			if (clock()-t0 > TIME_LIMIT * CLOCKS_PER_SEC) {
				break;
			}
			
		}
		int ans_rank=tree.select_2(tree.root);
		x = tree.nodes[ans_rank].x;
		y = tree.nodes[ans_rank].y;	
	}
	else{
		x = Top[y] - 1;
	}
	// cerr<<"x="<<x<<" y="<<y<<endl;
	// cerr<<"last"<<last_X<<" "<<last_Y<<endl;
	return new Point(x, y);
}

extern "C" void clearPoint(Point* p){
	delete p;
	return;
}

void clearArray(int M, int N, int** board){
	for(int i = 0; i < M; i++){
		delete[] board[i];
	}
	delete[] board;
}
