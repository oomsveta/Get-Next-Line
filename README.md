*This project has been created as part of the 42 curriculum by lwicket.*

## ℹ️ Project Overview

> [!WARNING]
> This project is still a work in progress. Its contents, along with the information in this README file, are subject to change.

✨ 𝑔𝑒𝓉_𝓃𝑒𝓍𝓉_𝓁𝒾𝓃𝑒 ✨ is one of the first projects at [42 School](https://www.42network.org/). It consists of reimplementing a function similar to the POSIX [`getline(3p)`](https://pubs.opengroup.org/onlinepubs/9799919799/functions/getline.html) function, that is, a function that retrieves the next line from a given stream. It must be written in C on top of [`read(2)`](https://sites.uclouvain.be/SystInfo/manpages/man2/read.2.html) and [`malloc(3)` and `free(3)`](https://sites.uclouvain.be/SystInfo/manpages/man3/malloc.3.html). No other functions are allowed.

The stated objective is to get the student to learn the concept of [static local variables](https://en.wikipedia.org/wiki/Local_variable#Static_local_variables), but it's also a great opportunity to dive into buffering and file handling in C.

The constraint that makes the project interesting is that it has to work with a read buffer of arbitrary size, defined at compile time through a macro (`BUFFER_SIZE`). Because of that, the easy but slow way of reading the stream byte by byte until a newline is found is out of the question. It means we have to come up with solutions to handle the leftover bytes following a longer-than-one-line read, and reads that don't contain enough bytes to make a line.

Using [`lseek(2)`](https://sites.uclouvain.be/SystInfo/manpages/man2/lseek.2.html) or loading the whole file in memory is forbidden.

<img width="300" height="300" src="https://github.com/user-attachments/assets/8f2d6949-8c25-4dfa-94b1-b688fb34424b"/>

### Bonus Task

This project comes with an extra task for extra-motivated students.

The mandatory part of the assignment only asks you to make `get_next_line` work assuming there would only ever be one file being read at a time.

For the bonus part, `get_next_line` must be able to handle multiple files. This means we can't simply have one static buffer for all the leftover bytes: we need one buffer per file being read.

## 🛠️ Compilation & Usage

> [!WARNING]
> The I/O for this project is implemented on top of UNIX syscalls, which means you will need a UNIX-like system (such as Linux or macOS) to compile and run it.

> [!NOTE]
> The project is structured as per the assignment. It is not ready to use as is. This section itself is only there because the assignment requires it.

### Requirements

- GCC or Clang
- C99 support
- A UNIX-like system (such as Linux or macOS)

### Compilation

`get_next_line` doesn't feature a Makefile. You have to compile it yourself:

```shell
cc -c get_next_line_bonus.c get_next_line_utils_bonus.c -I.
```

Or update `-I.` to `-I<whatever path you put the .h files in>`.

Optionally, you can overwrite the size of the read buffer with `-DBUFFER_SIZE=`:

```shell
cc -c get_next_line_bonus.c get_next_line_utils_bonus.c -I. -DBUFFER_SIZE=128
```

> [!TIP]
> In case you don't provide a buffer size, it defaults to 128.

The resulting object files can then be linked into a binary the normal way.

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

## 🔬 Implementation Notes

My implementation differs from the approach usually taken by 42 students to solve this exercise.

The main constraint of `get_next_line` is that it must read the file using a buffer of an arbitrary size that is fixed at compile-time. This buffer may be too large or too small for the size of the line that `get_next_line` must read, so a second buffer is necessary to hold the bytes waiting to be processed. These bytes exist either because the read was insufficient to reach the end of the line, or because we read more than necessary and must keep the extra bytes in memory to construct subsequent lines.

### The Problem

In most student implementations, the handling of that second buffer is the main avenue for improvement. Often, students simply read the file in a loop until the end of the line is found. Each time they read, they append the newly read bytes to their processing buffer using "`ft_strjoin`" (a utility to concatenate two strings and allocate memory for the result), before replacing the buffer altogether with a new one. Because of this single design choice:
- Every single read triggers a malloc.
- Every single read triggers a free.
- Every single read triggers a strcpy.

This is really bad news for performance. Not only is a malloc costly by itself, but repeated memory allocations and deallocations drastically increase the risk of [memory fragmentation](https://en.wikipedia.org/wiki/Fragmentation_(computing)#Memory).

### The Solution

The solution to the problem above is straightforward: instead of reallocating the buffer at each read, try to reuse the same buffer for every line.

The idea is simple: I want a buffer that is big enough to hold a line, but not so big that it wastes memory. To achieve this, I implemented a data structure known as a [dynamic array](https://en.wikipedia.org/wiki/Dynamic_array). I start with a small size that I expect to be good enough to hold a line (in this case, I start with `BUFFER_SIZE` because my buffer has to be at least that big). Whenever the length of the line outgrows the buffer capacity, I reallocate the buffer to be bigger using a growth factor of 1.5. It might still reallocate a few times, but the buffer size will eventually reach a sweet spot where it is large enough to hold any line without wasting too much memory.

Key benefits of this approach:
- `get_next_line` never reallocates memory if the current buffer size is big enough.
- If it isn't big enough, the optimal size is reached in logarithmic time.
- Memory fragmentation is kept to a minimum. The growth factor of 1.5 (instead of 2) allows the allocator to reuse previously freed memory<sup>1</sup>, which improves cache locality.
- I know the exact size of the buffer at any moment, allowing me to use highly optimized memory functions instead of slower string functions. This also permits reading binary files without making any changes to the logic.

Here's what the dynamic array looks like along with examples of how it is used:

```c
typedef struct s_buffer
{
    char    *content;
    size_t  length;
    size_t  capacity;
}   t_buffer;
```
Reading to the buffer:
```c
// "if there is not enough room left for another read"
if (buffer->capacity - buffer->length < BUFFER_SIZE)
{
	resize_buffer(buffer);
}
ssize_t bytes_read = read(fd, buffer->content + buffer->length, BUFFER_SIZE);
buffer->length += bytes_read;
```
Copying from the buffer:
```c
size_t line_length = eol_ptr - buffer->content + 1;
char *line = malloc(line_length + 1);

line[line_length] = '\0';
memcpy(line, buffer->content, line_length);

// erasing content is just decrementing the length and shifting the data to the left
buffer->length -= line_length;
memmove(buffer->content, buffer->content + line_length, buffer->length);
```

## 📚 References & Acknowledgments

1. [Facebook's vector implementation](https://github.com/facebook/folly/blob/main/folly/docs/FBVector.md#memory-handling): on growth factors smaller than 2 allowing for memory reuse

### AI Usage

The whole code has been handcrafted with love by yours truly.

I used AI to proofread my README as I was writing it, but I didn't have it generate any of the content either. I've been increasingly stern about the AI keeping my tone and style, only fixing my grammar and *overly* unnatural wording ("overly" is its own addition by the way; I guess it's its way of signifying that my English is beyond repair lol).
