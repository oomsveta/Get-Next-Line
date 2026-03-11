/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   get_next_line_bonus.h                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lwicket <lwicket@student.42belgium.be>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/08 20:39:07 by lwicket           #+#    #+#             */
/*   Updated: 2026/03/11 22:07:37 by lwicket          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <stddef.h>	// provides size_t

#ifndef BUFFER_SIZE
# define BUFFER_SIZE 128
#endif
// 0x7ffff000 is the Linux kernel limit of maximum number of bytes per read.
#if BUFFER_SIZE == 0 || BUFFER_SIZE > 0x7ffff000
# error "Invalid buffer size"
#endif

typedef struct s_buffer
{
	unsigned char	*content;
	size_t			length;
	size_t			capacity;
}	t_buffer;

typedef struct s_fd_state
{
	t_buffer			buffer;
	struct s_fd_state	*next;
    int					fd;
}	t_fd_state;

char	*get_next_line(int fd);

// utils
void		*ft_memcpy(void *dest, const void *src, size_t n);
void		*ft_memchr(const void *buffer, int c, size_t n);
void		dispose_buffer(int fd, t_fd_state **head);
t_buffer	*find_or_create_buffer(int fd, t_fd_state **head);
