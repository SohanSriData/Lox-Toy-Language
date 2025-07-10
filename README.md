
# Toy Language Interpreter

A simple tree-walking interpreter for a custom programming language.

## Features

- Arithmetic expressions
- Variable declarations and assignments
- Control flow (`if`, `while`, etc.)
- User-defined functions
- **Coming soon**: User-defined classes, Arrays, Lists

## Getting Started

### Build

This project uses `make` for compilation.

To build the interpreter, run:

```bash
make
````

The compiled binary will be available at:

```bash
./build/main
```

### Run

You can run the interpreter in two ways:

#### 1. REPL Mode

Start an interactive session:

```bash
./build/main
```

#### 2. Script Mode

Execute a script file:

```bash
./build/main test.txt
```

The repository includes a sample script at `test.txt`.

## Example

```plaintext
var x = 10;
var y = x * 2;

if (y > 15) {
    print y;
}

fun greet(name) {
    print "Hello, " + name;
}

greet("World");
```

## Roadmap

* [x] Arithmetic expressions
* [x] Variable declarations
* [x] Control flow
* [x] User-defined functions
* [ ] User-defined classes

---

Feel free to clone, explore, and extend!
