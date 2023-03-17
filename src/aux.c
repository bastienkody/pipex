/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   aux.c                                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bguillau <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/02/26 15:03:50 by bguillau          #+#    #+#             */
/*   Updated: 2023/02/26 15:04:06 by bguillau         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../inc/pipex.h"

void	close_n_free(t_info *info) 
{
	if (info->files)
	{
		close_files(info->files);
		free(info->files);
	}
	if (info->cmd_nb && info->pipefd)
	{
		close_pipefd(info->cmd_nb, info->pipefd);
		free_int_matrix(info->pipefd, info->cmd_nb);
	}
	if (info->path)
		ft_lstclear(&(info->path), &free);
	if (info->cmd_start)
		cmd_lstclear(&(info->cmd_start), &free);
	free(info);
}

int	arg_checker(int argc, char **argv)
{
	if (argc < 5)
	{
		ft_fprintf(2, "%sProvide at least 4 arguments%s\n", BOLD, END);
		ft_fprintf(2, "%sUsage%s : ", UNDRLN, END);
		ft_fprintf(2, "\"infile\" \"cmd1\" \"cmd2\" ... ");
		ft_fprintf(2, "\"cmdn\" \"outfile\"\n");
		return (0);
	}
	if (!ft_strncmp(argv[1], "here_doc", ft_strlen("here_doc")) && argc < 6)
	{
		ft_fprintf(2, "%s5 arguments needed if here_doc%s\n", BOLD, END);
		ft_fprintf(2, "%sUsage%s : ", UNDRLN, END);
		ft_fprintf(2, "here_doc \"Limiter\" \"cmd1\" \"cmd2\" ... ");
		ft_fprintf(2, "\"cmdn\" \"outfile\"\n");
		return (0);
	}
	return (1);
}

int	analyze_ex_code(int status, t_info *info)
{
	ft_fprintf(2, "exit status:%i\n", status);
	if (info->files->out_is_writbl)
		return (1);
	info->cmd = cmd_lstlast(info->cmd_start);
	if (info->cmd->exist)
		return (127);
	if (!info->cmd->exist && status != 2)
		return (0);
	if (WIFEXITED(status))
	{
		ft_fprintf(2, "WIFEXITED(status):%i\n", WIFEXITED(status));
		ft_fprintf(2, "WEXITSTATUS(status):%i\n", WEXITSTATUS(status));
		return (WEXITSTATUS(status));
	}
	if (WIFSIGNALED(status))
	{
		ft_fprintf(2, "WIFSIGNALED(status):%i\n", WIFSIGNALED(status));
		ft_fprintf(2, "WTERMSIG(status):%i\n", WTERMSIG(status));
		return (WTERMSIG(status));
	}
	return (status);
}

void	free_char_matrix(char **matrix)
{
	int	i;

	i = 0;
	while (matrix[i])
		free(matrix[i++]);
	free(matrix);
}

void	free_int_matrix(int **matrix, int size)
{
	int	i;

	i = 0;
	while (i < size - 1)
		free(matrix[i++]);
	free(matrix);
}
