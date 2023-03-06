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

void	exec_in_child(t_cmd *cmd, char **envp, t_files *files, int pipefd[2])
{
	if (!ft_strchr(cmd->cmd_name, '/') && cmd->exist)
	{
		ft_fprintf(2, "zsh: command not found: %s\n", cmd->cmd_name);
		return ;
	}
	if (cmd->next)										// pas la derniere commande : stdout to pipe entry
		dup2(pipefd[1], 1);
	else if (!files->out_is_writbl)						// derniere cmd et outfile writbl (wether just created or existing)
		dup2(files->out_fd, 1);
	if (!files->out_exist && files->out_is_writbl && cmd->next == NULL)
	{
			ft_fprintf(2, "zsh: permission deNied: %s for cmd %s\n", files->outfile, cmd->cmd_argv[1]);
			return ;
	}
	if (cmd->index == 0 && !files->in_is_readbl)		// premiere cmd infile ok : read stdin from infile
		dup2(files->in_fd, 0);
	if (cmd->index == 0 && files->in_is_readbl)
		return ;
	if (cmd->index != 0)								// autres cmd : read stdin from pipe exit
		dup2(pipefd[0], 0);
	close(pipefd[1]);
	close(pipefd[0]);
	close_files(files);									// close infile n outfile
	if (execve(cmd->cmd_path, cmd->cmd_argv, envp) == -1)
	{
		perror("zsh");
		return ;
	}
}

int	pipex(t_cmd *cmd, char **envp, t_files files)
{
	pid_t	pid;
	int		i;
	int		child_status;
	int		pipefd[2];

	if (pipe(pipefd) == -1)  // pipe(pipefd + 2) == -1
	{
		perror("pipe");
		exit(EXIT_FAILURE);
	}
	i= 0;
	while (cmd)
	{
		pid = fork();
		if (pid > 0)
			i++;
		if (pid == -1)
			perror("fork");
		if (pid == 0)
			exec_in_child(cmd, envp, &files, pipefd);
		cmd = cmd->next;
	}
	close(pipefd[1]);
	close(pipefd[0]);
	close_files(&files);	
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
	//print_files(files);
	//print_cmd_list(cmd_list);
	pipex(cmd_list, envp, files);





	close_files(&files);
	free_n_quit(path, &cmd_list);
	return (0);
}
