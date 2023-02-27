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

void	free_n_quit(t_list *path)
{
	ft_lstclear(&path, free);
}

int	arg_checker(int argc, char **argv)
{
	if (argc < 5)
		return (0);
	if (!ft_strncmp(argv[1], "here_doc", ft_strlen("here_doc")) && argc < 6)
		return (0);
	ft_fprintf(1, "Pas de pb d'arg\n");
	return (1);
}

int	main(int argc, char **argv, char **envp)
{
	t_list	*path;
	t_files	files;

	if (!arg_checker(argc, argv))
		return (1);
	//infile = access(argv[1], F_OK);
	//outfile = access(argv[argc -1], F_OK);
	execve("ls", argv, envp);
	path = path_to_llist(envp);
	files = file_parser(argc, argv);
	free_n_quit(path);
	return (0);
}