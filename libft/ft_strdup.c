/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_strdup.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jkauppi <marvin@42.fr>                     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2019/10/18 10:19:40 by jkauppi           #+#    #+#             */
/*   Updated: 2019/10/24 18:03:32 by jkauppi          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

char	*ft_strdup(const char *s1)
{
	char		*dst;

	dst = (char *)ft_memalloc(ft_strlen(s1) + 1);
	if (!dst)
		return (NULL);
	else
		return (ft_strcpy(dst, s1));
}
