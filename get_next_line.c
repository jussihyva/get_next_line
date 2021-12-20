/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   get_next_line.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jkauppi <jkauppi@student.hive.fi>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2019/10/31 10:24:46 by jkauppi           #+#    #+#             */
/*   Updated: 2021/12/20 03:29:47 by jkauppi          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "get_next_line.h"

static int	ft_build_new_line(t_read_attrs *read_attrs)
{
	t_list		*elem;
	t_list		*tmp;
	int			ret;
	char		match_char;

	match_char = *read_attrs->match_ptr;
	*read_attrs->match_ptr = '\0';
	ret = 1;
	if (read_attrs->elem_lst)
	{
		elem = read_attrs->elem_lst;
		while (elem)
		{
			tmp = elem->next;
			ft_strcat(read_attrs->line, elem->content);
			ft_strdel((char **)&(elem->content));
			free(elem);
			elem = tmp;
		}
		ft_strcat(read_attrs->line, read_attrs->buffer);
		read_attrs->num_of_saved_char = 0;
		read_attrs->elem_lst = NULL;
	}
	else if (*read_attrs->buffer)
		ft_strcpy(read_attrs->line, read_attrs->buffer);
	if (match_char == '\n')
		ft_strcpy(read_attrs->buffer, read_attrs->match_ptr + 1);
	else
		read_attrs->buffer[0] = '\0';
	read_attrs->read_ptr = read_attrs->buffer;
	read_attrs->write_ptr -= read_attrs->index + 1;
	*read_attrs->write_ptr = '\0';
	return (ret);
}

static void	ft_add_buf_lst(t_read_attrs *read_attrs)
{
	t_list		*new_elem;
	size_t		index;

	index = read_attrs->write_ptr - read_attrs->buffer;
	if (index + BUFF_SIZE >= BUFF_SIZE * BUFF_FACTOR - 1)
	{
		new_elem = ft_lstnew(read_attrs->buffer, index + 1);
		if (read_attrs->elem_lst)
			ft_lstadd_e(&read_attrs->elem_lst, new_elem);
		else
			read_attrs->elem_lst = new_elem;
		read_attrs->num_of_saved_char += index;
		read_attrs->read_ptr = read_attrs->buffer;
		read_attrs->write_ptr = read_attrs->buffer;
	}
	else
		read_attrs->read_ptr = read_attrs->write_ptr;
	return ;
}

static int	my_get_line(t_read_attrs *read_attrs)
{
	t_bool	is_new_line;

	is_new_line = E_FALSE;
	read_attrs->match_ptr = ft_strchr(read_attrs->read_ptr, '\n');
	if (read_attrs->match_ptr)
	{
		read_attrs->read_ptr = read_attrs->match_ptr + 1;
		read_attrs->index = read_attrs->match_ptr - read_attrs->buffer;
		read_attrs->line = ft_strnew(read_attrs->num_of_saved_char
				+ read_attrs->index);
		read_attrs->ret = ft_build_new_line(read_attrs);
		is_new_line = E_TRUE;
	}
	else if (read_attrs->end_of_file)
	{
		read_attrs->match_ptr = ft_strchr(read_attrs->read_ptr, '\0');
		read_attrs->read_ptr = read_attrs->match_ptr;
		read_attrs->index = read_attrs->match_ptr - read_attrs->buffer;
		read_attrs->line = ft_strnew(read_attrs->num_of_saved_char
				+ read_attrs->index);
		read_attrs->ret = ft_build_new_line(read_attrs);
		if (read_attrs->line[0])
			read_attrs->ret = 1;
		else
			read_attrs->ret = 0;
		is_new_line = E_TRUE;
	}
	else
		ft_add_buf_lst(read_attrs);
	return (is_new_line);
}

static void	get_chars_from_file(t_read_attrs *read_attrs)
{
	read_attrs->ret = read(read_attrs->fd, read_attrs->write_ptr, BUFF_SIZE);
	if (read_attrs->ret > 0)
		read_attrs->write_ptr += read_attrs->ret;
	else if (read_attrs->ret == 0)
	{
		read_attrs->end_of_file = E_TRUE;
	}
	*read_attrs->write_ptr = '\0';
	return ;
}

int	get_next_line(const int fd, char **line)
{
	static t_read_attrs		read_attrs_array[FD_SIZE];
	t_read_attrs			*read_attrs;
	int						ret;

	if (line && (fd == 0 || fd > 2) && fd < FD_SIZE)
	{
		read_attrs = &read_attrs_array[fd];
		if (!read_attrs->buffer)
		{
			read_attrs->fd = fd;
			read_attrs->buffer = ft_strnew(BUFF_SIZE * BUFF_FACTOR);
			read_attrs->read_ptr = read_attrs->buffer;
			read_attrs->write_ptr = read_attrs->buffer;
		}
		while (read_attrs->ret >= 0 && !my_get_line(read_attrs))
			get_chars_from_file(read_attrs);
		ret = read_attrs->ret;
		*line = read_attrs->line;
	}
	else
		ret = -1;
	return (ret);
}
