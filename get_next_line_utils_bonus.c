/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   get_next_line_utils_bonus.c                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lwicket <lwicket@student.42belgium.be>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/08 20:38:14 by lwicket           #+#    #+#             */
/*   Updated: 2026/03/10 21:05:28 by lwicket          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <stdlib.h>
#include <stddef.h>		// provides NULL, size_t
#include <stdint.h>		// provides uint8_t, uint64_t, uintptr_t
#include <stdbool.h>	// provides bool
#include "get_next_line_bonus.h"

void	*ft_memchr(const void *buffer, int c, size_t n)
{
	const unsigned char	*bytes;

	bytes = buffer;
	c = (unsigned char)c;
	while (n != 0)
	{
		if (*bytes == c)
		{
			return ((void *)bytes);
		}
		bytes += 1;
		n -= 1;
	}
	return (NULL);
}

static void	*simple_copy(void *buffer1, const void *buffer2, size_t n)
{
	uint8_t			*dest;
	const uint8_t	*src;

	dest = buffer1;
	src = buffer2;
	while (n != 0)
	{
		*dest++ = *src++;
		n -= 1;
	}
	return (buffer1);
}

void	*ft_memcpy(void *buffer1, const void *buffer2, size_t n)
{
	uint8_t			*dest;
	const uint8_t	*src;
	uint64_t		*dest64;
	const uint64_t	*src64;

	if (((uintptr_t)buffer1 ^ (uintptr_t)buffer2) & 7)
	{
		return (simple_copy(buffer1, buffer2, n));
	}
	dest = buffer1;
	src = buffer2;
	while (n != 0 && ((uintptr_t)dest & 7))
	{
		*dest++ = *src++;
		n -= 1;
	}
	dest64 = (uint64_t *)dest;
	src64 = (const uint64_t *)src;
	while (n >= 8)
	{
		*dest64++ = *src64++;
		n -= 8;
	}
	return (simple_copy(dest64, src64, n));
}

t_buffer	*find_or_create_buffer(int fd, t_fd_state **head)
{
	t_fd_state	*current;

	current = *head;
	while (current)
	{
		if (current->fd == fd)
			return (&current->buffer);
		current = current->next;
	}
	current = malloc(sizeof(t_fd_state));
	if (!current)
		return (NULL);
	current->buffer.content = malloc(BUFFER_SIZE);
	if (!current->buffer.content)
	{
		free(current);
		return (NULL);
	}
	current->fd = fd;
	current->buffer.length = 0;
	current->buffer.capacity = BUFFER_SIZE;
	current->next = *head;
	*head = current;
	return (&current->buffer);
}

void	dispose_buffer(int fd, t_fd_state **head)
{
	t_fd_state	*current;
	t_fd_state	*prev;

	current = *head;
	prev = NULL;
	while (current && current->fd != fd)
	{
		prev = current;
		current = current->next;
	}
	if (prev)
	{
		prev->next = current->next;
	}
	else
	{
		*head = current->next;
	}
	free(current->buffer.content);
	free(current);
}
