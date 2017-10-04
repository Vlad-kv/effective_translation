#include "visualization.h"

#include <vector>

const int H_TO_BRANCH = 1;
const int H_TO_CHILDRENS = 0;
const char VERTICAL_BRANCH = '|';
const char HORISONTAL_BRANCH = '-';

vector<string> field;

int calc_depth(expr_sp c) {
	if (c->not_terminal_name == "") {
		return 1 + H_TO_CHILDRENS;
	}
	int res = 0;
	if (c->child == nullptr) {
		return H_TO_CHILDRENS + 1;
	}
	for (expr_sp v = c->child; v; v = v->next) {
		res = max(res, calc_depth(v));
	}
	return res + H_TO_BRANCH + 2 + H_TO_CHILDRENS;
}
int calc_width(expr_sp c) {
	if (c->not_terminal_name == "") {
		return 1;
	}
	int res = -1;
	for (expr_sp v = c->child; v; v = v->next) {
		res += calc_width(v) + 1;
	}
	return max(res, (int)c->not_terminal_name.size());
}

void build(expr_sp c, int st_1, int st_2) {
	string& str = c->not_terminal_name;
	int size_2 = calc_width(c);
	
	for (int w = 0; w < H_TO_CHILDRENS; w++) {
		field[st_1 + w][st_2 + (size_2 - 1) / 2] = VERTICAL_BRANCH;
	}
	st_1 += H_TO_CHILDRENS;
	if (str == "") {
		field[st_1][st_2] = c->t.get_val();
		return;
	}
	int shift = (size_2 - str.size()) / 2;
	for (int w = 0; w < str.size(); w++) {
		field[st_1][st_2 + shift + w] = str[w];
	}
	if (c->child == nullptr) {
		return;
	}
	shift = (size_2 - 1) / 2;
	for (int w = 0; w < H_TO_BRANCH; w++) {
		field[st_1 + 1 + w][st_2 + shift] = VERTICAL_BRANCH;
	}
	st_1 += 1 + H_TO_BRANCH;
	for (int w = 0; w < size_2; w++) {
		field[st_1][w + st_2] = HORISONTAL_BRANCH;
	}
	st_1++;
	{
		int new_st_2 = st_2;
		for (expr_sp v = c->child; v; v = v->next) {
			build(v, st_1, new_st_2);
			new_st_2 += calc_width(v) + 1;
		}
	}
	st_1--;
	for (int w = 0; w < size_2; w++) {
		if (field[st_1 + 1][st_2 + w] == ' ') {
			field[st_1][st_2 + w] = ' ';
		} else {
			break;
		}
	}
	for (int w = size_2 - 1; w >= 0; w--) {
		if (field[st_1 + 1][st_2 + w] == ' ') {
			field[st_1][st_2 + w] = ' ';
		} else {
			break;
		}
	}
}

void visualize(expr_sp c) {
	int size_1 = calc_depth(c), size_2 = calc_width(c);
	for (int e = 0; e < size_1; e++) {
		field.push_back("");
		for (int r = 0; r < size_2; r++) {
			field[e].push_back(' ');
		}
	}
	build(c, 0, 0);
	for (auto &str : field) {
		cout << str << "\n";
	}
	field.clear();
}
