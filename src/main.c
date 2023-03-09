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
	if (cmd->exist)														// cmd error : shell cmd vs exec
	{
		if (!ft_strchr(cmd->cmd_name, '/'))
			ft_fprintf(2, "pipex: command not found: %s\n", cmd->cmd_name);
		else
			ft_fprintf(2, "pipex: no such file or directory: %s\n", cmd->cmd_name);
		return ;
	}
	if (cmd->next)														// pas la derniere commande : stdout to pipe entry
		dup2(pipefd[1], 1);
	else if (!files->out_is_writbl)										// derniere cmd et outfile writbl (wether just created or existing)
		dup2(files->out_fd, 1);
	if (!files->out_exist && files->out_is_writbl && cmd->next == NULL)	// last cmd, outfile no_wrtbl : error printing + exit (no exec of lst cmd)
	{
			ft_fprintf(2, "pipex: permission deNied (outfile ; lst cmd wont be executted): %s for cmd %s\n", files->outfile, cmd->cmd_path);
			return ;
	}
	if (cmd->index == 0 && !files->in_is_readbl)						// first cmd infile ok : read stdin from infile
		dup2(files->in_fd, 0);
	if (cmd->index == 0 && files->in_is_readbl)							// first cmd infile no_redbl : 
		return ;
	if (cmd->index != 0)												// autres cmd : read stdin from pipe exit
		dup2(pipefd[0], 0);
	close(pipefd[1]);
	close(pipefd[0]);
	close_files(files);													// close infile n outfile
	if (execve(cmd->cmd_path, cmd->cmd_argv, envp) == -1) 				// handles args error (ie "ls : no such file or dir: /trucmuachinchose")
		perror("pipex");
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
	return (child_status);
}

int	main(int argc, char **argv, char **envp)
{
	t_list	*path;
	t_files	files;
	t_cmd	*cmd_list;
	int		lst_cmd_ex_code;

	if (!arg_checker(argc, argv))
		return (1);
	path = path_to_llist(envp);
	files = file_parser(argc, argv);
	cmd_list = cmd_parser(argv, files, path);
	//print_files(files);
	//print_cmd_list(cmd_list);
	lst_cmd_ex_code = pipex(cmd_list, envp, files);
	close_files(&files);
	free_n_quit(path, &cmd_list);
	return (lst_cmd_ex_code);
}
