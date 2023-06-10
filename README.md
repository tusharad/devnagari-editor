# devnagari-editor

A text editor with auto-complete functionality for devnagari (Hindi) text.

## Introduction

* The objective of this project is provide a text editor with auto-complete functionality such as:
    
    1. Suggesting hindi words from dictionary.
    2. Suggesting similar text from given documents.

## Technologies used
* C++
* GTK3

## Features
* Open, save text files.
* Fast suggestions of hindi words from dictionary.
* Finding similar texts from given large texts with extreme speed.
* Cache system for frequently used words.
* No freezing during searching due to multi-threading

## Installing dependancies

* gtk3

```bash
$ sudo apt install libgtkmm-3.0-dev
```

## How to run

Compile c++ program
```bash
$ g++ Main.cpp `pkg-config gtkmm-3.0 --cflags --libs`
```
Run your application
```bash
$ ./a.out
```

## Concepts used

1. C++ GTK3 for GUI
2. Object oriented concepts
3. Multi-threading
4. Priority_queue
5. String operations
6. Levenstien distance, sliding window
7. File handling
8. Cache mechanism
9. Fuzzy searching / Cosine similarity
10. Reading data in chunks