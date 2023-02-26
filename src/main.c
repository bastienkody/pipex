/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bguillau <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/02/25 18:40:25 by bguillau          #+#    #+#             */
/*   Updated: 2023/02/25 18:52:44 by bguillau         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../inc/pipex.h"

/* transform char *PATH to t_list */
t_list	*path_to_llist(char *path_line)
{
	t_list	*path;
	char	**tmp;
	int		i;

	tmp = ft_split(ft_strchr(path_line, '=') + 1, ':');
	path = NULL;
	i = -1;
	while (tmp[++i])
		ft_lstadd_back(&path, ft_lstnew(ft_strdup(tmp[i])));
	while (--i > -1)
		free(tmp[i]);
	free(tmp);
	return (path);
}

void	print_path(t_list *path)
{
	while (path)
	{
		ft_fprintf(1, "%s\n", path->content);
		path = path->next;
	}
}

/* get the t_list from the above fct */
int	main(int argc, char **argv, char **envp)
{
	int		res;
	t_list	*path;

	if (argc !=2)
		return (1);
	res = access(argv[1], F_OK);
	if (!res)
		ft_fprintf(1, "file [%s] exist\n", argv[1]);
	else
		ft_fprintf(1, "file [%s] doesnt exist\n", argv[1]);
	while (*envp)
	{
		if (!ft_strncmp("PATH", *envp, 4))
			path = path_to_llist(*envp);
		envp++;
	}
	print_path(path);
	ft_lstclear(&path, free);
	return (0);
}

