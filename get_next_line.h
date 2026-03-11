/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   get_next_line.h                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lwicket <lwicket@student.42belgium.be>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/08 20:33:56 by lwicket           #+#    #+#             */
/*   Updated: 2026/03/11 22:01:41 by lwicket          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <stddef.h>	// provides size_t

typedef struct s_buffer
{
	unsigned char	*content;
	size_t			length;
	size_t			capacity;
}	t_buffer;

char	*get_next_line(int fd);

// utils
void	*ft_memcpy(void *dest, const void *src, size_t n);
void	*ft_memmove(void *dest, const void *src, size_t n);
void	*ft_memchr(const void *buffer, int c, size_t n);
size_t	ft_zmax(size_t a, size_t b);
