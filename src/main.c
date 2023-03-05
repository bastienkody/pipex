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
		exit(EXIT_FAILURE);
	}
	if (execve(cmd->cmd_path, cmd->cmd_argv, envp) == -1)
	{
		perror("zsh");
		exit(EXIT_FAILURE);
	}
	//exit(EXIT_SUCCESS);
}

int	pipex(t_cmd *cmd, char **envp, t_files files)
{
	pid_t	pid;
	int		i;
	int		child_status;

	i= 0;
	while (cmd)
	{
		pid = fork();
		if (pid > 0)
		{
			ft_fprintf(1, "in parent, caring about child at pid :%i\n", pid);
			i++;
		}
		if (pid == -1)
			perror("fork");
		if (pid == 0)
			exec_in_child(cmd, envp);
		if (!files.out_exist && files.out_is_writbl && cmd->next == NULL)
			ft_fprintf(2, "zsh: permission denied: %s\n",files.outfile);
		cmd = cmd->next;
	}
	while (i--)
		waitpid(0, &child_status, 0);
	return (1);
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
	cmd_list = cmd_parser(argv, files, path);
	//print_cmd_list(cmd_list);
	pipex(cmd_list, envp, files);






	free_n_quit(path, &cmd_list);
	return (0);
}
