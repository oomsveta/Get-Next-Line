/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   get_next_line.h                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lwicket <lwicket@student.42belgium.be>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/08 20:33:56 by lwicket           #+#    #+#             */
/*   Updated: 2026/10/26 19:08:20 by lwicket          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef GET_NEXT_LINE_H
# define GET_NEXT_LINE_H

# ifndef BUFFER_SIZE
#  define BUFFER_SIZE 128
# endif
// 0x7ffff000 is the Linux kernel limit of maximum number of bytes per read.
# if BUFFER_SIZE <= 0 || BUFFER_SIZE > 0x7ffff000
#  error "Invalid buffer size"
# endif

# include <stdint.h>	// provides uintptr_t
# include <stddef.h>	// provides size_t

typedef struct s_buffer
{
	unsigned char	*content;
	size_t			length;
	size_t			capacity;
}	t_buffer;

char			*get_next_line(int fd);
t_buffer		*gnl_get_buffer(void);

// utils
void			*ft_memcpy(void *dest, const void *src, size_t n);
void			free_buffer(void);
void			*ft_memchr(const void *buffer, int c, size_t n);
size_t			ft_zmax(size_t a, size_t b);

#endif	/* GET_NEXT_LINE_H */
