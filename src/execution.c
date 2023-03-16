/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   execution.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bguillau <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/03/15 12:06:13 by bguillau          #+#    #+#             */
/*   Updated: 2023/03/15 12:06:29 by bguillau         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../inc/pipex.h"

void	execute(t_cmd *cmd, char **envp)
{
	if (cmd->exist)
	{
		if (!ft_strchr(cmd->cmd_name, '/'))
			ft_fprintf(2, "pipex: command not found: %s\n", cmd->cmd_name);
		else
			ft_fprintf(2, "pipex: no such file or directory: %s\n", cmd->cmd_name);
		exit(EXIT_FAILURE);
	}
	execve(cmd->cmd_path, cmd->cmd_argv, envp);
	perror("pipex (execve)");
	exit(EXIT_FAILURE);
}

void	exec_mid_cmd(t_cmd *cmd, char **envp, t_files *files, int **pipefd)
{
	if (files->in_fd > 0)
		close(files->in_fd);
	if (cmd->index >= 2)
		close(pipefd[cmd->index - 2][READ_END]);
	close(pipefd[cmd->index - 1][WRITE_END]);
	close(pipefd[cmd->index][READ_END]);
	if (dup2(pipefd[cmd->index - 1][READ_END], 0) == -1)
		ft_fprintf(2, "dup error on pipe %i READ END before exec cmd %s\n", cmd->index - 1, cmd->cmd_name);
	if (dup2(pipefd[cmd->index][WRITE_END], 1) == -1)
		ft_fprintf(2, "dup error on pipe %i WRITE END before exec cmd %s\n", cmd->index, cmd->cmd_name);
	execute(cmd, envp);
}

void	exec_first_cmd(t_cmd *cmd, char **envp, t_files *files, int **pipefd)
{
	close(pipefd[cmd->index][READ_END]);
	if (dup2(pipefd[cmd->index][WRITE_END], 1) < 0)
		ft_fprintf(2, "dup error on pipe %i WRITE END before exec first cmd %s\n", cmd->index, cmd->cmd_name);
	if (!files->here_doc)
	{
		if (!files->in_exist && !files->in_is_readbl)
			dup2(files->in_fd, 0);
		else
		{
			close(pipefd[cmd->index][WRITE_END]);
			if (files->in_exist)
				ft_fprintf(2, "pipex: no such file or directory: %s\n", files->infile);
			else if (files->in_is_readbl)
				ft_fprintf(2, "pipex: permission denied: %s\n", files->infile);
			exit(EXIT_FAILURE);
		}
	}
	else
	{
		//create pipe (+int fd[2])
		//dup write end for here_doc_fct
		//call here_doc with write end of pipe
	}
	execute(cmd, envp);
}

void	exec_last_cmd(t_cmd *cmd, char **envp, t_files *files, int **pipefd)
{
	close(pipefd[cmd->index - 1][WRITE_END]);
	if (cmd->index >= 2)
		close(pipefd[cmd->index - 2][READ_END]);
	dup2(pipefd[cmd->index - 1][READ_END], 0);
	if (!files->out_is_writbl)
		dup2(files->out_fd, 1);
	else
	{
		close(pipefd[cmd->index - 1][READ_END]);
		ft_fprintf(2, "pipex: permission denied: %s\n", files->outfile);
		exit(EXIT_FAILURE);
	}
	execute(cmd, envp);
}
