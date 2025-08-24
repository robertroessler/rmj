// t0.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#define _SILENCE_CXX20_U8PATH_DEPRECATION_WARNING 1

#include <string>
#include <iostream>
#include <fstream>
#include <filesystem>
#include <chrono>
#include "rmj.h"

namespace fs = std::filesystem;
namespace kr = std::chrono;
using namespace rmj;

int main(int argc, char* argv[])
{
	//std::locale::global(std::locale("en_US.UTF-8"));

	if (argc == 1) {
		// NO file supplied, so do BASIC low-level tests / diagnostics
		// first, create and test js_vals...
		js_val z{ true };
		std::cout << "z=" << z << std::endl;
		js_val z_z{ "z" };
		std::cout << "z_z=" << z_z << std::endl;
		js_val z_2 = z_z;
		std::cout << "z_2=" << z_2 << " (results of assignment z_2 = z_z)" << std::endl;
		js_val z_3 = std::move(z_z);
		std::cout << "z_3=" << z_3 << " (results of z_3 = std::move(z_z))" << std::endl;
		std::cout << "z_z=" << z_z << " (EMPTY after std::move assignment)" << std::endl;
		// N.B. - C++ initializer list values that are *numeric* MUST be doubles
		js_val x{ js_arr{true, false, js_arr{0_js, 1_js}} };
		// N.B. - this is C++, NOT JavaScript, so "objects" look like pairs when
		// appearing in initializer lists... in below examples, note lack of ':'
		// ... while the JSON would look like { "true" : false, "0" : 1.0 }
		js_val y{ js_obj{{"true", false}, {"0", 1.0}} };
		js_val a{
			js_obj{
				{"a", js_arr{
					true, nullptr, 1.234, "Hi!",
					js_obj{{"ab", 42.0}}}
				},
				{"b", double(0x7123456789012) /* 51 bits */ }
			}
		};
		if (z.is_null())
			std::cout << "got nullptr_t" << std::endl;
		if (z.is_bool())
			std::cout << "got bool" << std::endl;
		if (x.is_arr())
			std::cout << "got js_arr" << std::endl;
		if (y.is_obj())
			std::cout << "got js_obj" << std::endl;
		if (a.is_obj()) {
			std::cout << "got js_obj" << std::endl;
			std::cout << "got double=" << a["b"].as_num() << " (using default std::ostream output)" << std::endl;
			std::cout << "got double=" << a["b"] << " (using rmj::operator<< output)" << std::endl;
		}

		// ... then check out our "spaceship" and [default] == operators...
		// (not to mention the compiler-derived "secondary" comparisons)
		js_val p1{ false }, p2{ false };
		js_val q1{ false }, q2{ true };
		js_val r1{ js_obj{{"a", 42_js}} };
		js_val r2{ js_obj{{"b", 41_js}} };
		js_val s1{ js_arr{0_js, 1_js, 2_js} };
		js_val s2{ js_arr{0_js, 1_js, 3_js} };
		js_val s3{ js_arr{0_js, 1_js, 2_js, 3_js} };
		js_val s4{ js_arr{0_js, 1_js, 2_js, 1_js + 1_js + 1_js} };
		js_val s5{ js_arr{9_js} };
		js_val s6{ js_arr{9_js, "a"} };
		js_val s7{ js_arr{"a", 9_js} };
		// (we only need to do this once)
		std::cout << std::boolalpha;
		std::cout << "p1 == p2 -> " << (p1 == p2) << std::endl;
		std::cout << "p1 <=> p2 -> " << (p1 <=> p2) << std::endl;
		std::cout << "p1  <  p2 -> " << (p1 < p2) << std::endl;
		std::cout << "q1 == q2 -> " << (q1 == q2) << std::endl;
		std::cout << "q1 <=> q2 -> " << (q1 <=> q2) << std::endl;
		std::cout << "q1  <  q2 -> " << (q1 < q2) << std::endl;
		std::cout << "r1 == r2 -> " << (r1 == r2) << std::endl;
		std::cout << "r1 <=> r2 -> " << (r1 <=> r2) << std::endl;
		std::cout << "r1  <  r2 -> " << (r1 < r2) << std::endl;
		std::cout << "s1 == s2 -> " << (s1 == s2) << std::endl;
		std::cout << "s1 <=> s2 -> " << (s1 <=> s2) << std::endl;
		std::cout << "s1  <  s2 -> " << (s1 < s2) << std::endl;
		std::cout << "s2 <=> s3 -> " << (s2 <=> s3) << std::endl;
		std::cout << "s2  <  s3 -> " << (s2 < s3) << std::endl;
		std::cout << "s3 <=> s4 -> " << (s3 <=> s4) << std::endl;
		std::cout << "s3  <  s4 -> " << (s3 < s4) << std::endl;
		std::cout << "s1 <=> s4 -> " << (s1 <=> s4) << std::endl;
		std::cout << "s1  <  s4 -> " << (s1 < s4) << std::endl;
		std::cout << "s4 <=> s5 -> " << (s4 <=> s5) << std::endl;
		std::cout << "s4  <  s5 -> " << (s4 < s5) << std::endl;
		std::cout << "s5 <=> s6 -> " << (s5 <=> s6) << std::endl;
		std::cout << "s5  <  s6 -> " << (s5 < s6) << std::endl;
		std::cout << "s6 == s7 -> " << (s6 == s7) << std::endl;
		std::cout << "s6 <=> s7 -> " << (s6 <=> s7) << std::endl;
		std::cout << "s6  <  s7 -> " << (s6 < s7) << std::endl;

		// ... check that we can make a "deep" copy of a js_val...
		const auto x_backup{ x };
		std::cout << "BEFORE changes to x: x == x_backup -> " << (x == x_backup) << std::endl;
		std::cout << "x[2].as_arr().size()=" << x[2].as_arr().size() << std::endl;
		x[2].as_arr().push_back(2);
		std::cout << "x[2].as_arr().size()=" << x[2].as_arr().size() << std::endl;
		std::cout << "x_backup[2].as_arr().size()=" << x_backup[2].as_arr().size() << std::endl;
		std::cout << "AFTER changes to x: x == x_backup -> " << (x == x_backup) << std::endl;

		// ... then, use the js_val "indexing" operator[] 1- and multi- level...
		std::cout << "x[0]=" << x[0] << std::endl;
		std::cout << "x[1]=" << x[1] << std::endl;
		std::cout << "y[\"0\"]=" << y["0"] << std::endl;
		std::cout << "x[2][0]=" << x[2][0] << std::endl;
		std::cout << "a[\"a\"][4][\"ab\"]=" << a["a"][4]["ab"] << std::endl;
		// N.B. - in *assignments* we allow ANY believable *numeric* value when a
		// [JSON] number is desired, whereas C++ would NOT recognize an integer as a
		// [JSON] number, but it WOULD allow either nullptrs or bools to act like
		// numeric values, resulting in different [non-numeric] underlying js_vals!
		// ... but ALL of the below versions of "42*" result in the SAME js_val
		x[2][0] = 42;
		std::cout << "x[2][0]=42, x[2][0]=" << x[2][0] << std::endl;
		x[2][0] = 42.0;
		std::cout << "x[2][0]=42.0, x[2][0]=" << x[2][0] << std::endl;
		x[2][0] = 42_js;
		std::cout << "x[2][0]=42_js, x[2][0]=" << x[2][0] << std::endl;
		x[2][0] = 42.0f;
		std::cout << "x[2][0]=42.0f, x[2][0]=" << x[2][0] << std::endl;
		x[2][0] = 42ull;
		std::cout << "x[2][0]=42ull, x[2][0]=" << x[2][0] << std::endl;

		// ... how about some parsing and stringifying?
		const std::string simulated_BOM_1{ "\xef""\xbb""\xbf""1" };
		std::cout << "parse(simulated_BOM_1) -> "
			<< js_val::parse(simulated_BOM_1) << std::endl;
		std::cout << "parse(null) -> " << js_val::parse("null") << std::endl;
		std::cout << "parse(true) -> " << js_val::parse("true") << std::endl;
		std::cout << "parse(1) -> " << js_val::parse("1") << std::endl;
		std::cout << "parse(-1) -> " << js_val::parse("-1") << std::endl;
		std::cout << "parse(1.2) -> " << js_val::parse("1.2") << std::endl;
		std::cout << "parse(1.2e3) -> " << js_val::parse("1.2e3") << std::endl;
		std::cout << "parse(\"hello\") -> "
			<< js_val::parse("\"hello\"") << std::endl;
		std::cout << "parse(\"[ 1.2, 3.4 ]\") -> "
			<< js_val::parse("[ 1.2, 3.4 ]") << std::endl;

		// ... with "we're not in Kansas anymore" UTF-8...
		std::cout << "parse(null) -> " << js_val::parse("null") << std::endl;
		std::cout << "parse(false) -> " << js_val::parse("false") << std::endl;
		std::cout << "parse(true) -> " << js_val::parse("true") << std::endl;
		std::cout << "parse(2.34) -> " << js_val::parse("2.34") << std::endl;
		std::cout << "parse(\"abc\") -> " << js_val::parse("\"abc\"") << std::endl;
		std::cout << "parse([]) -> " << js_val::parse("[]") << std::endl;
		std::cout << "parse({}) -> " << js_val::parse("{}") << std::endl;
		const auto not_kansas{ "\"我能吞下玻璃而不伤身体\"" };
		std::cout << "pass_thru default of false:" << std::endl
			<< "\tparse(" << not_kansas << ").to_string()" << std::endl
			<< "\t-> " << js_val::parse(not_kansas) << std::endl;
		std::cout << "pass_thru set to true:" << std::endl
			<< "\tparse(" << not_kansas << ").to_string(true)" << std::endl
			<< "\t-> " << std::setw(pass_thru) << js_val::parse(not_kansas) << std::endl;
		std::cout << "parse(\"{ \"def\" : 42 }\") -> "
			<< js_val::parse("{ \"def\" : 42 }") << std::endl;
		std::cout << "parse(\"{ \"def\" : 42, \"ghi\" : 77 }\") -> "
			<< js_val::parse("{ \"def\" : 42, \"ghi\" : 77 }") << std::endl;
		std::cout << "parse(\"27\") -> " << js_val::parse("27") << std::endl;
		std::cout << "parse(\"27e0\") -> " << js_val::parse("27e0") << std::endl;
		std::cout << "parse(\"27e-10\") -> " << js_val::parse("27e-10") << std::endl;
		std::cout << "parse(\"2.7e10\") -> " << js_val::parse("2.7e10") << std::endl;
		std::cout << "parse(\"2.7e-10\") -> " << js_val::parse("2.7e-10") << std::endl;
		std::cout << "parse(\"2.7e+10\") -> " << js_val::parse("2.7e+10") << std::endl;
		std::cout << "parse(\"[27]\") -> " << js_val::parse("[27]") << std::endl;
		std::cout << "parse(\"[27,[42]]\") -> "
			<< js_val::parse("[27,[42]]") << std::endl;

		// ... wrap up the UTF-8 with some escapes...
		std::cout << R"(parse("\u0041BC") -> )"
			<< js_val::parse("\"\\u0041BC\"") << std::endl;
		const auto ev1{ R"("a\uD800\uDC00c")" };
		std::cout << "parse(" << ev1 << ") -> "
			<< js_val::parse(ev1) << " or [with pass_thru] "
			<< std::setw(pass_thru) << js_val::parse(ev1) << std::endl;
		const auto ev2{ R"("a\u0807\u0808c")" };
		std::cout << "parse(" << ev2 << ") -> "
			<< js_val::parse(ev2) << " or [with pass_thru] "
			<< std::setw(pass_thru) << js_val::parse(ev2) << std::endl;
		std::cout << "parse(\"<values-to-escape>\") -> "
			<< js_val::parse("\"\\b\\f\\n\\t\"") << std::endl;
		std::cout << "parse(\"<values-to-escape>\") -> "
			<< js_val::parse("\"\\u0000\"") << std::endl;
		std::cout << "parse(\"<values-to-escape>\") -> "
			<< js_val::parse("\"\\u0020\"") << std::endl;

#ifndef NO_FORMAT
		// ... let's see some of that again using our std::format support...
		std::cout <<
			std::format("show parse results using std::format:\n");
		std::cout <<
			std::format("\tparse(\"[ 1.2, 3.4 ]\")\n\t-> {}\n",
				js_val::parse("[ 1.2, 3.4 ]"));
		std::cout <<
			std::format("\tparse(\"我能吞下玻璃而不伤身体\") [using \"default\" stringify]\n\t-> {}\n",
				js_val::parse("\"我能吞下玻璃而不伤身体\""));
		std::cout <<
			std::format("\tparse(\"我能吞下玻璃而不伤身体\") [using \"pass_thru\" stringify]\n\t-> {:_}\n",
				js_val::parse("\"我能吞下玻璃而不伤身体\""));
#endif

		// ... finally, see if we can throw useful exceptions on parsing errors
		try {
			js_val::parse("27eNON-NUMBER");
		} catch (std::exception& e) {
			std::cerr << "parse(27eNON-NUMBER) -> "
				<< "Exception: " << e.what() << std::endl;
		}
		try {
			js_val::parse("[1,,]");
		} catch (std::exception& e) {
			std::cerr << "parse([1,,]) -> "
				<< "Exception: " << e.what() << std::endl;
		}
		try {
			js_val::parse("{\"a\":2,,}");
		} catch (std::exception& e) {
			std::cerr << "parse({\"a\":2,,}) -> "
				<< "Exception: " << e.what() << std::endl;
		}
	} else
		// [presumed] JSON file supplied, attempt to read, parse, and print it
		try {
			std::string text;
			const auto p = fs::path(argv[1]);
			// first, read the entire file into a std::string...
			if (std::ifstream f(p, std::ios::binary); f) {
				const auto n = size_t(fs::file_size(p)); // (can't throw)
				text.reserve(n), text.resize(n);
				const auto nn = f.read(text.data(), n).gcount();
				if (nn != n)
					throw std::invalid_argument("Unable to read "s + argv[1]);
			} else
				throw std::invalid_argument("Unable to open "s + argv[1]);
			std::cerr << "Parsing " << argv[1] << std::endl;
			const auto t0 = kr::steady_clock::now();
			// ... then, parse it (looking for legal JSON, and tracking time)...
			auto json = js_val::parse(text);
			const auto t1 = kr::steady_clock::now();
			// ... then stringify the parsed js_val...
			const auto text2 = json.to_string();
			const auto t2 = kr::steady_clock::now();
			// ... and display first the times, then the stringified output
			std::cerr << "parse time="
				<< kr::duration_cast<kr::microseconds>(t1 - t0).count()
				<< "mus" << std::endl;
			std::cerr << "to_string time="
				<< kr::duration_cast<kr::microseconds>(t2 - t1).count()
				<< "mus" << std::endl;
			std::cout << text2 << std::endl;
		} catch (std::exception& e) {
			std::cerr << "rmj::parse -> exception, 'what()'="
				<< e.what() << std::endl;
		}
}
