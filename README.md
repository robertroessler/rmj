# RMj

[![Build status](https://ci.appveyor.com/api/projects/status/vk34os2bldqog6u2?svg=true)](https://ci.appveyor.com/project/robertroessler/rmj)

The RMj (rmj) "mini" JSON parser project *can* be seen as YAJP (Yet Another JSON Parser) - OK, it
probably *will* be seen that way, as it is technically guilty as charged.

It is written in fairly idiomatic modern C++ 20, but it has a key advantage:
it uses the **rva::variant** template class from the
[Recursive Variant Authority](https://github.com/codeinred/recursive-variant)
to provide a pretty convincing simulation of an actual recursive "sum type" -
which, of course, is not provided by the C++ language at this time.

See the [recursive-variant](https://github.com/codeinred/recursive-variant)
project's site for more details - or just look at how rmj uses it in rmj.h.

__Note that rmj has been implemented as a C++ 20 "header-only library", so that to
be used, you only need to reference / include the "rmj.h" file - which will itself
reference variant.hpp - so these two files need to be present in your source folder,
or at least on your compile-time include path.__

The primary "user" (as well as "developer") documentation for rmj is present in
the rmj.h header file, while examples and a test harness are provided in "t0.cpp".

Besides being "pure" C++, the code is believed to be both 32/64 -bit "safe", and
to contain no dependencies (overtly or lurking) on Windows.

## Details

As I had need of a simple JSON parser, and didn't
require a whole ecosystem to be imported, I ended up creating rmj to just *be*
a simple JSON parser (as of RFC 8259) that implements a [static] parse method -
yielding variant **js_val** objects  - and a **js_val** to_string method which returns
a std::string.

The parse method (signature: static **js_val** parse(std::string_view src)) accepts a
string - expected to be valid [UTF-8 encoded] RFC 8259 JSON - and returns a **js_val**, which is
effectively a recursive sum type containing *all* JSON data types and values mapped
to C++ 20 data types and values.

If parse detects *invalid* JSON syntax it throws an exception (a std::runtime_error)
with a message stating the problem and the precise offset of this error in the input
string.

The to_string method (signature: constexpr std::string to_string()) will serialize
its [recursive] **js_val** variant data as valid [UTF-8 encoded] RFC 8259 JSON.

## More Details

### Performance

While no "exotic" attempts were made to break any JSON parsing speed records, at
the same time, *some* efforts were made to not do a terrible job... the results are:

On a 12-th Gen Intel Core i7 12700K, the file pass1.json from the json.org test suite
is parsed in ~55 microseconds, while to_string serialization of that parsed **js_val**
requires ~16 microseconds.

### JSON -> C++ 20 Type and Value Mapping

Finally, the mapping of JSON data types and values to C++ 20 data types and values
shouldn't really contain any surprises, with reasonably "direct" mappings suggesting
themselves in all cases - as shown in the following table, which details the
correspondence between JSON data types and values and the C++ **js_val** recursive
sum type:

| JSON Data Type or Value | **js_val** (C++ 20 "std\:\:variant" with the listed elements) |
| --- | --- |
| *null* | *nullptr* (nullptr_t) |
| *true* / *false* | *true* / *false* (bool) |
| \#json-numeric-value\# | (double) |
| \#json-string-value\# | (std::string) |
| *json-object* | (std\:\:map\<std::string, **js_val**\>) |
| *json-array* | (std\:\:vector\<**js_val**\>) |

### Implementation Notes

As mentioned above, a "header-only library" is supplied, consisting of the local
file "rmj.h" in conjunction with the imported file "variant.hpp".

While the rmj.h header does implement the basic parsing and serializing described
above - plus some helper functions to make C++ use a little friendlier, the real
magic that enables the above is suppled by the
[recursive-variant](https://github.com/codeinred/recursive-variant)
project, contributing the variant.hpp header which allows at least the illusion
of a recursive sum type in C++ 20!

The extensively commented code in rmj.h and variant.hpp show both how the **js_val**
type is implemented, as well as *its use in its own definition*, with the file
"t0.cpp" present both for testing and illustrating standard use cases.

Note that the variant.hpp file from the
[recursive-variant](https://github.com/codeinred/recursive-variant)
project is included here in the rmj project in slightly modified form to simplify
"dependency" issues, as well as to include a minor syntactic fix necessitated by a
later version of C++, and to include the license from that project.