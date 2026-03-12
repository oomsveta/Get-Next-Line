/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   get_next_line_utils.c                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lwicket <lwicket@student.42belgium.be>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/08 20:35:44 by lwicket           #+#    #+#             */
/*   Updated: 2026/03/12 22:21:26 by lwicket          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <stddef.h>		// provides NULL, size_t
#include <stdint.h>		// provides uint8_t, uint64_t, uintptr_t

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

void	*ft_memmove(void *dest, const void *src, size_t n)
{
	char		*d;
	const char	*s;

	if (dest == src)
	{
		return (dest);
	}
	d = dest;
	s = src;
	if ((uintptr_t)d - (uintptr_t)s < n)
	{
		d += n;
		s += n;
		while (n != 0)
		{
			*--d = *--s;
			n -= 1;
		}
		return (dest);
	}
	return (ft_memcpy(d, s, n));
}

size_t	ft_zmax(size_t a, size_t b)
{
	if (a > b)
	{
		return (a);
	}
	return (b);
}
