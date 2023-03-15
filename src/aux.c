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

void	close_n_free(t_list *path, t_cmd **cmd_list, t_files *files)
{
	close_files(files);
	ft_lstclear(&path, free);
	cmd_lstclear(cmd_list, free);
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

int	analyze_ex_code(int status)
{
	ft_fprintf(2, "exit status:%i\n", status);
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
