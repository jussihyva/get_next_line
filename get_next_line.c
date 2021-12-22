/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   get_next_line.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jkauppi <jkauppi@student.hive.fi>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2019/10/31 10:24:46 by jkauppi           #+#    #+#             */
/*   Updated: 2021/12/22 20:02:37 by jkauppi          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "get_next_line.h"

static void	get_saved_buffers(t_list **elem_lst, char *buffer,
							char *line)
{
	t_list		*elem;
	t_list		*tmp;

	elem = *elem_lst;
	while (elem)
	{
		tmp = elem->next;
		ft_strcat(line, elem->content);
		ft_strdel((char **)&(elem->content));
		free(elem);
		elem = tmp;
	}
	ft_strcat(line, buffer);
	*elem_lst = NULL;
	return ;
}

static void	reset_line_offset(t_read_attrs *read_attrs, size_t index,
					char *match_ptr,
					char match_char)
{
	read_attrs->read_ptr = read_attrs->buffer;
	read_attrs->write_ptr -= read_attrs->line_offset + index;
	if (match_char == '\n')
	{
		read_attrs->write_ptr--;
		ft_strcpy(read_attrs->buffer, match_ptr + 1);
	}
	else
		read_attrs->buffer[0] = '\0';
	*read_attrs->write_ptr = '\0';
	read_attrs->line_offset = 0;
	return ;
}

static int	get_next_line_from_buffer(t_read_attrs *read_attrs, char **line,
						size_t index, char *match_ptr)
{
	int			ret;
	char		match_char;

	*line = ft_strnew(read_attrs->num_of_saved_char + index);
	match_char = *match_ptr;
	*match_ptr = '\0';
	ret = 1;
	if (read_attrs->elem_lst)
		get_saved_buffers(&read_attrs->elem_lst,
			read_attrs->buffer + read_attrs->line_offset, *line);
	else if (*(read_attrs->buffer + read_attrs->line_offset))
		ft_strcpy(*line, read_attrs->buffer + read_attrs->line_offset);
	read_attrs->num_of_saved_char = 0;
	if (match_char == '\0' || read_attrs->line_offset + index + BUFF_SIZE
		> BUFF_SIZE * BUFF_FACTOR)
		reset_line_offset(read_attrs, index, match_ptr, match_char);
	else
		read_attrs->line_offset += index + 1;
	if (!(*line)[0] && match_char == '\0')
		ret = 0;
	return (ret);
}

static void	save_buffer_if_full(t_read_attrs *read_attrs)
{
	t_list		*new_elem;
	size_t		index;

	index = read_attrs->write_ptr - read_attrs->buffer
		- read_attrs->line_offset;
	if (read_attrs->line_offset + index + BUFF_SIZE	> BUFF_SIZE * BUFF_FACTOR)
	{
		new_elem = ft_lstnew(read_attrs->buffer + read_attrs->line_offset,
				index + 1);
		if (read_attrs->elem_lst)
			ft_lstadd_e(&read_attrs->elem_lst, new_elem);
		else
			read_attrs->elem_lst = new_elem;
		read_attrs->num_of_saved_char += index;
		read_attrs->read_ptr = read_attrs->buffer;
		read_attrs->write_ptr = read_attrs->buffer;
		read_attrs->line_offset = 0;
	}
	else
		read_attrs->read_ptr = read_attrs->write_ptr;
	return ;
}

static int	take_next_line(t_read_attrs *read_attrs, int *ret, char **line)
{
	t_bool		is_new_line;
	size_t		index;
	char		*match_ptr;

	is_new_line = E_FALSE;
	match_ptr = ft_strchr(read_attrs->read_ptr, '\n');
	if (!match_ptr && read_attrs->end_of_file)
		match_ptr = read_attrs->write_ptr;
	if (match_ptr)
	{
		read_attrs->read_ptr = match_ptr + 1;
		index = match_ptr - read_attrs->buffer - read_attrs->line_offset;
		*ret = get_next_line_from_buffer(read_attrs, line, index, match_ptr);
		if (!(*ret))
			ft_strdel(&read_attrs->buffer);
		is_new_line = E_TRUE;
	}
	else
		save_buffer_if_full(read_attrs);
	return (is_new_line);
}

static t_read_attrs **get_read_attrs(const int fd)
{
	static t_read_attrs		*read_attrs_array[FD_SIZE];
	t_read_attrs			*read_attrs;

	if (!read_attrs_array[fd])
	{
		read_attrs = ft_memalloc(sizeof(*read_attrs));
		read_attrs->buffer = ft_strnew(BUFF_SIZE * BUFF_FACTOR);
		read_attrs->read_ptr = read_attrs->buffer;
		read_attrs->write_ptr = read_attrs->buffer;
		read_attrs_array[fd] = read_attrs;
	}
	return (&read_attrs_array[fd]);
}

int	get_next_line(const int fd, char **line)
{
	t_read_attrs		**read_attrs;
	int					ret;

	ret = -1;
	if (line && (fd == 0 || fd > 2) && fd < FD_SIZE)
	{
		read_attrs = get_read_attrs(fd);
		ret = BUFF_SIZE;
		while (ret >= 0 && !take_next_line(*read_attrs, &ret, line))
		{
			ret = read(fd, (*read_attrs)->write_ptr, BUFF_SIZE);
			(*read_attrs)->write_ptr += ret;
			*(*read_attrs)->write_ptr = '\0';
			if (ret != BUFF_SIZE)
				(*read_attrs)->end_of_file = E_TRUE;
			else
				(*read_attrs)->end_of_file = E_FALSE;
		}
		if (ret <= 0)
			ft_memdel((void **)read_attrs);
	}
	return (ret);
}
