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
		ft_fprintf(1, "infile:%s\n", files.infile);
		ft_fprintf(1, "infile exist:%i\n", files.in_exist);
		ft_fprintf(1, "infile is readbl:%i\n", files.in_is_readbl);
	}
	else
	{
		ft_fprintf(1, "here_doc?:%i\n", files.here_doc);
		ft_fprintf(1, "limiter:%s\n", files.limiter);
	}
	ft_fprintf(1, "outfile:%s\n", files.outfile);
	ft_fprintf(1, "outfile exist:%i\n", files.out_exist);
	ft_fprintf(1, "outfile is writbl:%i\n", files.out_is_writbl);
}

void	print_cmd_list(t_cmd *start)
{
	int	i;

	while (start)
	{
		ft_fprintf(1, "---------------------\n|NEW COMMAND|\n");
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
	ft_fprintf(1, "---------------------\n");
}

/* for pid_lst */
void	ft_lstprint(t_list *lst)
{
	while (lst)
	{
		ft_fprintf(1, "%d\n", *((pid_t *)(lst->content)));
		lst = lst->next;
	}
}
