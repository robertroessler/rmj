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
	js_val x{ js_arr{true, false, js_arr{0_js, 1_js}} };
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

	// ... then, use the js_val "indexing" operator[] 1- and multi- level...
	std::cout << "x[0]=" << std::boolalpha << x[0].as_bool() << std::endl;
	std::cout << "x[1]=" << std::boolalpha << x[1].as_bool() << std::endl;
	std::cout << "y[\"0\"]=" << y["0"].as_num() << std::endl;
	std::cout << "x[2][0]=" << x[2][0].as_num() << std::endl;
	std::cout << "a[\"a\"][4][\"ab\"]=" << a["a"][4]["ab"].as_num() << std::endl;
	x[2][0] = 42_js;
	std::cout << "x[2][0]=42_js, x[2][0]=" << x[2][0].as_num() << std::endl;

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
