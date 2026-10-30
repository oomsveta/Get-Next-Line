/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   get_next_line.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lwicket <lwicket@student.42belgium.be>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/08 20:33:29 by lwicket           #+#    #+#             */
/*   Updated: 2026/10/30 20:19:14 by lwicket          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <stdlib.h>
#include <unistd.h>
#include "get_next_line.h"

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
		free_buffer();
		return (NULL);
	}
	line[line_length] = '\0';
	ft_memcpy(line, buffer->content, line_length);
	buffer->length -= line_length;
	ft_memcpy(buffer->content, buffer->content + line_length, buffer->length);
	return (line);
}

/**
 * Getter to retrieve get_next_line's internal buffer.
 *
 * Useful to share it between different functions (get_next_line and
 * free_buffer).
 */
t_buffer	*gnl_get_buffer(void)
{
	static t_buffer	buffer = {NULL, 0U, 0U};

	return (&buffer);
}

char	*get_next_line(int fd)
{
	t_buffer		*buffer;
	unsigned char	*eol_ptr;

	buffer = gnl_get_buffer();
	if (!buffer->content)
	{
		buffer->content = malloc(BUFFER_SIZE);
		if (!buffer->content)
		{
			return (NULL);
		}
		buffer->capacity = BUFFER_SIZE;
	}
	eol_ptr = fetch_eol(fd, buffer);
	if (!eol_ptr)
	{
		free_buffer();
		return (NULL);
	}
	return (extract_line(buffer, eol_ptr));
}
