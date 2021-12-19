/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   get_next_line.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jkauppi <jkauppi@student.hive.fi>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2019/10/31 10:24:46 by jkauppi           #+#    #+#             */
/*   Updated: 2021/12/19 12:14:01 by jkauppi          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "get_next_line.h"

static int	ft_build_new_line(char *buffer, char **line,
		size_t index, t_list **elem_lst)
{
	t_list		*elem;
	t_list		*tmp;

	buffer[index] = '\0';
	if (elem_lst && *elem_lst)
	{
		elem = *elem_lst;
		while (elem)
		{
			tmp = elem->next;
			ft_strcat(*line, elem->content);
			ft_strdel((char **)&(elem->content));
			free(elem);
			elem = tmp;
		}
		free(elem);
		*elem_lst = NULL;
		ft_strcat(*line, buffer);
	}
	else
		ft_strcpy(*line, buffer);
	ft_strcpy(buffer, &buffer[index + 1]);
	return (1);
}

static size_t	ft_add_buf_lst(char *buffer,
		size_t index, t_list **elem_lst, size_t *num_of_char)
{
	t_list			*new_elem;

	if (index + BUFF_SIZE >= BUFF_SIZE * BUFF_FACTOR - 1)
	{
		new_elem = ft_lstnew(buffer, index + 1);
		if (*elem_lst)
			ft_lstadd_e(elem_lst, new_elem);
		else
			*elem_lst = new_elem;
		*num_of_char += index;
		index = 0;
		*buffer = '\0';
	}
	return (index);
}

static int	check_new_line(t_read_attrs *read_attrs, ssize_t *index,
					ssize_t ret)
{
	int			is_new_line;

	read_attrs->buffer[*index + ret] = '\0';
	read_attrs->match_ptr = ft_strchr(&read_attrs->buffer[*index], '\n');
	if (read_attrs->match_ptr)
	{
		*index = read_attrs->match_ptr - read_attrs->buffer;
		is_new_line = 1;
	}
	else
		is_new_line = 0;
	return (is_new_line);
}

static int	ft_read_fd_buffer(t_read_attrs *read_attrs, int fd, char **line,
		t_list **elem)
{
	ssize_t				index;
	ssize_t				ret;
	size_t				num_of_char;

	num_of_char = 0;
	index = ft_strlen(read_attrs->buffer);
	while (1)
	{
		ret = read(fd, &read_attrs->buffer[index], BUFF_SIZE);
		if (ret <= 0 || check_new_line(read_attrs, &index, ret))
			break ;
		index = ft_add_buf_lst(read_attrs->buffer, index + ret, elem,
				&num_of_char);
	}
	if (ret >= 0)
	{
		*line = ft_strnew(num_of_char + index);
		if (!ret)
			read_attrs->buffer[index + 1] = '\0';
		if (*read_attrs->buffer || *elem)
			ret = ft_build_new_line(read_attrs->buffer, line, index, elem);
		else
			ret = 0;
	}
	return (ret);
}

int	get_next_line(const int fd, char **line)
{
	static char		*fd_table[FD_SIZE];
	t_read_attrs	read_attrs;

	read_attrs.fd = fd;
	read_attrs.ret = -1;
	read_attrs.elem = NULL;
	if (line && (fd == 0 || fd > 2) && fd < FD_SIZE)
	{
		if (!fd_table[fd])
			fd_table[fd] = ft_strnew(BUFF_SIZE * BUFF_FACTOR);
		read_attrs.buffer = fd_table[fd];
		read_attrs.match_ptr = ft_strchr(read_attrs.buffer, '\n');
		if (read_attrs.match_ptr)
		{
			*line = ft_strnew(read_attrs.match_ptr - read_attrs.buffer);
			read_attrs.ret = ft_build_new_line(read_attrs.buffer, line,
					read_attrs.match_ptr - read_attrs.buffer, &read_attrs.elem);
		}
		else
		{
			read_attrs.ret = ft_read_fd_buffer(&read_attrs, fd, line,
					&read_attrs.elem);
			if (!(*read_attrs.buffer) && !read_attrs.elem)
				ft_memdel((void **)(&fd_table[fd]));
		}
	}
	return (read_attrs.ret);
}
