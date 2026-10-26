/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   get_next_line_utils.c                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lwicket <lwicket@student.42belgium.be>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/08 20:35:44 by lwicket           #+#    #+#             */
/*   Updated: 2026/10/26 20:12:04 by lwicket          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "get_next_line.h"	// provides BUFFER_SIZE
#include <stddef.h>			// provides NULL, size_t
#include <stdlib.h>			// provides free

#if defined(__has_attribute)
# if __has_attribute(destructor)

void	free_buffer(void) __attribute__((destructor));
# endif
#endif

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

void	*ft_memcpy(void *buffer1, const void *buffer2, size_t n)
{
	unsigned char		*dest;
	const unsigned char	*src = buffer2;

	dest = buffer1;
	while (n != 0)
	{
		*dest++ = *src++;
		n -= 1;
	}
	return (buffer1);
}

size_t	ft_zmax(size_t a, size_t b)
{
	if (a > b)
	{
		return (a);
	}
	return (b);
}

/* Reallocates the buffer, aiming for a growth factor of 1.5x.
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
unsigned char	*resize_buffer(t_buffer *buffer)
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

void	free_buffer(void)
{
	t_buffer	*buffer;

	buffer = gnl_get_buffer();
	free(buffer->content);
	*buffer = (t_buffer){NULL, 0, BUFFER_SIZE};
}
