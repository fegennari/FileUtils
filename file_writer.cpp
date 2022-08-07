// FileUtils - File Writer Helper Functions
// by Frank Gennari
// 08/06/2022

#include <ostream>
using std::ostream;

template<typename T> void write_val(T const val, ostream &out) { // default
	out << val;
}
void write_int_val(int val, ostream &out) {
	if      (val < 0   ) {out.put('-'); val = -val;} // negative
	if      (val < 10  ) {out.put('0' + char(val));} // 1 digit
	else if (val < 100 ) {out.put('0' + char(val/10 )); out.put('0' + char(val%10));} // 2 digits
	else if (val < 1000) {out.put('0' + char(val/100)); out.put('0' + char((val/10)%10)); out.put('0' + char(val%100));} // 3 digits
	else {out << val;} // 4+ digits
}
void write_val(int            const val, ostream &out) {write_int_val(val, out);}
void write_val(short          const val, ostream &out) {write_int_val(val, out);}
void write_val(unsigned       const val, ostream &out) {write_int_val(val, out);}
void write_val(unsigned short const val, ostream &out) {write_int_val(val, out);}

void write_val(bool const val, ostream &out) {
	out.put(val ? '1' : '0');
}

