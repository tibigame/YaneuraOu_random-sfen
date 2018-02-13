﻿#ifndef _GRAPHIC_REDUX_H_
#define _GRAPHIC_REDUX_H_
// Reduxの概念を実装します

#include "graphic_common.h"
#include "graphic_string.h"
#include "../../../position.h"
#include <thread>
#include <vector>
#include <queue>

#ifdef GLFW3

extern std::mutex cmd_mtx; // cmdとの通信のためのミューテックス
extern std::mutex store_mtx; // Storeのキューにアクセスするためのミューテックス

// 実行する関数のタイプ
enum class FunctionType
{
	IS_READY, // isready
	EXIT, // 終了
	USER, // USER
	INFO_UPDATE, // infoの更新
	TEST, // テスト関数
	NONE
};

// Actionの定義 (Actionは関数のタイプとstringからなる)
struct Action {
	FunctionType ft;
	std::string str;
	int index;
	Action::Action();
	Action(const FunctionType ft_, const std::string &str);
};

class Button {
private:
	double left, bottom, right, top;
	FunctionType ft; // 実行する関数のタイプ
public:
	bool is_visible = false; // 可視状態か
	bool is_enable = false; // 有効化状態か
	Button();
	~Button();
	void init(double left_, double bottom_, double right_, double top_, FunctionType ft_); // 初期化を行う
	const bool is_range(double posx, double posy) const; // 渡されたposが自身の範囲内か
	const Action get_action(const std::string &str_) const; // アクションを発行する
};

// 状態を表現する構造体
class State {
	public:
	std::vector<Button> buttons;
	Position pos_; // Position構造体を受け取る
	bool is_render_pos = false; // pos_を描写するかのフラグ
	std::string info = u8" "; // 汎用の情報出力用の文字列を格納します

	State();
	State(const State &a); // コピーコンストラクタ
	State &operator=(const State &a);
};

// Stateを管理するクラス
class Store {
private:
	std::queue<Action> action_que; // Actionを格納するキュー
	
	
	GlString* gl_string;
public:
	void update_store(const State &nextState); // stateを更新する
	State state;

	Store();
	~Store();

	void set_glstring(GlString* gl_string); // gl_stringの初期化後にポインタを渡す

	void callback(const double posx, const double posy, const std::string &str);
	void add_action_que(Action ac); // キューにactionを追加する

	State provider(); // 現在のstateから描写に必要な情報を取り出す
	void render(const State &state) const; // providerを呼び出して描写を行う
	
};

// Actionを発行する
const Action action_callback(const double posx, const double posy, const std::string str, const std::vector<Button> buttons);
const Action action_update_info(const std::string new_info);

// Actionを受けて新しいStateを作成する (Stateは直接変更しない)
const State reducer(const Action &action, const State &state);


#endif

#endif _GRAPHIC_REDUX_H_
