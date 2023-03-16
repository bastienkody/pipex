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

/* maybe need to diff malloc and pipe error */
int	**get_pipefd(t_cmd *cmd)
{
	int	i;
	int	size;
	int	**pipefd;

	size = cmd_lstsize(cmd);
	pipefd = malloc((size - 1) * sizeof(int *));
	if (!pipefd)
		return (NULL);
	i = 0;
	while (i < size - 1)
	{
		pipefd[i] = malloc(2 * sizeof(int));
		if (!pipefd[i] || pipe(pipefd[i]) == -1)
		{
			perror("pipex");
			free_int_matrix(pipefd, i);
			return (NULL);
		}
		else
		{
			ft_fprintf(2, "pipe%i, read:%i\n", i, pipefd[i][0]);
			ft_fprintf(2, "pipe%i, write:%i\n", i, pipefd[i][1]);
		}
		i++;
	}
	return (pipefd);
}

int	pipex(t_cmd *cmd, char **envp, t_files files)
{
	pid_t	pid;
	int		child_status;
	int		**pipefd;
	int		lst_cmd_index;;

	lst_cmd_index = cmd_lstsize(cmd) - 1;
	pipefd = get_pipefd(cmd);
	if (!pipefd)
		return (-1);
	while (cmd)
	{
		pid = fork();
		if (pid == -1)
			perror("fork");
		if (pid == 0)
		{
			if (cmd->index == 0)
				exec_first_cmd(cmd, envp, &files, pipefd);
			else if (cmd->next)
				exec_mid_cmd(cmd, envp, &files, pipefd);
			else
				exec_last_cmd(cmd, envp, &files, pipefd);
		}
		if (pid > 0)
		{
			if (cmd->index == 1)
				close(pipefd[cmd->index - 1][WRITE_END]);
			else if (cmd->index != 0)
			{
				close(pipefd[cmd->index - 2][READ_END]);
				close(pipefd[cmd->index - 1][WRITE_END]);
			}
		}
		cmd = cmd->next;
	}
	close(pipefd[lst_cmd_index - 1][READ_END]);
	waitpid(-1, &child_status, 0);
	/* instead of while waitpid ... != 1 ; */
	free_int_matrix(pipefd, lst_cmd_index + 1);
	return (child_status);
}

int	main(int argc, char **argv, char **envp)
{
	t_list	*path;
	t_files	files;
	t_cmd	*cmd_list;
	int		ex_code;

	if (!arg_checker(argc, argv))
		return (1);
	files = file_parser(argc, argv);				// no malloc but open can have failed
	path = path_to_llist(envp);						// malloc error handled with exit_failure (+free what has been malloced)
	cmd_list = cmd_parser(argv, files, path);
	if (!cmd_list)
	{
		close_n_free(path, &cmd_list, &files);
		return (-1);
	}
	ex_code = pipex(cmd_list, envp, files);
	close_n_free(path, &cmd_list, &files);
	return (analyze_ex_code(ex_code));
}
