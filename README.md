*This project has been created as part of the 42 curriculum by lwicket.*

## ℹ️ Project Overview

> [!WARNING]
> This project is still a work in progress. Its contents, along with the information in this README file, are subject to change.

## 🛠️ Compilation & Usage

> [!WARNING]
> The I/O for this project is implemented on top of UNIX syscalls, which means you will need a UNIX-like system (such as Linux or macOS) to compile and run it.

TODO

## 📋 Coding Style

My coding style conforms to the [Norm](./norm.en.pdf), a set of strict styling guidelines enforced for all C exercises, which all students must follow under penalty of a zero grade.

Beyond purely cosmetic considerations, the Norm also forbids the following language constructs:

- `for` loops
- `do … while` loops
- `switch` statements
- `goto`
- the ternary operator
- declarations with initialization
- assignments inside controlling expressions

(non-exhaustive list)

> [!TIP]
> You can find the latest version of the Norm in the [Norminette repository](https://github.com/42School/norminette) – the program used to check conformity with the Norm. The version of the Norm included in this repository is the one that was in use when I completed this project.

## 🧑‍🔬 Implementation Notes

My implementation differs from the approach usually taken by 42 students to solve this exercise.

The main constraint of `get_next_line` is that it must read the file using a buffer of an arbitrary size that is fixed at compile-time. This buffer may be too large or too small for the size of the line that `get_next_line` must read, so a second buffer is necessary to hold the bytes waiting to be processed. These bytes exist either because the read was insufficient to reach the end of the line, or because we read more than necessary and must keep the extra bytes in memory to construct subsequent lines.

The handling of that second buffer was, in my view, the main avenue for improvement. In all the projects I looked at, students simply read the file in a loop until the end of the line is found. Each time they read, they append the newly-read bytes to their processing buffer using `ft_strjoin`. Because of this single design choice:
- Every single read triggers a malloc.
- Every single read triggers a free.
- Every single read triggers an ft_strcpy.

This is really bad news for performance. Not only is a malloc costly by itself, but repeated memory allocations and deallocations drastically increase the risk of memory fragmentation.

That's why I took a different path. Instead of reallocating the buffer at each read, I try to reuse the same buffer for every line. Here's what my buffer looks like:
```c
typedef struct s_buffer
{
    char    *content;
    size_t  length;
    size_t  capacity;
}   t_buffer;
```
The idea is simple: I want a buffer that is big enough to hold a line, but not so big that it wastes memory. To achieve this, I use a well-known technique for implementing dynamic arrays. I start with a small size that I expect to be good enough to hold a line (in this case, I start with `BUFFER_SIZE` because my buffer has to be at least that big). Whenever the length of the line outgrows the buffer capacity, I reallocate the buffer to be bigger using a growth factor of 1.5. It might still reallocate a few times, but the buffer size will eventually reach a sweet spot where it is large enough to hold any line without wasting too much memory.

Key benefits of this approach:
- `get_next_line` never reallocates memory if the current buffer size is big enough.
- If it isn't big enough, the optimal size is reached in logarithmic time.
- Memory fragmentation is kept to a minimum. The growth factor of 1.5 (instead of 2) allows the allocator to reuse previously freed memory, which improves cache locality.
- I know the exact size of the buffer at any moment, allowing me to use highly optimized memory functions instead of slower string functions. This also permits reading binary files without making any changes to the logic.

## 📚 References & Acknowledgments

- [Dynamic array](https://en.wikipedia.org/wiki/Dynamic_array) on Wikipedia
