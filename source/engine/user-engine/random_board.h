﻿#ifndef _RANDOMBOARD_H_
#define _RANDOMBOARD_H_

#include <array>
#include "../../position.h"
#include "int_board.h"

// 盤面の位置を確率的に選択するためのクラス
class PBoard
{
private:
	IntBoard board;
	IntBoard accum;
	int p_sum;
public:
	PBoard(const IntBoard init_board);

	// IntBoardとのビット演算を行う。累計和の更新は行われないので、accumulate()を呼ぶこと。
	void and(IntBoard& int_board); // IntBoardが立っていない部分を0にする。
	void ninp(IntBoard& int_board); // IntBoardが立っている部分を0にする。

	// 累積和の計算と確率的選択
	void accumu(); // 累積加算のBoardを計算する
	int rand(); // 2分探索で累積加算のBoardから選択する
	int accumu_rand(); // accumulateとrandを同時に実行する

	// 盤面を出力する。デバッグ用。
	friend std::ostream& operator<<(std::ostream& os, const PBoard& pos);
};

class PieceExistenceP {
private:
public:
	PieceExistenceP();
};

// 終盤のランダム局面を生成する
void end_game_mate(Position& pos_);

// テスト用の関数
void sq_test(); // sq列をBitBoardに変換してIntBoardに戻すテスト

#endif _RANDOMBOARD_H_