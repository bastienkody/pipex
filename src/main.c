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

void	cmd_exist_error(t_cmd *cmd)
{
	if (cmd->exist)
	{
		if (!ft_strchr(cmd->cmd_name, '/'))
			ft_fprintf(2, "pipex: command not found: %s\n", cmd->cmd_name);
		else
			ft_fprintf(2, "pipex: no such file or directory: %s\n", cmd->cmd_name);
		exit(EXIT_FAILURE);
	}
}

void	exec_mid_cmd(t_cmd *cmd, char **envp, t_files *files, int pipefd[])
{
	close(files->in_fd);
	if (cmd->index % 2)
	{
		dup2(pipefd[READ_ENDA], 0);
		dup2(pipefd[WRITE_ENDB], 1);
		close(pipefd[READ_ENDB]);
		close(pipefd[WRITE_ENDA]);
	}
	else
	{
		dup2(pipefd[READ_ENDB], 0);
		dup2(pipefd[WRITE_ENDA], 1);
		close(pipefd[READ_ENDA]);
		close(pipefd[WRITE_ENDB]);
	}
	cmd_exist_error(cmd);
	if (execve(cmd->cmd_path, cmd->cmd_argv, envp) == -1)
		perror("pipex");
}

void	exec_first_cmd(t_cmd *cmd, char **envp, t_files *files, int pipefd[])
{
	dup2(pipefd[WRITE_ENDA], 1);
	close(pipefd[READ_ENDA]);
	if (!files->here_doc)
	{
		if (!files->in_exist && !files->in_is_readbl)
			dup2(files->in_fd, 0);
		else
		{
			close(pipefd[WRITE_ENDA]);
			if (files->in_exist)
				ft_fprintf(2, "pipex: no such file or directory: %s\n", files->infile);
			else if (files->in_is_readbl)
				ft_fprintf(2, "pipex: permission denied: %s\n", files->infile);
			exit(EXIT_FAILURE);
		}
	}
	cmd_exist_error(cmd);
	if (execve(cmd->cmd_path, cmd->cmd_argv, envp) == -1)
		perror("pipex");
}

void	exec_last_cmd(t_cmd *cmd, char **envp, t_files *files, int pipefd[])
{
	if (cmd->index % 2)
	{
		dup2(pipefd[READ_ENDA], 0);
		close(pipefd[WRITE_ENDA]);
	}
	else
	{
		dup2(pipefd[READ_ENDB], 0);
		close(pipefd[WRITE_ENDB]);
	}
	if (!files->out_is_writbl)
		dup2(files->out_fd, 1);
	else
	{
		close(pipefd[READ_ENDA]);
		close(pipefd[READ_ENDB]);
		ft_fprintf(2, "pipex: permission denied: %s\n", files->outfile);
		exit(EXIT_FAILURE);
	}
	cmd_exist_error(cmd);
	if (execve(cmd->cmd_path, cmd->cmd_argv, envp) == -1)
		perror("pipex");
}

/* malloc pipefd + pipes eachs pipe, via ptr -1 or -2 for error
	maybe need to diff malloc and pipe error */
int	get_pipefd(int **pipefd, t_cmd *cmd)
{
	int	i;
	int	size;

	pipefd = malloc(1 * sizeof(int *));
	if (!pipefd)
		return (-1);
	i = 0;
	size = cmd_lstsize(cmd);
	while (i < size)
	{
		pipefd[i] = malloc(2 * sizeof(int));
		if (!pipefd[i] || pipe(pipefd[i] == -1)) // malloc error or pipe error
		{
			perror("pipex");
			free_int_matrix(pipefd, i);
			return (-2);
		}
		// ou pipe in here? si marche pas tres bien plus haut ou gestion error pas top
	}
	return (0);
}

int	pipex(t_cmd *cmd, char **envp, t_files files)
{
	pid_t	pid;
	int		child_status;
	int		**pipefd;

	pipefd = NULL;
	if (get_pipefd(pipefd, cmd))
		return (-1);
	while (cmd)
	{
		pid = fork();
		if (pid == -1)
			perror("fork");
		if (pid == 0)
		{
			if (!cmd->index)
				exec_first_cmd(cmd, envp, &files, pipefd);
			else if (!cmd->next)
				exec_last_cmd(cmd, envp, &files, pipefd);
			else
				exec_mid_cmd(cmd, envp, &files, pipefd);
		}
		cmd = cmd->next;
	}
	close(pipefd[1]);
	close(pipefd[0]);
	close(pipefd[2]);
	close(pipefd[3]);
	close_files(&files);
	while (waitpid(0, &child_status, 0) != -1)
		;
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
