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

void	print_path(t_list *path)
{
	while (path)
	{
		ft_fprintf(1, "%s\n", path->content);
		path = path->next;
	}
	ft_fprintf(1, "----------------\n");
}

void	print_files(t_files files)
{
	if (!files.here_doc)
		ft_fprintf(1, "infile:%s\n", files.infile);
	else
	{
		ft_fprintf(1, "here_doc?:%i\n", files.here_doc);
		ft_fprintf(1, "limiter:%s\n", files.limiter);
	}
	ft_fprintf(1, "outfile:%s\n", files.outfile);
	ft_fprintf(1, "----------------\n");
}

void	print_cmd_list(t_cmd *start)
{
	int	i;

	while (start)
	{
		ft_fprintf(1, "---------------------\nNEW COMMAND\n");
		ft_fprintf(1, "name:%s\n", start->cmd_name);
		i = 0;
		while (start->cmd_argv[i])
		{
			ft_fprintf(1, "cmd_argv:%s\n", start->cmd_argv[i]);
			i++;
		}
		ft_fprintf(1, "path:%s\n", start->cmd_path);
		ft_fprintf(1, "exist:%i\n", start->exist);
		ft_fprintf(1, "is exec:%i\n", start->is_exec);
		start = start->next;
	}
	ft_fprintf(1, "----------------\n");
}

void	free_matrix(char **matrix)
{
	int	i;

	i = 0;
	while (matrix[i])
		free(matrix[i++]);
	free(matrix);
}
