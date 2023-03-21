/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   printers.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bguillau <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/03/04 12:45:42 by bguillau          #+#    #+#             */
/*   Updated: 2023/03/04 12:45:47 by bguillau         ###   ########.fr       */
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
	ft_fprintf(1, "----------------\n|FILES|\n");
	if (!files.here_doc)
	{
		ft_fprintf(1, "inf:%s\n", files.inf);
		ft_fprintf(1, "inf exist:%i\n", files.in_e);
		ft_fprintf(1, "inf is readbl:%i\n", files.in_r);
		ft_fprintf(1, "inf_fd:%i\n", files.in_fd);
	}
	else
	{
		ft_fprintf(1, "here_doc?:%i\n", files.here_doc);
		ft_fprintf(1, "lim:%s\n", files.lim);
		ft_fprintf(1, "inf_fd:%i\n", files.in_fd);
	}
	ft_fprintf(1, "out:%s\n", files.out);
	ft_fprintf(1, "out exist:%i\n", files.out_e);
	ft_fprintf(1, "out is writbl:%i\n", files.out_w);
	ft_fprintf(1, "out_fd:%i\n", files.out_fd);
}

void	print_cmd_list(t_cmd *start)
{
	int	i;

	while (start)
	{
		ft_fprintf(1, "---------------------\n|NEW COMMAND|\n");
		ft_fprintf(1, "name:%s\n", start->cmd_name);
		ft_fprintf(1, "index:%i\n", start->index);
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
	ft_fprintf(1, "---------------------\n");
}
