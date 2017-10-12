#include <iostream>
#include <fstream>
using namespace std;

string file_to_execute = "..\\bin\\Debug\\parser_by_hands.exe";

bool is_file_exist(const string& filename) {
	std::ifstream fs(filename);
	return fs.is_open();
}
void execute_and_check(const string& filename, const string& expexted_result) {
	string result;
	{
		ifstream z(filename);
		string line;
		while (!z.eof()) {
			getline(z, line);
			if (line == "###") {
				break;
			}
			result += line + "\n";
		}
		if (line != "###") {
			throw runtime_error("test " + filename + " has no description");
		}
	}
	{
		ofstream x("input.txt");
		x << result;
	}
	system(file_to_execute.data());
	{
		ifstream z("output.txt");
		string first;
		z >> first;
		if (first != expexted_result) {
			throw runtime_error("in " + filename + " expected : " +
					expexted_result + " , found : " + first);
		}
	}
}
void check_collection(string description, string expexted_result) {
	for (int no = 1; no < 100; no++) {
		string filename = "test_" + description + "_";
		if (no < 10) {
			filename.push_back('0');
		}
		filename += to_string(no) + ".txt";
		if (!is_file_exist(filename)) {
			break;
		}
		cout << filename << " : ";
		try {
			execute_and_check(filename, expexted_result);
		} catch (...) {
			cout << "\n";
			throw;
		}
		cout << "ok\n";
	}
}
int main() {
	int q, w, e, r, t;
	try {
		check_collection("correct", "OK");
		check_collection("incorrect", "Error");
	} catch (runtime_error error) {
		cout << "Error : " << error.what() << "\n";
		return 0;
	}
	cout << "OK";
	return 0;
}
