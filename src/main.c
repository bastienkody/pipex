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
	if (execve(cmd->cmd_path, cmd->cmd_argv, envp) == -1)
	{
		//perror("pipex (at execution)");
		//exit(EXIT_FAILURE);
	}
	//exit(EXIT_SUCCESS);
}

void	exec_mid_cmd(t_cmd *cmd, char **envp, int **pipefd)
{
	ft_fprintf(2, "mid cmd %s %s (%i)\n", cmd->cmd_argv[0], cmd->cmd_argv[1], getpid());
	close(pipefd[cmd->index - 1][WRITE_END]);
	close(pipefd[cmd->index][READ_END]);
	dup2(pipefd[cmd->index - 1][READ_END], 0);
	dup2(pipefd[cmd->index][WRITE_END], 1);
		execute(cmd, envp);
}

void	exec_first_cmd(t_cmd *cmd, char **envp, t_files *files, int **pipefd)
{
	ft_fprintf(2, "first cmd %s %s (%i)\n", cmd->cmd_argv[0], cmd->cmd_argv[1], getpid());
	close(pipefd[cmd->index][READ_END]);
	if (dup2(pipefd[cmd->index][WRITE_END], 1) < 0)
		ft_fprintf(2, "dup error first cmd\n");
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
	execute(cmd, envp);
}

void	exec_last_cmd(t_cmd *cmd, char **envp, t_files *files, int **pipefd)
{
	ft_fprintf(2, "last cmd %s %s (%i)\n", cmd->cmd_argv[0], cmd->cmd_argv[1], getpid());
	close(pipefd[cmd->index - 1][WRITE_END]);
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

/* malloc pipefd + pipes eachs pipe,
	maybe need to diff malloc and pipe error */
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
		/*else
		{
			ft_fprintf(2, "pipe%i, read:%i\n", i, pipefd[i][0]);
			ft_fprintf(2, "pipe%i, write:%i\n", i, pipefd[i][1]);
		}*/
		i++;
	}
	return (pipefd);
}

/* each cmd reads in pipefd[index-1] and write in pipefd[index] ? */
int	pipex(t_cmd *cmd, char **envp, t_files files)
{
	pid_t	pid;
	int		child_status;
	int		**pipefd;
	int		lst_cmd_index;
	int		w_pid;

	lst_cmd_index = cmd_lstsize(cmd) - 1;
	pipefd = get_pipefd(cmd);
	if (!pipefd)
		return (-1);
	while (cmd)
	{
		pid = fork();
		if (pid == -1)
			perror("fork");
		if (pid > 0)
		{
			if (cmd->index == 1)
			{
				close(files.in_fd);
				close(pipefd[cmd->index - 1][WRITE_END]);
			}
			else if (cmd->index != 0)
			{
				close(pipefd[cmd->index - 2][READ_END]);
				close(pipefd[cmd->index - 1][WRITE_END]);
			}
		}
		if (pid == 0)
		{
			if (cmd->index == 0)
				exec_first_cmd(cmd, envp, &files, pipefd);
			else if (cmd->next)
				exec_mid_cmd(cmd, envp, pipefd);
			else
				exec_last_cmd(cmd, envp, &files, pipefd);
		}
		cmd = cmd->next;
	}
	close(files.out_fd);
	close(pipefd[lst_cmd_index - 1][READ_END]);
	while ((w_pid = waitpid(0, &child_status, WNOHANG)) != -1)
	{
		if (w_pid != 0)
			ft_fprintf(2, "during waitpid (%i finished)\n", w_pid);
	}
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
