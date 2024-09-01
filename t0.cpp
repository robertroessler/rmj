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
	//setlocale(LC_ALL, "en_US.utf8");

	// (this block is used for BASIC low-level tests / diagnostics)
#if 1
	std::cout << "Hello World!\n";

	// first, create and test js_vals...
	js_val z{ true };
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
		std::cout << "got double=" << a["b"].as_num() << std::endl;
	}

	// ... then check out our "spaceship" and [default] == operators...
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
	std::cout << "p1 == p2 -> " << std::boolalpha << (p1 == p2) << std::endl;
	std::cout << "p1 <=> p2 -> " << (p1 <=> p2) << std::endl;
	std::cout << "q1 <=> q2 -> " << (q1 <=> q2) << std::endl;
	std::cout << "r1 == r2 -> " << std::boolalpha << (r1 == r2) << std::endl;
	std::cout << "r1 <=> r2 -> " << (r1 <=> r2) << std::endl;
	std::cout << "s1 <=> s2 -> " << (s1 <=> s2) << std::endl;
	std::cout << "s2 <=> s3 -> " << (s2 <=> s3) << std::endl;
	std::cout << "s3 <=> s4 -> " << (s3 <=> s4) << std::endl;
	std::cout << "s1 <=> s4 -> " << (s1 <=> s4) << std::endl;
	std::cout << "s4 <=> s5 -> " << (s4 <=> s5) << std::endl;
	std::cout << "s5 <=> s6 -> " << (s5 <=> s6) << std::endl;
	std::cout << "s6 == s7 -> " << std::boolalpha << (s6 == s7) << std::endl;
	std::cout << "s6 <=> s7 -> " << (s6 <=> s7) << std::endl;

	// ... check that we can make a "deep" copy of a js_val...
	const auto x_backup{ x };
	std::cout << "BEFORE changes to x: x == x_backup -> " << std::boolalpha << (x == x_backup) << std::endl;
	std::cout << "x[2].as_arr().size()=" << x[2].as_arr().size() << std::endl;
	x[2].as_arr().push_back(2);
	std::cout << "x[2].as_arr().size()=" << x[2].as_arr().size() << std::endl;
	std::cout << "x_backup[2].as_arr().size()=" << x_backup[2].as_arr().size() << std::endl;
	std::cout << "AFTER changes to x: x == x_backup -> " << std::boolalpha << (x == x_backup) << std::endl;

	// ... then, use the js_val "indexing" operator[] 1- and multi- level...
	std::cout << "x[0]=" << std::boolalpha << x[0].as_bool() << std::endl;
	std::cout << "x[1]=" << std::boolalpha << x[1].as_bool() << std::endl;
	std::cout << "y[\"0\"]=" << y["0"].as_num() << std::endl;
	std::cout << "x[2][0]=" << x[2][0].as_num() << std::endl;
	std::cout << "a[\"a\"][4][\"ab\"]=" << a["a"][4]["ab"].as_num() << std::endl;
	// N.B. - in *assignments* we allow ANY believable *numeric* value when a
	// [JSON] number is desired, whereas C++ would NOT recognize an integer as a
	// [JSON] number, but it WOULD allow either nullptrs or bools to act like
	// numeric values, resulting in different [non-numeric] underlying js_vals!
	// ... but ALL of the below versions of "42*" result in the SAME js_val
	x[2][0] = 42;
	std::cout << "x[2][0]=42, x[2][0]=" << x[2][0].as_num() << std::endl;
	x[2][0] = 42.0;
	std::cout << "x[2][0]=42.0, x[2][0]=" << x[2][0].as_num() << std::endl;
	x[2][0] = 42_js;
	std::cout << "x[2][0]=42_js, x[2][0]=" << x[2][0].as_num() << std::endl;
	x[2][0] = 42.0f;
	std::cout << "x[2][0]=42.0f, x[2][0]=" << x[2][0].as_num() << std::endl;
	x[2][0] = 42ull;
	std::cout << "x[2][0]=42ull, x[2][0]=" << x[2][0].as_num() << std::endl;

	// ... how about some parsing and stringifying?
	const std::string simulated_BOM_1{ "\xef""\xbb""\xbf""1" };
	std::cout << "parse(simulated_BOM_1)=" << js_val::parse(simulated_BOM_1).to_string() << std::endl;
	std::cout << "parse(null)=" << js_val::parse("null").to_string() << std::endl;
	std::cout << "parse(true)=" << std::boolalpha << js_val::parse("true").to_string() << std::endl;
	std::cout << "parse(1)=" << js_val::parse("1").to_string() << std::endl;
	std::cout << "parse(-1)=" << js_val::parse("-1").to_string() << std::endl;
	std::cout << "parse(1.2)=" << js_val::parse("1.2").to_string() << std::endl;
	std::cout << "parse(1.2e3)=" << js_val::parse("1.2e3").to_string() << std::endl;
	std::cout << "parse(\"hello\")=" << js_val::parse("\"hello\"").to_string() << std::endl;
	std::cout << "parse(\"[ 1.2, 3.4 ]\") => got array=" << std::boolalpha << js_val::parse("[ 1.2, 3.4 ]").is_arr() << std::endl;

	// ... with "we're not in Kansas anymore" UTF-8...
	std::cout << "to_string(parse(null))=" << js_val::parse("null").to_string() << std::endl;
	std::cout << "to_string(parse(false))=" << js_val::parse("false").to_string() << std::endl;
	std::cout << "to_string(parse(true))=" << js_val::parse("true").to_string() << std::endl;
	std::cout << "to_string(parse(2.34))=" << js_val::parse("2.34").to_string() << std::endl;
	std::cout << "to_string(parse(\"abc\"))=" << js_val::parse("\"abc\"").to_string() << std::endl;
	std::cout << "to_string(parse([]))=" << js_val::parse("[]").to_string() << std::endl;
	std::cout << "to_string(parse({}))=" << js_val::parse("{}").to_string() << std::endl;
	std::cout << "to_string(parse(\"我能吞下玻璃而不伤身体\"))=" << js_val::parse("\"我能吞下玻璃而不伤身体\"").to_string() << std::endl;
	std::cout << "to_string(parse(\"{ \"def\" : 42 }\"))=" << js_val::parse("{ \"def\" : 42 }").to_string() << std::endl;
	std::cout << "to_string(parse(\"{ \"def\" : 42, \"ghi\" : 77 }\"))=" << js_val::parse("{ \"def\" : 42, \"ghi\" : 77 }").to_string() << std::endl;
	std::cout << "to_string(parse(\"[27]\")=" << js_val::parse("[27]").to_string() << std::endl;
	std::cout << "to_string(parse(\"[27,[42]]\")=" << js_val::parse("[27,[42]]").to_string() << std::endl;

	// ... wrap up the UTF-8 with some escapes...
	std::cout << R"(to_string(parse("\u0041BC")=)" << js_val::parse("\"\\u0041BC\"").to_string() << std::endl;
	std::cout << R"(to_string(parse("a\uD800\uDC00c")=)" << js_val::parse("\"a\\uD800\\uDC00c\"").to_string() << std::endl;
	std::cout << R"(to_string(parse("a\u0807\u0808c")=)" << js_val::parse("\"a\\u0807\\u0808c\"").to_string() << std::endl;
	std::cout << "to_string(\"<values-to-escape>\"))=" << js_val::parse("\"\\b\\f\\n\\t\"").to_string() << std::endl;
	std::cout << "to_string(\"<values-to-escape>\"))=" << js_val::parse("\"\\u0000\"").to_string() << std::endl;
	std::cout << "to_string(\"<values-to-escape>\"))=" << js_val::parse("\"\\u0020\"").to_string() << std::endl;

	// ... finally, see if we can throw useful exceptions on parsing errors
	try {
		std::cout << "to_string(parse([1,,]))=" << std::flush, std::cout << js_val::parse("[1,,]").to_string() << std::endl;
	} catch (std::exception& e) {
		std::cerr << "Exception: " << e.what() << std::endl;
	}
	try {
		std::cout << "to_string(parse({\"a\":2,,}))=" << std::flush, std::cout << js_val::parse("{\"a\":2,,}").to_string() << std::endl;
	} catch (std::exception& e) {
		std::cerr << "Exception: " << e.what() << std::endl;
	}
#endif

	// (this block is used to read, parse, and print a JSON file)
#if 1
	if (argc > 1) {
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
			std::cerr << "parse time=" << kr::duration_cast<kr::microseconds>(t1 - t0).count() << "mus" << std::endl;
			std::cerr << "to_string time=" << kr::duration_cast<kr::microseconds>(t2 - t1).count() << "mus" << std::endl;
			std::cout << text2 << std::endl;
		} catch (std::exception& e) {
			std::cerr << "rmj::parse => exception, 'what()'=" << e.what() << std::endl;
		}
	}
#endif
}
