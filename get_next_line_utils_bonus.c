/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   get_next_line_utils_bonus.c                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lwicket <lwicket@student.42belgium.be>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/08 20:38:14 by lwicket           #+#    #+#             */
/*   Updated: 2026/03/11 23:25:14 by lwicket          ###   ########.fr       */
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

/**
 * Returns the buffer associated with @p fd. If it does not exist in
 * @p active_fds, creates a new node and prepends it to the list.
 *
 * Note: Prepending optimizes for temporal locality. A newly opened file
 * descriptor is highly likely to be read from immediately and repeatedly,
 * so placing it at the head of the list ensures O(1) access time.
 */
t_buffer	*find_or_create_buffer(int fd, t_fd_state **active_fds)
{
	t_fd_state	*current;

	current = *active_fds;
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
	current->next = *active_fds;
	*active_fds = current;
	return (&current->buffer);
}

void	dispose_buffer(int fd, t_fd_state **active_fds)
{
	t_fd_state	*to_delete;

	while (*active_fds)
	{
		if ((*active_fds)->fd == fd)
		{
			to_delete = *active_fds;
			*active_fds = to_delete->next;
			free(to_delete->buffer.content);
			free(to_delete);
			return ;
		}
		active_fds = &(*active_fds)->next;
	}
}
