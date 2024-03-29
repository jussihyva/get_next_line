/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_memalloc.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jkauppi <marvin@42.fr>                     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2019/10/21 08:45:35 by jkauppi           #+#    #+#             */
/*   Updated: 2019/10/25 07:49:57 by jkauppi          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <stdlib.h>
#include "libft.h"

void	*ft_memalloc(size_t size)
{
	void	*mem_area;

	if ((mem_area = (void *)(malloc(size))))
	{
		ft_bzero(mem_area, size);
		return (mem_area);
	}
	else
		return (NULL);
}
