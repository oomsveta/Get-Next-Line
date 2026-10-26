/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   get_next_line_utils.c                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lwicket <lwicket@student.42belgium.be>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/08 20:35:44 by lwicket           #+#    #+#             */
/*   Updated: 2026/10/26 19:25:00 by lwicket          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <stddef.h>			// provides NULL, size_t

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
