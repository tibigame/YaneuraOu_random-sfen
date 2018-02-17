﻿#include "graphic_primitive.h"
#include "graphic_object.h"
#include "../../../position.h"
#include "../ex_board.h"
#include <stdlib.h>

// graphic_primitive.cppやgraphic_string.cppを利用して複雑な構造物を描写する関数群です

#ifdef GLFW3
// 将棋盤を描写します
void draw_shogiboard(const GLuint &textureID) {
	// 将棋盤のワールド座標は １一の位置が(-1, -1)と(0, 0)で囲まれる正方形でそこから左下に伸びている。
	// ９九の位置は(-9, -9)と(-8, -8)で囲まれる正方形である。
	// こうすることでa, bの符号の座標が-a, -bから+1ずつの領域となる。 (Shogi Notebookからのアイデア)

	// 最下層のBorder
	GLfloat Border = 0.05f;
	draw_rect(board_border - shogiboard_size - Border, board_border - shogiboard_size - Border, board_border + Border, board_border + Border,
		conv_GL_color(10, 10, 2), conv_GL_color(118, 72, 23, 96), conv_GL_color(7, 7, 7), -0.002f
	);
	// ベースとなる最下層のポリゴン
	draw_rect(board_border - shogiboard_size, board_border - shogiboard_size, board_border, board_border,
		conv_GL_color(80, 80, 48), conv_GL_color(254, 214, 91, 255), conv_GL_color(38, 38, 38), -0.001f
	);
	// 将棋盤テクスチャのポリゴン
	draw_rect(
		board_border - shogiboard_size, board_border - shogiboard_size, board_border, board_border,
		conv_GL_color(80, 80, 48, 32), conv_GL_color(254, 214, 91, 138), conv_GL_color(38, 38, 38, 18),
		0.f, 0.f, 0.0, shogiboard_line_heap * 0.5, true, textureID
	);
	// 線の描写
	for (auto i = 0; i < 10; ++i) {
		draw_line(0.f, 0.f - i, -9.f, 0.f - i, 3.f,
			conv_GL_color(0, 0, 0), conv_GL_color(12, 20, 6, 220), conv_GL_color(24, 20, 9), shogiboard_line_heap);
	}
	for (auto j = 0; j < 10; ++j) {
		draw_line(0.f - j, 0.f, 0.f - j, -9.f, 3.f,
			conv_GL_color(0, 0, 0), conv_GL_color(12, 20, 6, 220), conv_GL_color(24, 20, 9), shogiboard_line_heap);
	}
	// 星の描写
	draw_circle(-3.f, -3.f, 0.1f, conv_GL_color(0, 0, 0), conv_GL_color(12, 20, 6, 220), conv_GL_color(24, 20, 9), shogiboard_line_heap * 2);
	draw_circle(-3.f, -6.f, 0.1f, conv_GL_color(0, 0, 0), conv_GL_color(12, 20, 6, 220), conv_GL_color(24, 20, 9), shogiboard_line_heap * 2);
	draw_circle(-6.f, -3.f, 0.1f, conv_GL_color(0, 0, 0), conv_GL_color(12, 20, 6, 220), conv_GL_color(24, 20, 9), shogiboard_line_heap * 2);
	draw_circle(-6.f, -6.f, 0.1f, conv_GL_color(0, 0, 0), conv_GL_color(12, 20, 6, 220), conv_GL_color(24, 20, 9), shogiboard_line_heap * 2);
}

// 段、筋数字の描写
void draw_shogiboard_rank_file_number(GlString* gl_string) {
	for (auto i = 1; i <= 9; ++i) {
		int_string(i, NumberType::Chinese, 0.4f, 0.05f, -i + 0.3f, 0.f, gl_string);
	}
	for (auto i = 1; i <= 9; ++i) {
		int_string(i, NumberType::Alabic, 0.4f, 0.3f - i, 0.f, 0.f, gl_string);
	}
}

// 数字を描写します
void int_string(const int num, NumberType num_type,
	const GLfloat f_size, GLfloat display_x, GLfloat display_y, GLfloat offset, GlString* gl_string) {
	const GLfloat f_size2 = num_type == NumberType::HalfAlabic ? f_size * 0.5f : f_size;
	GLuint textureID;
	if (num < 0) { // 負の数ならハイフンを出力しておく
		if (gl_string->get_texture_id(u8"-", textureID)) {
			draw_rect_ex(0.f, 0.f, f_size2, f_size,
				GL_COLOR_ZERO, GL_COLOR_MAX, GL_COLOR_ZERO, 0.5f,
				display_x - 5.5f * f_size2, display_y, 0.0, f_size, offset, true, textureID);
		}
		int_string(-num, num_type, f_size, display_x, display_y, offset, gl_string);
		return;
	}
	if (num >= 10) {
		div_t d = div(num, 10);
		if (gl_string->get_texture_id(d.rem, num_type, textureID)) {
			draw_rect_ex(0.f, 0.f, f_size2, f_size,
				GL_COLOR_ZERO, GL_COLOR_MAX, GL_COLOR_ZERO, 0.5f,
				display_x, display_y, 0.0, f_size, offset, true, textureID);
		}
		int_string(d.quot, num_type, f_size, display_x - f_size2, display_y, offset, gl_string);
		return;
	}
	if (gl_string->get_texture_id(num, num_type, textureID)) {
		draw_rect_ex(0.f, 0.f, f_size2, f_size,
			GL_COLOR_ZERO, GL_COLOR_MAX, GL_COLOR_ZERO, 0.5f,
			display_x, display_y, 0.0, f_size, offset, true, textureID);
	}
}

// 盤面の駒文字1つを描写します
void board_piece_string(const Piece pc, const Square sq, GlString* gl_string, const Color c) {
	int file = file_index_table[sq] + 1;
	int rank = rank_index_table[sq] + 1;
	GLfloat display_x = -file + 0.5f;
	GLfloat display_y = c == BLACK ? -rank + 0.47f : -rank + 0.53f; // 升目の中央ではなくやや自陣よりの線に置く補正

	double degree = color_of(pc) == BLACK ? 0.0 : 180.0; // 後手なら180度回転させる
	GLuint textureID;
	if (gl_string->get_texture_id(pc, textureID)) {
		draw_rect_ex(-0.5f, -0.5f, 0.5f, 0.5f,
			GL_COLOR_ZERO, GL_COLOR_MAX, GL_COLOR_ZERO, 0.5f,
			display_x, display_y, degree, 1.1f, 0.12f, true, textureID);
	}
}

// 盤面全ての駒を描写します
void draw_board(const Position &pos_, GlString* gl_string) {
	Piece pc;
	for (auto i = 0; i < 81; ++i) {
		pc = pos_.board[i];
		if (pc != NO_PIECE) {
			board_piece_string(pc, Square(i), gl_string, color_of(pc));
		}
	}
}

// 手駒領域に先手、後手の記号を描写します
void hand_piece_mark(const Color c, const GLfloat f_size = 0.6f, const GLfloat f_size_offset = 0.f) {
	double degree = c == BLACK ? 0.0 : 180.0; // 後手なら180度回転させる
	GLfloat display_x;
	GLfloat display_y;
	if (c == BLACK) {
		display_x = board_border + hand_inner_margin - 0.13f;
		display_y = -0.229f;
		draw_pentagon_ex(0.f, 0.f, f_size, f_size,
			GL_COLOR_ZERO, GL_COLOR_ZERO, GL_COLOR_ZERO, 0.50f,
			display_x, display_y, 0.0, 1.f, 0.12f);
	}
	else {
		display_x = board_border - shogiboard_size - hand_inner_margin + 0.13f;
		display_y = -shogiboard_size + 0.229f + f_size + 0.5f;
		GLfloat boarder = 0.05f;
		draw_pentagon_ex(0.f, 0.f, f_size, f_size,
			GL_COLOR_ZERO, GL_COLOR_ZERO, GL_COLOR_ZERO, 0.499f,
			display_x, display_y, 180.0, 1.f, 0.12f);
		draw_pentagon_ex(0.f + boarder, 0.f + boarder, f_size - boarder, f_size - boarder,
			GL_COLOR_ZERO, GL_COLOR_MAX, GL_COLOR_ZERO, 0.5f,
			display_x, display_y, 180.0, 1.f, 0.12f);
	}

}

// 手駒領域に文字1つを描写します
void hand_piece_string(const Piece pc, const int num, const Color c, GlString* gl_string,
	const GLfloat f_size = 0.7f, const GLfloat f_size_offset = 0.f) {
	double degree = c == BLACK ? 0.0 : 180.0; // 後手なら180度回転させる
	GLfloat display_x;
	GLfloat display_y;
	if (c == BLACK) {
		display_x = board_border + hand_inner_margin + f_size_offset;
		display_y = -0.9f - num * (f_size + 0.01f);
	}
	else {
		display_x = board_border - shogiboard_size - hand_inner_margin + 0.f - f_size_offset;
		display_y = -board_size + 0.9f + num * (f_size + 0.01f) - f_size + 0.5f;
	}
	GLuint textureID;
	if (gl_string->get_texture_id(pc, textureID)) {
		draw_rect_ex(0.f, -f_size, f_size, 0.f,
			GL_COLOR_ZERO, GL_COLOR_MAX, GL_COLOR_ZERO, 0.5f,
			display_x, display_y, degree, 1.f, 0.f, true, textureID);
	}
}
// 漢数字の表示
void hand_piece_int(const int pc_num, const int num, const Color c, GlString* gl_string,
	const GLfloat f_size=0.7f, const GLfloat f_size_offset=0.f) {
	double degree = c == BLACK ? 0.0 : 180.0; // 後手なら180度回転させる
	GLfloat display_x;
	GLfloat display_y;
	if (c == BLACK) {
		display_x = board_border + hand_inner_margin + f_size_offset;
		display_y = -0.9f - num * (f_size + 0.01f);
	}
	else {
		display_x = board_border - shogiboard_size - hand_inner_margin + 0.f - f_size_offset;
		display_y = -board_size + 0.9f + num * (f_size + 0.01f) - f_size + 0.5f;
	}
	GLuint textureID;
	if (gl_string->get_texture_id(pc_num, NumberType::Chinese, textureID)) {
		draw_rect_ex(0.f, -f_size, f_size, 0.f,
			GL_COLOR_ZERO, GL_COLOR_MAX, GL_COLOR_ZERO, 0.5f,
			display_x, display_y, degree, 1.f, 0.f, true, textureID);
	}
}

inline int hand_cnt_p(const Hand hand, const Piece pc) {
	int a = hand_count(hand, pc);
	if (a >= 11) {
		return 3;
	}
	if (a >= 2) {
		return 2;
	}
	if (a == 1) {
		return 1;
	}
	return 0;
}

// 手駒の駒文字列の長さを返します
inline int prescan_hand_str_length(const Hand hand) {
	int length_count = 0;
	length_count += hand_cnt_p(hand, ROOK);
	length_count += hand_cnt_p(hand, BISHOP);
	length_count += hand_cnt_p(hand, GOLD);
	length_count += hand_cnt_p(hand, SILVER);
	length_count += hand_cnt_p(hand, KNIGHT);
	length_count += hand_cnt_p(hand, LANCE);
	length_count += hand_cnt_p(hand, PAWN);
	return length_count;
}

void draw_hand_piece(const Hand hand, const Piece pc, int &num, const Color c, GlString* gl_string,
	const GLfloat f_size=0.7f, const GLfloat f_size_offset=0.f) {
	int a = hand_count(hand, raw_type_of(pc));
	if (a == 0) { return; }
	hand_piece_string(raw_type_of(pc), num, c, gl_string, f_size, f_size_offset);
	++num;
	if (a == 1) { return; }
	if (a >= 10) {
		hand_piece_int(10, num, c, gl_string, f_size, f_size_offset);
		++num;
		a -= 10;
		if (a == 0) { return; }
	}
	hand_piece_int(a, num, c, gl_string, f_size, f_size_offset);
	++num;
}

// 手駒の駒文字を描写します
void draw_hand(const Position &pos_, GlString* gl_string) {
	Hand hand_b = pos_.hand[BLACK];
	Hand hand_w = pos_.hand[WHITE];

	// 手駒デバッグ用
	/*
	hand_b = (Hand)0;
	add_hand(hand_b, ROOK, 2);
	add_hand(hand_b, BISHOP, 2);
	add_hand(hand_b, GOLD, 1);
	add_hand(hand_b, SILVER, 2);
	add_hand(hand_b, KNIGHT, 4);
	add_hand(hand_b, LANCE, 4);
	add_hand(hand_b, PAWN, 12);

	hand_w = (Hand)0;
	add_hand(hand_w, ROOK, 2);
	add_hand(hand_w, BISHOP, 2);
	add_hand(hand_w, GOLD, 4);
	add_hand(hand_w, SILVER, 2);
	add_hand(hand_w, KNIGHT, 3);
	add_hand(hand_w, LANCE, 4);
	add_hand(hand_w, PAWN, 18);
	
	*/
	
	int str_length = prescan_hand_str_length(hand_b); // まず文字列長をチェックするためにスキャンする
	GLfloat f_size;
	GLfloat f_size_offset;
	if (str_length <= 9) {
		f_size = 0.8f;
		f_size_offset = -0.025f;
	}
	else if (str_length <= 13) {
		f_size = 0.7f;
		f_size_offset = 0.025f;
	}
	else {
		f_size = 0.5f;
		f_size_offset = 0.1125f;
	}
	int count = 0;
	draw_hand_piece(hand_b, B_ROOK, count, BLACK, gl_string, f_size, f_size_offset);
	draw_hand_piece(hand_b, B_BISHOP, count, BLACK, gl_string, f_size, f_size_offset);
	draw_hand_piece(hand_b, B_GOLD, count, BLACK, gl_string, f_size, f_size_offset);
	draw_hand_piece(hand_b, B_SILVER, count, BLACK, gl_string, f_size, f_size_offset);
	draw_hand_piece(hand_b, B_KNIGHT, count, BLACK, gl_string, f_size, f_size_offset);
	draw_hand_piece(hand_b, B_LANCE, count, BLACK, gl_string, f_size, f_size_offset);
	draw_hand_piece(hand_b, B_PAWN, count, BLACK, gl_string, f_size, f_size_offset);

	str_length = prescan_hand_str_length(hand_w); // まず文字列長をチェックするためにスキャンする
	f_size = str_length <= 12 ? 0.7f : 0.5f;
	f_size_offset;
	if (str_length <= 9) {
		f_size = 0.8f;
		f_size_offset = -0.025f;
	}
	else if (str_length <= 13) {
		f_size = 0.7f;
		f_size_offset = 0.025f;
	}
	else {
		f_size = 0.5f;
		f_size_offset = 0.1125f;
	}
	count = 0;
	draw_hand_piece(hand_w, W_ROOK, count, WHITE, gl_string, f_size, f_size_offset);
	draw_hand_piece(hand_w, W_BISHOP, count, WHITE, gl_string, f_size, f_size_offset);
	draw_hand_piece(hand_w, W_GOLD, count, WHITE, gl_string, f_size, f_size_offset);
	draw_hand_piece(hand_w, W_SILVER, count, WHITE, gl_string, f_size, f_size_offset);
	draw_hand_piece(hand_w, W_KNIGHT, count, WHITE, gl_string, f_size, f_size_offset);
	draw_hand_piece(hand_w, W_LANCE, count, WHITE, gl_string, f_size, f_size_offset);
	draw_hand_piece(hand_w, W_PAWN, count, WHITE, gl_string, f_size, f_size_offset);

	hand_piece_mark(BLACK);
	hand_piece_mark(WHITE);
}

// 手番と手数を描写します
void draw_teban(const Position &pos_, GlString* gl_string) {
	constexpr GLfloat f_size = 0.7f;
	const GLfloat display_x = -board_size + f_size;
	const GLfloat display_y = -board_size - board_border - 0.2f;
	
	std::string teban = pos_.sideToMove == WHITE ? u8"手番: 後手" : u8"手番: 先手";
	char gamePly[100];
	_itoa_s(pos_.gamePly, gamePly, 10);
	std::string tesuu = u8", 手数: ";
	tesuu += +gamePly;
	draw_string(teban + tesuu, gl_string, 1.f, 9.f, 1, GL_COLOR_ZERO, GL_COLOR_MAX, GL_COLOR_ZERO,
		0.5f, display_x, display_y, 0.0, f_size);
}

void draw_string(std::string str, GlString* gl_string,
	GLfloat line_height, GLfloat MAX_LINE_WIDTH, GLint MAX_LINE,
	GLfloat *mat_ambient, GLfloat *mat_diffuse, GLfloat *mat_specular, GLfloat z = 0.f,
	GLfloat display_x = 0.f, GLfloat display_y = 0.f, double degree = 0.0, GLfloat font_size = 1.f) {

	const GLfloat height_margin = font_size * (line_height - 1.f) / 2.f; // 文字の上下に付くマージン
	GLuint textureID;
	GLint line = 0; // いまレンダリングされている文字の行数
	GLfloat pos_x = 0.f, pos_y = height_margin; // いまレンダリングされている文字の位置
	GLfloat size_x, size_y; // いまレンダリングされている文字のサイズ
	GLfloat font_half_width = font_size * 0.5f;
	
	std::tuple<std::string, int> t;
	auto str_size = str.size();

	int pos = 0;
	std::string buf;
	int buf_size = 0;

	for (int pos = 0; pos < str_size; pos += buf_size) { // u8文字列を1文字ずつスキャンしていく
		t = utf8_next_char(str, str_size, pos);
		buf = std::get<0>(t);
		buf_size = std::get<1>(t);
		if (buf[0] == u8'\n' && buf_size== 1) { // 改行文字のとき
			goto GotoNewLine; // レンダリングは不要なので改行処理のif文内部に直接ジャンプする
		}

		if (buf_size == 1) {
			size_x = font_half_width * 0.8f;
		} else {
			size_x = font_size * 0.8f;
		}
		size_y = font_size;

		// 1文字ずつレンダリングする
		textureID = gl_string->create_and_get_texture_id(buf.c_str());
		if (textureID) {
			draw_rect_ex(0.f, -size_y, size_x, 0.f,
				GL_COLOR_ZERO, GL_COLOR_MAX, GL_COLOR_ZERO, 0.49f,
				display_x + pos_x, display_y + pos_y, 0.0, font_size, 0.f, true, textureID);
		}
		pos_x += size_x * 0.8f; // 次の文字のためにレンダリング位置をずらす

		// 改行の処理
		if (pos_x + font_size > MAX_LINE_WIDTH) { // 最大文字幅を超えそうなら強制改行する
		GotoNewLine:
			pos_x = 0;
			pos_y -= line_height;
			++line;
			if (line >= MAX_LINE) { // 最大行数を越えたら抜ける
				return;
			}
		}
	}
}

// info情報を出力します
void draw_info(const std::string &info_, GlString* gl_string) {
	const GLfloat display_x = -board_size - board_border - hand_inner_margin - hand_size - hand_outer_margin + 0.2f;
	const GLfloat display_y = -board_size - board_border - 1.2f;
	draw_string(info_, gl_string, 1.f, 13.4f, 6,
		GL_COLOR_ZERO, GL_COLOR_MAX, GL_COLOR_ZERO, 0.49f,
		display_x, display_y, 0.0, 0.6f);
}

// ボタンを描写します
void draw_button(GLfloat left, GLfloat bottom, GLfloat right, GLfloat top,
	GLfloat *mat_ambient, GLfloat *mat_diffuse, GLfloat *mat_specular, GLfloat z, std::string str) {
	// ボーダーの色
	draw_rect_ex(left, bottom, right, top, mat_ambient, mat_diffuse, mat_specular, z,
		0.f, 0.f, 0.0, 1.f, 0.f);
	GLfloat border = 0.035f;
	// 上から白を透明度ありで重ねてメインの色とする
	draw_rect_ex(left + border, bottom + border, right - border, top - border, mat_ambient, GL_COLOR_BUTTON_ALPHA_WHITE, mat_specular, z + 0.01f,
		0.f, 0.f, 0.0, 1.f, 0.f);

	GLfloat font_size = 0.5f;
	GLfloat left_margin = font_size;
	// テキストを載せる
	draw_string(str, gl_string, 1.0f, 2.f, 1,
		GL_COLOR_ZERO, GL_COLOR_MAX, GL_COLOR_ZERO, z + 0.02f,
		left + left_margin, top, 0.0, font_size);
}


#endif
