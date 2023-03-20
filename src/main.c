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

void	pipex(t_info *info, char **envp)
{
	pid_t	pid;
	int		child_status;
	pid_t	ret;

	while (info->cmd)
	{
		pid = fork();
		if (pid == -1)
			perror("fork");
		if (pid == 0)
			execute(info, envp);
		if (pid > 0)
		{
			if (!info->cmd->next)
				info->last_pid = pid;
			if (info->cmd->index == 1)
				close(info->pipefd[info->cmd->index - 1][WRITE_END]);
			else if (info->cmd->index != 0)
			{
				close(info->pipefd[info->cmd->index - 2][READ_END]);
				close(info->pipefd[info->cmd->index - 1][WRITE_END]);
			}
		}
		info->cmd = info->cmd->next;
	}
	close(info->pipefd[info->cmd_nb - 2][READ_END]);
	ret = waitpid(-1, &child_status, 0);
	while (ret != -1)
	{
		if (ret == info->last_pid)
			info->exit_code = child_status;
	ret = waitpid(-1, &child_status, 0);
	}
}

t_info	*init_info(int argc, char **argv, char **envp)
{
	t_info	*info;
 
	info = malloc(1 * sizeof(t_info));
	if (!info)
		return (NULL);
	info->path = path_to_llist(envp, info);				// path can be null(unset $PATH) malloc error handled with exit_failure (avec free info just before)
	info->files = file_parser(argc, argv);				// + opens can have failed
	if (!info->files)
	{
		close_n_free(info);
		return (NULL);
	}
	info->cmd = cmd_parser(argv, info);
	if (!info->cmd)
	{
		close_n_free(info);
		return (NULL);
	}
	info->cmd_start = info->cmd;
	info->cmd_nb = cmd_lstsize(info->cmd_start);
	info->exit_code = 0;
	return (info);
}

int	main(int argc, char **argv, char **envp)
{
	t_info	*info;
	int		exit_code;

	if (!arg_checker(argc, argv))
		return (-1);
	info = init_info(argc, argv, envp);
	if (!info)
		return (-2);
	info->pipefd = get_pipefd(info);
	if (!info->pipefd)
	{
		close_n_free(info);
		return (-3);
	}
	pipex(info, envp);
	exit_code = analyze_ex_code(info->exit_code, info);
	close_n_free(info);
	return (exit_code);
}
