// FileUtils - Base File Reader Implementation
// by Frank Gennari
// 06/07/2022

#include "file_reader.h"
#include <stdint.h>

// hack to avoid slow multithreaded locking in getc()/ungetc() in MSVC++
#ifndef _getc_nolock
#define _getc_nolock   getc
#define _ungetc_nolock ungetc
#endif


bool base_file_reader::open_file(bool binary) {
	assert(!filename.empty());
	assert(!fp); // must call close_file() before reusing
	fp = fopen(filename.c_str(), (binary ? "rb" : "r"));
	if (!fp) {cerr << "Error: Could not open file " << filename << endl;}
	return (fp != 0);
}
void base_file_reader::close_file() {
	if (fp) {checked_fclose(fp);}
	fp = NULL;
}

bool base_file_reader::read_int(int &v) {
	bool first_char(1), is_neg(0);
	v = 0;

	while (1) {
		char const c(get_char(fp));
		if (first_char && fast_isspace(c)) continue; // skip leading whitespace
		if (first_char && c == '-') {is_neg = 1; first_char = 0; continue;} // negative
		if (!fast_isdigit(c)) {unget_last_char(c); break;} // non-integer character, unget it and finish
		v = 10*v + int(c - '0');
		first_char = 0;
	}
	if (first_char) return 0; // no integer characters were read
	if (is_neg) {v = -v;}
	return 1;
}
bool base_file_reader::read_uint(unsigned &v) {
	int temp(-1);
	if (!read_int(temp) || temp < 0) return 0;
	v = temp; // cast to unsigned
	return 1;
}

bool base_file_reader::read_string(char *s, unsigned max_len) {
	unsigned ix(0);
			
	while (1) {
		if (ix+1 >= max_len) return 0; // buffer overrun
		char const c(get_char(fp));
		if (c == 0) break;
		if (fast_isspace(c)) {
			if (ix == 0) continue; // leading whitespace
			if (c == '\n') {unget_last_char(c);} // preserve the newline
			break; // trailing whitespace
		}
		s[ix++] = c;
	}
	if (ix == 0) return 0; // nothing was read
	s[ix] = 0; // add null terminator
	return 1;
}
