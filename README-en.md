# Tiny-JSON
![](https://img.shields.io/badge/c%2B%2B-11-blue) ![](https://img.shields.io/badge/release-v1.2-blue) ![](https://img.shields.io/badge/coverage-100%25-green) ![](https://img.shields.io/badge/license-mit-blue)

### Introduction
Tiny-JSON is a light-weight JSON/JSON5 liberary implemented by C++11. The coding style refers to Google C++ Style Guide, only need includes head file to import, and has no third party dependency.

[中文版本](https://github.com/Syan-Lin/Tiny-JSON/blob/main/README.md)

### Features
- Read JSON file
- Generate JSON file
- Support JSON5
- Extremly easy used
- Highly readable code
- Detailed error message

### How to use
Only includes `tiny_json.h` to your project, a compiler above 'c++11' is required.

### Examples
Compiler version: `gcc version 8.1.0 (x86_64-posix-seh-rev0)`

![code](Res/example.png)

>More examples in `example.cpp` and `Tests/class_test.cpp`

### Error prompt
Print colorful text in console in Windows and Linux is supported, with `tiny_json::detail` to set whether need to print error message.

Another way to get error message is through `tiny_json::parse(json, error)`, the error messege will write to second parameter.

![code](Res/error.png)

>More error prompt in `unit_test.cpp`, and set `detail` to `true` in line 33 to show.

### Tests
#### Unit test
This part includes third party `doctest`, only test files need this liberary.

Test coverage reaches 100% in core functions and classes.
```
[doctest] doctest version is "2.4.9"
[doctest] run with "--help" for options
===============================================================================
[doctest] test cases:  11 |  11 passed | 0 failed | 0 skipped
[doctest] assertions: 376 | 376 passed | 0 failed |
[doctest] Status: SUCCESS!
```
#### Performance tests
A simple performance test frame is defined in `tiny_json_performance.h`, which could set the scale of types separately to see its actual performance.

Test result:
-
>Tested in i5-12600KF, single thread

Test cases defined below:
- Null: `"#nullx": null`
- Array: `"#arrayx": [1, "string", true, null]`
- Object: `"#objectx": {{"number", 1}, {"string", "hello"}, {"bool", false}, {"null", null}}`
- String: `"#stringx": "Hello World!"`
- Number: `"#numberx": "31415"`
- Boolean: `"#boolx": true`
```

```

### UML Diagram
![uml](uml.jpg)
