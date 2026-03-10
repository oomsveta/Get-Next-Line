/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   get_next_line_bonus.c                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lwicket <lwicket@student.42belgium.be>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/08 20:37:07 by lwicket           #+#    #+#             */
/*   Updated: 2026/03/10 21:05:20 by lwicket          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <stddef.h>
#include <stdlib.h>
#include <unistd.h>
#include "get_next_line_bonus.h"

/**
 * @brief Resizes the content buffer to a larger capacity.
 *
 * This function implements a dynamic growth strategy for the buffer. It
 * reallocates the memory for `buffer->content` to a new, larger size,
 * preserving the existing data.
 *
 * The growth strategy is a hybrid approach to ensure both efficiency and
 * safety:
 * 1.  It first calculates a preferred new capacity by growing the current
 *     capacity by 50% (`capacity * 1.5`). This is done to amortize the cost of
 *     reallocations over many reads.
 * 2.  It then calculates the absolute minimum capacity required for the next
 *     read operation (`length + BUFFER_SIZE`).
 * 3.  It sets the final new capacity to the larger of these two values.
 *
 * @param[in,out] buffer A pointer to the `t_buffer` that needs to be resized.
 *
 * @return A pointer to the newly allocated memory block on success.
 * @return `NULL` on failure. Failure can occur if:
 *         - The `size_t` calculation for `new_capacity` overflows.
 *         - `malloc()` fails to allocate memory for the new buffer.
 */
static	char	*resize_buffer(t_buffer *buffer)
{
	char	*new_buffer;
	size_t	new_capacity;
	size_t	min_required_capacity;

	new_capacity = buffer->capacity + buffer->capacity / 2;
	min_required_capacity = buffer->length + BUFFER_SIZE;
	if (new_capacity < min_required_capacity)
	{
		new_capacity = min_required_capacity;
	}
	if (buffer->capacity >= new_capacity)
	{
		return (NULL);
	}
	new_buffer = malloc(new_capacity);
	if (!new_buffer)
	{
		return (NULL);
	}
	ft_memcpy(new_buffer, buffer->content, buffer->length);
	free(buffer->content);
	buffer->content = new_buffer;
	buffer->capacity = new_capacity;
	return (new_buffer);
}

/**
 * @brief Reads a chunk of data from a file descriptor into a buffer.
 *
 * This function reads up to `BUFFER_SIZE` bytes from the given file descriptor
 * @p fd and appends the data to the provided @p buffer.
 *
 * Before reading, it ensures there is enough capacity in the buffer for at
 * least one more read of `BUFFER_SIZE` bytes. If the remaining space
 * (`capacity - length`) is insufficient, it calls `resize_buffer()` to expand
 * the buffer's capacity.
 *
 * @param[in] fd The file descriptor to read data from.
 * @param[in,out] buffer A pointer to the buffer where the read data will be
 *                       stored. The buffer's state (`content`, `length`, and
 *                       potentially `capacity`) will be modified.
 *
 * @return On success, returns the number of bytes read (a positive `ssize_t`).
 * @return `0` if the end of the file is reached during the read attempt.
 * @return `-1` on error. This can happen if `resize_buffer()` fails (e.g.,
 *         malloc failure) or if the underlying `read()` system call fails.
 */
static ssize_t	read_more(int fd, t_buffer *buffer)
{
	ssize_t	bytes_read;

	if (buffer->capacity - buffer->length < BUFFER_SIZE)
	{
		if (!resize_buffer(buffer))
		{
			return (-1);
		}
	}
	bytes_read = read(fd, buffer->content + buffer->length, BUFFER_SIZE);
	if (bytes_read == -1)
	{
		return (-1);
	}
	buffer->length += bytes_read;
	return (bytes_read);
}

/**
 * @brief Searches for the end of a line within a buffer, reading more data if
 *        needed.
 *
 * The 'end of line' is defined as either:
 * 1. The first newline character (`\n`).
 * 2. The end of the file, if the file ends without a final newline.
 *
 * The function first inspects the existing content of the buffer. If a newline
 * is not found, it enters a loop that repeatedly calls `read_more()` to append
 * more data from the file descriptor until a newline is found or the end of the
 * file is reached.
 *
 * If the end of the file is reached (`read_more` returns 0) but there is still
 * data in the buffer, the function treats the very last character in the buffer
 * as the effective "end of the line" for the final, non-newline-terminated
 * line.
 *
 * @param[in] fd The file descriptor to read more data from if the EOL is not
 *               already in the buffer.
 * @param[in,out] buffer A pointer to the `t_buffer`. This buffer will be filled
 *                       with more data if the EOL is not initially present.
 *
 * @return On success, returns a pointer to the end-of-line character within
 *         `buffer->content`. This will be either a pointer to the `\n`
 *         character itself, or a pointer to the last character of the buffer if
 *         the end of the file was reached without a final newline.
 * @return `NULL` if the end of the file is reached and the buffer is completely
 *         empty, or if a read error occurs and there is no data left to process.
 */
static char	*fetch_eol(int fd, t_buffer *buffer)
{
	char	*eol_ptr;
	ssize_t	bytes_read;
	size_t	offset;

	eol_ptr = ft_memchr(buffer->content, '\n', buffer->length);
	while (!eol_ptr)
	{
		offset = buffer->length;
		bytes_read = read_more(fd, buffer);
		if (bytes_read > 0)
		{
			eol_ptr = ft_memchr(buffer->content + offset, '\n', bytes_read);
			continue ;
		}
		if (buffer->length == 0)
		{
			return (NULL);
		}
		eol_ptr = buffer->content + buffer->length - 1;
	}
	return (eol_ptr);
}

/**
 * @brief Extracts a single line from the buffer and prepares the buffer for the
 *        next call.
 *
 * This function performs two primary tasks:
 * 1.  **Extraction**: It isolates a line of text from the beginning of the
 *     buffer, identified by the provided @p eol_ptr (a pointer to end of the
 *     line). It allocates new memory for this line, copies the content
 *     (including the `\n` if any), and adds a null terminator.
 * 2.  **Buffer update**: After extraction, it updates the buffer by removing
 *     the extracted line. It does this by shifting the remaining data to the
 *     beginning of the buffer's content area and decreasing the buffer's
 *     length.
 *
 * @param[in,out] buffer A pointer to the `t_buffer` from which to extract the
 *                       line.
 * @param[in] eol_ptr    A pointer to the last character of the line. The last
 *                       character is `\n`, unless it is the last line of a file
 *                       not terminated by `\n`.
 *
 * @return On success, a pointer to a newly allocated, null-terminated string
 *         containing the extracted line.
 * @return `NULL` if memory allocation fails.
 */
static char	*extract_line(t_buffer *buffer, char *eol_ptr)
{
	char	*line;
	size_t	line_length;

	line_length = eol_ptr - buffer->content + 1;
	line = malloc(line_length + 1);
	if (!line)
	{
		return (NULL);
	}
	line[line_length] = '\0';
	ft_memcpy(line, buffer->content, line_length);
	buffer->length -= line_length;
	ft_memcpy(buffer->content, buffer->content + line_length, buffer->length);
	return (line);
}

/**
 * @brief Reads the next line from a given file descriptor.
 *
 * This function reads from a file descriptor @p fd and returns one line at a
 * time, defined as a sequence of characters ending with a newline (`\n`) or
 * the end of the file (EOF).
 *
 * @param[in] fd The file descriptor to read from. Must be a valid, open file
 *               descriptor.
 *
 * @return A pointer to a newly allocated, null-terminated string containing the
 *         line read from the file descriptor. This string includes the newline
 *         character if it was present.
 * @return `NULL` if there are no more lines to read, if a read error occurs,
 *         or if a memory allocation error occurs.
 *
 * @note The caller is responsible for freeing the memory of the string returned
 *       by this function.
 */
char	*get_next_line(int fd)
{
	static t_fd_state	*head = NULL;
	t_buffer			*buffer;
	char				*eol_ptr;
	char				*line;

	buffer = find_or_create_buffer(fd, &head);
	if (!buffer)
	{
		return (NULL);
	}
	eol_ptr = fetch_eol(fd, buffer);
	if (!eol_ptr)
	{
		dispose_buffer(fd, &head);
		return (NULL);
	}
	line = extract_line(buffer, eol_ptr);
	if (!line)
	{
		dispose_buffer(fd, &head);
		return (NULL);
	}
	return (line);
}
