/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   get_next_line_utils_bonus.c                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lwicket <lwicket@student.42belgium.be>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/08 20:38:14 by lwicket           #+#    #+#             */
/*   Updated: 2026/11/17 19:01:18 by lwicket          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <stddef.h>					// provides NULL, size_t
#include <stdint.h>					// provides uintptr_t
#include <stdlib.h>					// provides free, malloc
#include "get_next_line_bonus.h"	// provides BUFFER_SIZE, t_fd_state, t_word

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
	unsigned char		*dest;
	const unsigned char	*src;

	dest = buffer1;
	src = buffer2;
	while (n != 0)
	{
		*dest++ = *src++;
		n -= 1;
	}
	return (buffer1);
}

/**
 * NOTE: This memcpy is called on overlapping buffers. I know this is safe in
 * the context of get_next_line because I control both the implementation of
 * memcpy and its callers: it copies forward, and the destination always comes
 * before the source. In any other context, memmove should be used instead.
 * The only reason I didn't use it is that I ran out of functions I could
 * define (this project allows only 10 functions in total).
 */
void	*ft_memcpy(void *buffer1, const void *buffer2, size_t n)
{
	unsigned char		*dest;
	const unsigned char	*src;
	t_word				*dest64;
	const t_word		*src64;

	if (((uintptr_t)buffer1 ^ (uintptr_t)buffer2) & (sizeof(t_word) - 1))
	{
		return (simple_copy(buffer1, buffer2, n));
	}
	dest = buffer1;
	src = buffer2;
	while (n != 0 && ((uintptr_t)dest & (sizeof(t_word) - 1)))
	{
		*dest++ = *src++;
		n -= 1;
	}
	dest64 = (t_word *)dest;
	src64 = (const t_word *)src;
	while (n >= sizeof(t_word))
	{
		*dest64++ = *src64++;
		n -= sizeof(t_word);
	}
	simple_copy(dest64, src64, n);
	return (buffer1);
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
