# SimpleScheme

SimpleScheme is a simple scheme interpreter implemented by C++ (C++11). It supports a subset of features of R5RS. The idea to develop such a project was inspired by [*SICP*](http://sarabander.github.io/sicp/html/index.xhtml).

## Compile and Run

The default compiler for this project is Clang and Boost Library, Bison, Flex is required. If you want to use GCC to compile this project, you have to modify makefile manually.

```bash
sudo apt-get install build-essential clang flex bison libboost-all-dev
git clone https://github.com/ZhangZhenghao/SimpleScheme.git
cd SimpleScheme/src
make
./build/main
```

We can enable log and statistic by setting variable `OPTIONS`:

```bash
make OPTIONS="-DLOG -DSTATS"
```

## Test

```bash
make test
```

Or run `driver.py`  in `test` directory. All test cases are included in `test` directory. A test case is considered  passed if there is no error.

## Features

- Parse S expression using flex and bison
- Implement evaluator using eval-apply model in *SICP*
- Implement closure using the concept of environment
- Implement garbage collection using mark-sweep algorithm
- Implement tail recursion optimzation
- Implement a few of primtive procedures
- Compact with most codes in *SICP*

### Types

- Rational
- Float
- Symbol
- String
- Pair
- Procedure

### Expressions

- define
- set!
- if
- cond
- let
- and/or
- lambda
- begin
- quote

### Procedures

- Check: number?, pair?, string?, etc.
- Arithmetic: +,-,*,/,etc.
- Comparation: <, >, =, <=, >=, etc.
- Logic: not
- Pair: cons, car, cdr, etc.
- String: number->string, etc.
- List: list, map, append, etc.
- I/O: read, display, etc.
- Debug: assert, assert=, etc.
- Advenced: apply, eval, etc.

## Note(Simplified Chinese)

- [Scheme解释器实现(一)：变量和环境](http://sine-x.com/scheme-evaluator-1/)

- [Scheme解释器实现(二)：解析](http://sine-x.com/scheme-evaluator-2/)

- [Scheme解释器实现(三)：解释](http://sine-x.com/scheme-evaluator-3/)

- [Scheme解释器实现(四)：优化](http://sine-x.com/scheme-evaluator-4/)
