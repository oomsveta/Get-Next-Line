/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   get_next_line_bonus.c                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lwicket <lwicket@student.42belgium.be>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/08 20:37:07 by lwicket           #+#    #+#             */
/*   Updated: 2026/03/11 22:08:52 by lwicket          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <stddef.h>
#include <stdlib.h>
#include <unistd.h>
#include "get_next_line_bonus.h"

static unsigned char	*resize_buffer(t_buffer *buffer)
{
	unsigned char	*new_buffer;
	const size_t	min_required_capacity = buffer->length + BUFFER_SIZE;
	size_t			new_capacity;

	new_capacity = buffer->capacity + buffer->capacity / 2;
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
		return (NULL);
	}
	line[line_length] = '\0';
	ft_memcpy(line, buffer->content, line_length);
	buffer->length -= line_length;
	ft_memcpy(buffer->content, buffer->content + line_length, buffer->length);
	return (line);
}

char	*get_next_line(int fd)
{
	static t_fd_state	*head = NULL;
	t_buffer			*buffer;
	unsigned char		*eol_ptr;
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
