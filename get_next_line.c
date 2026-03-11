/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   get_next_line.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lwicket <lwicket@student.42belgium.be>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/08 20:33:29 by lwicket           #+#    #+#             */
/*   Updated: 2026/03/11 23:24:53 by lwicket          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <stdlib.h>
#include <unistd.h>
#include "get_next_line.h"

#ifndef BUFFER_SIZE
# define BUFFER_SIZE 128
#endif
// 0x7ffff000 is the Linux kernel limit of maximum number of bytes per read.
#if BUFFER_SIZE <= 0 || BUFFER_SIZE > 0x7ffff000
# error "Invalid buffer size"
#endif

/**
 * Reallocates the buffer, aiming for a growth factor of 1.5x.
 * A growth factor smaller than 2 allows the memory allocator to eventually
 * reuse previously freed chunks, limiting memory fragmentation over time.
 *
 * Note: The 1.5x factor is not strictly respected for small capacities.
 * `ft_zmax` ensures the buffer always grows by at least `BUFFER_SIZE` to fit
 * the next read. The 1.5x geometric growth takes over once the buffer is large.
 *
 * The `buffer->capacity >= new_capacity` check detects unsigned integer
 * overflow, preventing allocation errors in the extreme case where capacity
 * exceeds SIZE_MAX.
 */
static unsigned char	*resize_buffer(t_buffer *buffer)
{
	unsigned char	*new_buffer;
	const size_t	new_capacity = ft_zmax(
			buffer->capacity + buffer->capacity / 2,
			buffer->length + BUFFER_SIZE);

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
 * Returns a pointer to the end of the line.
 *
 * The end of the line is defined as either the first '\n' character found, or
 * the last character currently in the buffer if the end of the file is reached
 * or a read error occurs.
 *
 * If the buffer is empty and no more data can be read, the function returns
 * NULL, signaling that we are done with this file.
 */
static unsigned char	*fetch_eol(int fd, t_buffer *buffer)
{
	unsigned char	*eol_ptr;
	ssize_t			bytes_read;
	size_t			offset;

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

static char	*extract_line(t_buffer *buffer, unsigned char *eol_ptr)
{
	const size_t	line_length = eol_ptr - buffer->content + 1;
	char			*line;

	line = malloc(line_length + 1);
	if (!line)
	{
		free(buffer->content);
		*buffer = (t_buffer){NULL, 0, BUFFER_SIZE};
		return (NULL);
	}
	line[line_length] = '\0';
	ft_memcpy(line, buffer->content, line_length);
	buffer->length -= line_length;
	ft_memmove(buffer->content, buffer->content + line_length, buffer->length);
	return (line);
}

char	*get_next_line(int fd)
{
	static t_buffer	buffer = {NULL, 0, BUFFER_SIZE};
	unsigned char	*eol_ptr;

	if (!buffer.content)
	{
		buffer.content = malloc(buffer.capacity);
		if (!buffer.content)
		{
			return (NULL);
		}
	}
	eol_ptr = fetch_eol(fd, &buffer);
	if (!eol_ptr)
	{
		free(buffer.content);
		buffer = (t_buffer){NULL, 0, BUFFER_SIZE};
		return (NULL);
	}
	return (extract_line(&buffer, eol_ptr));
}
