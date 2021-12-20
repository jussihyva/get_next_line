/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   get_next_line.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jkauppi <marvin@42.fr>                     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2019/10/31 10:24:46 by jkauppi           #+#    #+#             */
/*   Updated: 2019/11/05 08:36:19 by jkauppi          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "get_next_line.h"

static int			ft_build_new_line(char **buffer, char **line,
		char *match_ptr, t_list **elem_lst)
{
	t_list		*elem;
	t_list		*tmp;

	*match_ptr = '\0';
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
		ft_strcat(*line, *buffer);
	}
	else
		*line = ft_strcpy(*line, *buffer);
	*buffer = ft_strcpy(*buffer, match_ptr + 1);
	return (1);
}

static size_t		ft_add_buf_lst(char *buffer,
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

static int			ft_reply(char **buffer, size_t num_of_char,
		char **line, t_list **elem)
{
	*line = ft_memalloc((sizeof(char) * ft_strlen(*buffer) + num_of_char + 1));
	if (**buffer || *elem)
	{
		return (ft_build_new_line(buffer, line, *(buffer)
					+ ft_strlen(*(buffer)), elem));
	}
	else
		ft_memdel((void **)(buffer));
	return (0);
}

static int			ft_read_fd_buffer(char **buffer, int fd, char **line,
		t_list **elem)
{
	ssize_t				index;
	ssize_t				ret;
	char				*match_ptr;
	size_t				num_of_char;

	num_of_char = 0;
	index = ft_strlen(*buffer);
	while ((ret = read(fd, *buffer + index, BUFF_SIZE)) > 0)
	{
		*(char *)(*buffer + index + ret) = '\0';
		if ((match_ptr = ft_strchr(*buffer + index, '\n')))
		{
			*line = ft_memalloc((sizeof(char) * ft_strlen(*buffer) +
						num_of_char + 1));
			return (ft_build_new_line(buffer, line, match_ptr, elem));
		}
		index += ret;
		index = ft_add_buf_lst(*buffer, index, elem, &num_of_char);
	}
	if (!ret)
		*(*buffer + index + 1) = '\0';
	if (ret)
		return (ret);
	ret = ft_reply(buffer, num_of_char, line, elem);
	return (ret);
}

int					get_next_line(const int fd, char **line)
{
	static char			*fd_table[FD_SIZE];
	char				*match_ptr;
	ssize_t				ret;
	t_list				*elem;

	elem = NULL;
	if (line && (fd == 0 || fd > 2) && fd < FD_SIZE)
	{
		if (!*(fd_table + fd))
			*(fd_table + fd) = (ft_strnew(BUFF_SIZE * BUFF_FACTOR));
		if ((match_ptr = ft_strchr(*(fd_table + fd), '\n')))
		{
			*line = ft_memalloc(((match_ptr - fd_table[fd] + 1)));
			return (ft_build_new_line((fd_table + fd), line, match_ptr, &elem));
		}
		else
		{
			ret = (ft_read_fd_buffer(fd_table + fd, fd, line, &elem));
			return (ret);
		}
	}
	return (-1);
}
