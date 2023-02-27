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
}
