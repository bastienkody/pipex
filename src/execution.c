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

void	execute(t_info *info, char **envp)
{
	if (info->cmd->index == 0)
		dup_first_cmd(info);
	else if (info->cmd->next)
		dup_mid_cmd(info);
	else
		dup_last_cmd(info);
	if (info->cmd->exist)
	{
		if (!ft_strchr(info->cmd->cmd_name, '/'))
			ft_fprintf(2, "%s%s\n", CNFD, info->cmd->cmd_name);
		else
			ft_fprintf(2, "%s%s\n", NSFD, info->cmd->cmd_name);
		close_n_free(info);
		exit(EXIT_FAILURE);
	}
	close_pipefd(info->cmd_nb, info->pipefd);
	close_files(info->files);
	execve(info->cmd->cmd_path, info->cmd->cmd_argv, envp);
	perror("pipex (execve)");
	close_n_free(info);
	exit(EXIT_FAILURE);
}

void	dupper(int new_fd, int old_fd, t_info *info)
{
	if (dup2(new_fd, old_fd) == -1)
	{
		perror("pipex (dup2)");
		close_n_free(info);
		exit(EXIT_FAILURE);
	}
}

void	dup_mid_cmd(t_info *info)
{
	if (info->files->in_fd > 0)
		close(info->files->in_fd);
	if (info->cmd->index >= 2)
		close(info->pipefd[info->cmd->index - 2][READ_END]);
	close(info->pipefd[info->cmd->index - 1][WRITE_END]);
	close(info->pipefd[info->cmd->index][READ_END]);
	dupper(info->pipefd[info->cmd->index - 1][READ_END], 0, info);
	dupper(info->pipefd[info->cmd->index][WRITE_END], 1, info);
}

void	dup_first_cmd(t_info *info)
{
	close(info->pipefd[info->cmd->index][READ_END]);
	dupper(info->pipefd[info->cmd->index][WRITE_END], 1, info);
	if (!info->files->in_e && !info->files->in_r)
		dupper(info->files->in_fd, 0, info);
	else
	{
		close(info->pipefd[info->cmd->index][WRITE_END]);
		if (info->files->in_e)
			ft_fprintf(2, "%s%s\n", NSFD, info->files->inf);
		else if (info->files->in_r)
			ft_fprintf(2, "%s%s\n", PDND, info->files->inf);
		close_n_free(info);
		exit(EXIT_FAILURE);
	}
}

void	dup_last_cmd(t_info *info)
{
	close(info->pipefd[info->cmd->index - 1][WRITE_END]);
	if (info->cmd->index >= 2)
		close(info->pipefd[info->cmd->index - 2][READ_END]);
	dupper(info->pipefd[info->cmd->index - 1][READ_END], 0, info);
	if (!info->files->out_w)
		dupper(info->files->out_fd, 1, info);
	else
	{
		close(info->pipefd[info->cmd->index - 1][READ_END]);
		ft_fprintf(2, "pipex: permission denied: %s\n", info->files->out);
		info->exit_code = 1;
		close_n_free(info);
		exit(EXIT_FAILURE);
	}
}
