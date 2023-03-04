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

void	free_n_quit(t_list *path, t_cmd **cmd_list)
{
	ft_lstclear(&path, free);
	cmd_lstclear(cmd_list, free);
}

int	arg_checker(int argc, char **argv)
{
	if (argc < 5)
	{
		ft_fprintf(2, "%sProvide at least 4 arguments%s\n", REDBOLD, END);
		ft_fprintf(2, "%sUsage%s : ", UNDRLN, END);
		ft_fprintf(2, "\"infile\" \"cmd1\" \"cmd2\" ... ");
		ft_fprintf(2, "\"cmdn\" \"outfile\"\n");
		return (0);
	}
	if (!ft_strncmp(argv[1], "here_doc", ft_strlen("here_doc")) && argc < 6)
	{
		ft_fprintf(2, "%s5 arguments needed if here_doc%s\n", REDBOLD, END);
		ft_fprintf(2, "%sUsage%s : ", UNDRLN, END);
		ft_fprintf(2, "here_doc \"Limiter\" \"cmd1\" \"cmd2\" ... ");
		ft_fprintf(2, "\"cmdn\" \"outfile\"\n");
		return (0);
	}
	return (1);
}

void	free_matrix(char **matrix)
{
	int	i;

	i = 0;
	while (matrix[i])
		free(matrix[i++]);
	free(matrix);
}
