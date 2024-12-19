# RMj

[![Build status](https://ci.appveyor.com/api/projects/status/vk34os2bldqog6u2?svg=true)](https://ci.appveyor.com/project/robertroessler/rmj)

The RMj (rmj) "mini" JSON parser project *can* be seen as YAJP (Yet Another JSON Parser) - OK, it
probably *will* be seen that way, as it is technically guilty as charged.

It is written in idiomatic modern C++ 20, but it has a key advantage:
it uses the **rva\:\:variant** template class from the
[Recursive Variant Authority](https://github.com/codeinred/recursive-variant)
to provide a pretty convincing simulation of an actual *recursive* "sum type" -
which, of course, is not provided by the C++ language at this time.

See the [recursive-variant](https://github.com/codeinred/recursive-variant)
project's site for more details - or just look at how rmj uses it in rmj.h.

__Note that rmj has been implemented as a C++ 20 "header-only library", so that to
be used, you only need to reference / include the "rmj.h" file - which will itself
reference variant.hpp - so these two files need to be present in your source folder,
or at least on your compile-time include path.__

The primary "user" (as well as "developer") documentation for rmj is present in
the rmj.h header file, while examples and a test harness are provided in "t0.cpp".

Besides being "pure" C++ 20, the code is believed to be both 32/64 -bit "safe", and
to contain no dependencies (overt *or* lurking) on any particular OS / hardware platform.

## Details

As I had need of a simple JSON parser, and didn't
require a whole ecosystem to be imported, I ended up creating rmj to just *be*
a simple JSON parser (as of RFC 8259) that implements a [static] **js_val\:\:parse** method -
yielding variant **js_val** objects  - and a **js_val\:\:to_string** method which serializes
to a **std\:\:string**.

The **parse** method  

(signature: **static js_val js_val\:\:parse(std\:\:string_view)**)  

accepts a string - expected to be valid [UTF-8 encoded] RFC 8259 JSON - and returns
a **js_val**, which is effectively a recursive sum type containing *all* JSON data
types and values mapped to C++ 20 data types and values.

If **parse** detects *invalid* JSON syntax it throws an exception (a **std\:\:runtime_error**)
with a message stating the problem and the precise offset of this error in the input
string.

The **to_string** method  

(signature: **constexpr std\:\:string to_string() const**)  

will serialize
its [recursive] **js_val** variant data as valid [UTF-8 encoded] RFC 8259 JSON.  However,
with the "v1" release of RMj, there is now an "optional" (defaulted) parameter to **to_string**:
just using **to_string()** with no parameter will default to "escaping" *any* UTF-8 sequence
that would result in "non-printable" characters (*i.e.*, non-ASCII)... passing a *true*
parameter will revert to "pass-through" mode, in which *any* valid UTF-8 sequence will
appear in the serialized output (excepting the required "always-escaped" control codes and
other RFC 8259-defined special characters).

But wait, there's more added for "v1": to make it easier to write code to display **js_val**
objects, the following two operators are also supplied (both based on the new, more nuanced
**to_string**_):

(signature: **inline std\:\:ostream& operator<<(std\:\:ostream&, const js_val&)**)

(signature: **inline std\:\:ostream& operator>>(std\:\:ostream&, const js_val&)**)

While these both use **to_string** to serialize their **js_val** to an output **std\:\:ostream**,
the former uses the default ("all non-ASCII is escaped") mode, resulting in guaranteed
printable output, while the latter uses the ("pass-through") mode, resulting in *pure* UTF-8.

Finally, for completeness, a C++ 20 "spaceship" **operator<=>** is supplied  

(signature: **constexpr auto operator<=>(const js_val&) const**)  

which is able to compare two **js_val** objects and return the results as a "three-way"
comparison, with *-1, 0, 1* representing the left-hand **js_val** object being *less than*,
*equal to*, or *greater than* the right-hand **js_val** object.

## More Details

### C++ [20] Language Issues

Note that the supplied **operator<=>** is in addition to the [defaulted] **operator==**,
and that using just these two, the C++ compiler is able to synthesize all of the "secondary"
comparison operators: **<, <=, !=, >=, >**... so we don't need to write them.

A word on the "choice" of the **operator>>** for "pass-through UTF-8 mode" stream output:
because of the limited availability of C++ operators for overloading to begin with, combined
with *any* other operator *not* having the same precedence / directional associativity as
**operator<<**, and we arrive at this operator usage... if something better presents itself -
or is suggested - this could change.

### Performance

While no "exotic" attempts were made to break any JSON parsing speed records, at
the same time, *some* efforts were made to not do a terrible job... the results are:

On a 12-th Gen Intel Core i7 12700K, the 1.4 KB file pass1.json from the json.org test suite
is parsed in ~49 microseconds, while **to_string** serialization of that parsed **js_val**
requires ~14 microseconds.

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
| *json-numeric-value* | (double) |
| *json-string-value* | (std\:\:string) |
| *json-object* | (std\:\:map\<std\:\:string, **js_val**\>) |
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