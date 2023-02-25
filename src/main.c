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

/* turn from void to t_list */
void	path_to_llist(char *path)
{
	//t_list	path;
	char	**tmp;

	tmp = ft_split(ft_strchr(path, '=') + 1, ':');
	while (*tmp)
	{
		ft_fprintf(1, "%s\n", *tmp++);
	}
}

/* get the t_list from the above fct */
int	main(int argc, char **argv, char **envp)
{
	int		res;
	//t_list	path;

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
		{
			ft_fprintf(1,"%s\n", *envp);
			path_to_llist(*envp);
		}
		envp++;
	}
	return (0);
}

