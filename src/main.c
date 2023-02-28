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

void	free_n_quit(t_list *path, t_cmd **cmd_list)
{
	ft_lstclear(&path, free);
	cmd_lstclear(cmd_list, free);
}

int	arg_checker(int argc, char **argv)
{
	if (argc < 5)
	{
		ft_fprintf(2, "%sProvide at least 4 arguments%s\n", REDBOLD, END);
		return (0);
	}
	if (!ft_strncmp(argv[1], "here_doc", ft_strlen("here_doc")) && argc < 6)
	{
		ft_fprintf(2, "%s5 arguments needed if here_doc%s\n", REDBOLD, END);
		return (0);
	}
	return (1);
}

int	main(int argc, char **argv, char **envp)
{
	t_list	*path;
	t_files	files;
	t_cmd	*cmd_list;

	if (!arg_checker(argc, argv))
		return (1);
	//infile = access(argv[1], F_OK);
	//outfile = access(argv[argc -1], F_OK);
	execve("ls", argv, envp);
	path = path_to_llist(envp);
	files = file_parser(argc, argv);
	cmd_list = cmd_parser(argv, files.here_doc);
	set_cmd_infos(&cmd_list, path);
	print_cmd_list(cmd_list);
	free_n_quit(path, &cmd_list);
	return (0);
}
