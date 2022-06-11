// FileUtils - File Reader Base Class
// by Frank Gennari
// 06/07/2022
#pragma once

#include <fstream>
#include <iostream>

unsigned const FILE_BUF_SZ = (1U<<16); // 64k

class base_file_reader {

protected:
	std::string filename;
	FILE *fp; // Note: we use a FILE* here instead of an ifstream because it's ~2.2x faster in MSVS
	static unsigned const MAX_CHARS = 1024;
	bool verbose;
	char buffer[MAX_CHARS]     = {0};
	char file_buf[FILE_BUF_SZ] = {0};
	unsigned file_buf_pos, file_buf_end;

	bool open_file(bool binary=0);
	void close_file();
	char get_next_char() {assert(fp); return get_char(fp);}
	static bool fast_isspace(char c) {return (c == ' ' || c == '\t' || c == '\n' || c == '\v' || c == '\f' || c == '\r');}
	static bool fast_isdigit(char c) {return (c >= '0' && c <= '9');}

	void unget_last_char(char c) {
		if (c == 0) return; // can't unget EOF
		assert(file_buf_pos > 0); // can't unget without previous get
		--file_buf_pos;
	}
	char get_char(FILE *fp_) {
		if (file_buf_pos == file_buf_end) { // fill file buffer
			file_buf_pos = 0;
			file_buf_end = fread(file_buf, 1, FILE_BUF_SZ, fp);
			if (file_buf_end == 0) return 0; // end of file
		}
		assert(file_buf_pos < file_buf_end);
		return file_buf[file_buf_pos++];
	}
	char get_char(std::ifstream &in) const {return in.get();}

	void strip_trailing_ws(string &str) const {
		while (!str.empty() && fast_isspace(str.back())) {str.pop_back();}
	}
	void add_char_to_str(char c, string &str) const {
		if (!fast_isspace(c) || !str.empty()) {str.push_back(c);}
	}
	template<typename T> void read_to_newline(T &stream, string *str=NULL, char comment_char='#') {
		bool prev_was_escape(0), saw_comment_char(0);

		while (1) {
			char const c(get_char(stream));
			if ((!prev_was_escape && c == '\n') || c == '\0' || c == 0) {
				if (str) {strip_trailing_ws(*str);}
				return;
			}
			prev_was_escape = (c == '\\'); // handle escape character at end of line
			if (str && !prev_was_escape) {
				if (c == comment_char) {saw_comment_char = 1;} // stop adding characters inside a comment
				if (!saw_comment_char) {add_char_to_str(c, *str);}
			}
		}
		assert(0); // never gets here
	}
	template<typename T> void read_str_to_newline(T &stream, string &str) {
		str.resize(0);

		while (1) {
			char const c(get_char(stream));
			if (c == '\n' || c == '\0' || c == 0) break; // end of file or line
			add_char_to_str(c, str);
		}
		strip_trailing_ws(str);
		return;
	}
	bool read_int(int &v);
	bool read_uint(unsigned &v);
	bool read_string(char *s, unsigned max_len);

public:
	base_file_reader(std::string const &fn) : filename(fn), fp(NULL), verbose(0), file_buf_pos(0), file_buf_end(0) {assert(!fn.empty());}
	~base_file_reader() {close_file();}
};

