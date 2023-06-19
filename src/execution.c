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

void	dupper(int new_fd, int old_fd, t_info *info)
{
	//ft_fprintf(2, "dupping for %s, newfd:%i, old:%i\n", info->cmd->cmd_name, new_fd, old_fd);
	if (dup2(new_fd, old_fd) == -1)
	{
		ft_fprintf(2, "cmd%s nfd%i ofd%i\n", info->cmd->cmd_name, new_fd, old_fd);
		perror("pipex (dup2)");
		close_n_free(info);
		exit(EXIT_FAILURE);
	}
}

int	dup_infile(t_info *info)
{
	int	fd;

	if (!info->files->in_e && !info->files->in_r)
	{
		fd = dupper(info->files->in_fd, 0, info);
		close(info->files->in_fd);
	}
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

void	dup_outfile(t_info *info)
{
	if (!info->files->out_w)
	{
		dupper(info->files->out_fd, 1, info);
		close(info->files->out_fd);
	}
	else
	{
		ft_fprintf(2, "%s%s\n", PDND, info->files->out);
		info->exit_code = 1;
		close_n_free(info);
		exit(EXIT_FAILURE);
	}
}

void	fork_pipe_n_dup(t_cmd *cmd, t_info *info, int *prevpipe, char **envp)
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
		close(pipefd[1]);
		dupper(*prevpipe, STDIN, info);
		close(*prevpipe);
		execute(cmd, info, envp);
	}
	else if (pid > 0)
	{
		close(pipefd[1]);
		close(*prevpipe);
		*prevpipe = pipefd[0];
	}
}

void	fork_pipe_n_dup_lst_cmd(t_cmd *cmd, t_info *info, int *prevpipe, char **envp)
{
	int	pid;

	pid = fork();
	info->last_pid = pid;
	if (pid == -1)
		perror("fork"); // + close n free ?
	if (pid == 0)
	{
		dupper(*prevpipe, STDIN, info);
		close(*prevpipe);
		dupper(info->files->out_fd, 1, info);
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
	if (cmd->exist)
	{
		if (!ft_strchr(cmd->cmd_name, '/'))
			ft_fprintf(2, "%s%s\n", CNFD, cmd->cmd_name);
		else
			ft_fprintf(2, "%s%s\n", NSFD, cmd->cmd_name);
		close_n_free(info);
		exit(EXIT_FAILURE);
	}
	//close_files(info->files);
	execve(cmd->cmd_path, cmd->cmd_argv, envp);
	perror("pipex (execve)");
	close_n_free(info);
	exit(EXIT_FAILURE);
}
