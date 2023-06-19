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

void	wait_cmds(t_info *info)
{
	int		child_status;
	pid_t	ret;

	ret = 0;
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
	info->cmd_nb = 0;
	info->cmd_start = NULL;
	info->path = path_to_llist(envp, info);
	info->files = file_parser(argc, argv);
	if (!info->files)
		return (close_n_free(info), NULL);
	info->cmd = cmd_parser(argv, info);
	if (!info->cmd)
		return (close_n_free(info), NULL);
	info->cmd_start = info->cmd;
	info->cmd_nb = cmd_lstsize(info->cmd_start);
	info->exit_code = 0;
	return (info);
}

void	pipex(t_info *info, char **envp)
{
	int	prevpipe;

	//prevpipe = dup_infile(info);
	prevpipe = dup(0);

	while (info->cmd)
	{
		if (info->cmd->next)
			fork_pipe_n_dup(info->cmd, info, &prevpipe, envp);
		else
			fork_pipe_n_dup_lst_cmd(info->cmd, info, &prevpipe, envp);
		info->cmd = info->cmd->next;
	}
}

int	main(int argc, char **argv, char **envp)
{
	t_info	*info;
	int		exit_code;

	if (!arg_checker(argc, argv))
		return (3);
	argv = quote_space_parser(argc, argv);
	info = init_info(argc, argv, envp);
	if (!info)
		return (4);
	pipex(info, envp);
	exit_code = analyze_ex_code(info->exit_code, info);
	close_n_free(info);
	return (exit_code);
}
