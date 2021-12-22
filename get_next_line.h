/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   get_next_line.h                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jkauppi <jkauppi@student.hive.fi>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2019/10/31 10:25:16 by jkauppi           #+#    #+#             */
/*   Updated: 2021/12/22 19:34:01 by jkauppi          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef GET_NEXT_LINE_H
# define GET_NEXT_LINE_H
# include "libft.h"
# include <sys/types.h>
# include <sys/uio.h>
# include <unistd.h>
# include <stdlib.h>
# define BUFF_SIZE 32
# define BUFF_FACTOR 10
# define FD_SIZE 3042

typedef enum e_bool
{
	E_FALSE,
	E_TRUE
}		t_bool;

typedef struct s_read_attrs
{
	char		*buffer;
	char		*read_ptr;
	char		*write_ptr;
	size_t		line_offset;
	t_list		*elem_lst;
	size_t		num_of_saved_char;
	t_bool		end_of_file;
}		t_read_attrs;

int		get_next_line(const int fd, char **line);

#endif
