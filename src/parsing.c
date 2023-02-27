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

t_files	file_parser(int argc, char **argv)
{
	t_files	files;

	if (!ft_strncmp(argv[1], "here_doc", ft_strlen("here_doc")))
	{
		files.infile = NULL;
		files.here_doc = 1;
		files.limiter = argv[2];
	}
	else
	{
		files.infile = argv[1];
		files.here_doc = 0;
		files.limiter = NULL;
	}
	files.outfile = argv[argc - 1];
	print_files(files);
	return (files);
}

/* 	Si cmd sans path (pas de '/')
		1:check in PATH. 2:check relative path (ou pas du tout!!). 3: Check accesibility 
	Si cmd avec path (avec un '/' dans le nom)
		1:check relative path 2:check in PATH. 3: Check accesibility 
a noter : execve marche avec un ./a.out (le ./ est accepte) et aussi sans (a.out dans dossier courant est execute)
A verifier egaleemnt pour access !!
*/
/*char	*acces_cmd_path(t_cmd *cmd, t_list *path)
{

}

t_cmd	cmd_parser(int argc, char **argv)
{

}
*/
