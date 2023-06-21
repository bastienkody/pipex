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

int	dupper(int old_fd, int new_fd, t_info *info)
{
	int	fd;

	fd = dup2(old_fd, new_fd) == -1;
	if (fd < -1)
	{
		perror("pipex (dup2)");
		close_n_free(info);
		exit(EXIT_FAILURE);
	}
	close(old_fd);
	return (fd);
}

int	dup_infile(t_info *info)
{
	if (!info->files->in_e && !info->files->in_r)
		return (dupper(info->files->in_fd, STDIN, info));
	else
	{
		if (info->files->in_e)
			ft_fprintf(2, "%s%s\n", NSFD, info->files->inf);
		else if (info->files->in_r)
			ft_fprintf(2, "%s%s\n", PDND, info->files->inf);
		close_n_free(info);
		exit(EXIT_FAILURE);
	}
}

int	dup_outfile(t_info *info)
{
	if (!info->files->out_w)
		return (dupper(info->files->out_fd, STDOUT, info));
	else
	{
		ft_fprintf(2, "%s%s\n", PDND, info->files->out);
		info->exit_code = 1;
		close_n_free(info);
		exit(EXIT_FAILURE);
	}
}

void	fork_pipe_dup(t_cmd *cmd, t_info *info, int *prevpipe, char **envp)
{
	int	pipefd[2];
	int	pid;

	if (pipe(pipefd) == -1)
		perror("while piping normal cmd");
	pid = fork();
	if (pid == -1)
		perror("fork"); // + close n free ?
	if (pid == 0)
	{
		close(pipefd[0]);
		dupper(pipefd[1], STDOUT, info);
		if (cmd->index)
			dupper(*prevpipe, STDIN, info);
		else
			dup_infile(info);
		execute(cmd, info, envp);
	}
	else if (pid > 0)
	{
		close(pipefd[1]);
		close(*prevpipe);
		*prevpipe = pipefd[0];
	}
}

void	fork_pipe_dup_lst(t_cmd *cmd, t_info *info, int *prevpipe, char **envp)
{
	int	pid;

	pid = fork();
	info->last_pid = pid;
	if (pid == -1)
		perror("fork"); // + close n free ?
	if (pid == 0)
	{
		dupper(*prevpipe, STDIN, info);
		dup_outfile(info);
		execute(cmd, info, envp);
	}
	else if (pid > 0)
	{
		close(*prevpipe);
		wait_cmds(info);
	}
}

void	execute(t_cmd *cmd, t_info *info, char **envp)
{
	char *cmd_argv[2];

	cmd_argv[0] = "";
	cmd_argv[1] = "";
	if (cmd->exist)
	{
		if (!ft_strchr(cmd->cmd_name, '/'))
			ft_fprintf(2, "%s%s\n", CNFD, cmd->cmd_name);
		else
			ft_fprintf(2, "%s%s\n", NSFD, cmd->cmd_name);
		close_n_free(info);
		exit(EXIT_FAILURE);
	}
	execve("", cmd_argv, envp);
	perror("pipex (execve)");
	close_n_free(info);
	exit(EXIT_FAILURE);
}
