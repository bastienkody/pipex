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

//void	execute(t_cmd *cmd, char **envp, t_files *files, int **pipefd)
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
			ft_fprintf(2, "pipex: command not found: %s\n", info->cmd->cmd_name);
		else
			ft_fprintf(2, "pipex: no such file or directory: %s\n", info->cmd->cmd_name);
		close_n_free(info);
		exit(EXIT_FAILURE);
	}
	close_pipefd(info->cmd_nb, info->pipefd);
	close_files(info->files);
	execve(info->cmd->cmd_path, info->cmd->cmd_argv, envp);
	perror("pipex (execve)");
	exit(EXIT_FAILURE);
}

void	dup_mid_cmd(t_info *info)
{
	if (info->files->in_fd > 0)
		close(info->files->in_fd);
	if (info->cmd->index >= 2)
		close(info->pipefd[info->cmd->index - 2][READ_END]);
	close(info->pipefd[info->cmd->index - 1][WRITE_END]);
	close(info->pipefd[info->cmd->index][READ_END]);
	if (dup2(info->pipefd[info->cmd->index - 1][READ_END], 0) == -1)
		ft_fprintf(2, "dup error on pipe %i READ END before exec cmd %s\n", info->cmd->index - 1, info->cmd->cmd_name);
	if (dup2(info->pipefd[info->cmd->index][WRITE_END], 1) == -1)
		ft_fprintf(2, "dup error on pipe %i WRITE END before exec cmd %s\n", info->cmd->index, info->cmd->cmd_name);
}

void	dup_first_cmd(t_info *info)
{
	close(info->pipefd[info->cmd->index][READ_END]);
	if (dup2(info->pipefd[info->cmd->index][WRITE_END], 1) < 0)
		ft_fprintf(2, "dup error on pipe %i WRITE END before exec first cmd %s\n", info->cmd->index, info->cmd->cmd_name);
	if (!info->files->here_doc)
	{
		if (!info->files->in_exist && !info->files->in_is_readbl)
			dup2(info->files->in_fd, 0);
		else
		{
			close(info->pipefd[info->cmd->index][WRITE_END]);
			if (info->files->in_exist)
				ft_fprintf(2, "pipex: no such file or directory: %s\n", info->files->infile);
			else if (info->files->in_is_readbl)
				ft_fprintf(2, "pipex: permission denied: %s\n", info->files->infile);
			exit(EXIT_FAILURE);
		}
	}
	else
	{
		//create pipe (+int fd[2])
		//dup write end for here_doc_fct
		//call here_doc with write end of pipe
	}
}

void	dup_last_cmd(t_info *info)
{
	close(info->pipefd[info->cmd->index - 1][WRITE_END]);
	if (info->cmd->index >= 2)
		close(info->pipefd[info->cmd->index - 2][READ_END]);
	dup2(info->pipefd[info->cmd->index - 1][READ_END], 0);
	if (!info->files->out_is_writbl)
		dup2(info->files->out_fd, 1);
	else
	{
		close(info->pipefd[info->cmd->index - 1][READ_END]);
		ft_fprintf(2, "pipex: permission denied: %s\n", info->files->outfile);
		info->exit_code = 1;
		exit(EXIT_FAILURE);
	}
}
