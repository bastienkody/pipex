/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parsing.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bguillau <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/02/26 14:38:45 by bguillau          #+#    #+#             */
/*   Updated: 2023/02/26 14:39:15 by bguillau         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../inc/pipex.h"

/* transform char *PATH to t_list */
t_list	*path_to_llist(char **envp)
{
	t_list	*path;
	char	**tmp;
	int		i;

	while (*envp)
	{
		if (!ft_strncmp("PATH", *envp, 4))
			break;
		envp++;
	}
	tmp = ft_split(ft_strchr(*envp, '=') + 1, ':');
	path = NULL;
	i = -1;
	while (tmp[++i])
		ft_lstadd_back(&path, ft_lstnew(ft_strdup(tmp[i])));
	while (--i > -1)
		free(tmp[i]);
	free(tmp);
	return (path);
}
