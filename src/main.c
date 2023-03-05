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

void	exec_in_child(t_cmd *cmd, char **envp)
{
	if (!ft_strchr(cmd->cmd_name, '/') && cmd->exist)
	{
		ft_fprintf(2, "zsh: command not found: %s\n", cmd->cmd_name);
		return ;
	}
	if (execve(cmd->cmd_path, cmd->cmd_argv, envp) == -1)
	{
		perror("zsh error");
	}
	//exit(EXIT_FAILURE);
}

void	wait_for_pid(t_list *pid_lst)
{
	int	child_status;

	while (pid_lst)
	{
		ft_fprintf(1, "pid waited : %d\n", *((pid_t *)(pid_lst->content)));
		waitpid(*((pid_t *)(pid_lst->content)), &child_status, 0);
		pid_lst = pid_lst->next;
	}
}

int	pipex(t_cmd *cmd_list, t_files files, char **envp)
{
	t_list	*pid_lst;
	pid_t	pid;


	if (!files.here_doc && (files.in_exist || files.in_is_readbl))
	{
		if (files.in_exist)
			ft_fprintf(2, "zsh: no such file or directory: %s\n", files.infile);
		else if (files.in_is_readbl)
			ft_fprintf(2, "zsh: permission denied: %s\n", files.infile);
		cmd_list = cmd_list->next;
	}
	pid_lst = NULL;
	while (cmd_list)
	{
		pid = fork();
		ft_lstadd_back(&pid_lst, ft_lstnew(&pid));
		if (pid == -1)
			ft_fprintf(2, "Fork error for cmd:%s\n", cmd_list->cmd_name);
		if (pid == 0)
		{
			exec_in_child(cmd_list, envp);
		}
		cmd_list = cmd_list->next;
		if (pid > 0)
			wait_for_pid(pid_lst);
	}
	free_pid_lst(&pid_lst);
	return (0);
}

int	main(int argc, char **argv, char **envp)
{
	t_list	*path;
	t_files	files;
	t_cmd	*cmd_list;

	if (!arg_checker(argc, argv))
		return (1);

	path = path_to_llist(envp);
	files = file_parser(argc, argv);
	cmd_list = cmd_parser(argv, files.here_doc);
	set_cmd_infos(&cmd_list, path);
	//print_files(files);
	//print_cmd_list(cmd_list);
	//ouvrir les files et/ou gerer here_doc
	pipex(cmd_list, files, envp);






	free_n_quit(path, &cmd_list);
	return (0);
}
