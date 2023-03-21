/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   t_cmd_utils.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bguillau <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/02/28 16:57:22 by bguillau          #+#    #+#             */
/*   Updated: 2023/02/28 16:57:37 by bguillau         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../inc/pipex.h"

/* llist fct for a t_cmd (stores info about each cmd given in pipex) */

t_cmd	*cmd_lstnew(char **cmd_spltd, int index)
{
	t_cmd	*new;

	if (!cmd_spltd)
		return (NULL);
	new = malloc(1 * sizeof(t_cmd));
	if (!new)
		return (free_char_matrix(cmd_spltd), NULL);
	new->cmd_argv = cmd_spltd;
	new->cmd_name = cmd_spltd[0];
	new->index = index;
	new->next = NULL;
	return (new);
}

t_cmd	*cmd_lstlast(t_cmd *start)
{
	if (!start)
		return (NULL);
	while (start->next)
		start = start->next;
	return (start);
}

t_cmd	*cmd_lstadd_back(t_cmd **start, t_cmd *new)
{
	t_cmd	*ptr;

	if (!start)
		return (NULL);
	if (!new)
		return (*start);
	if (start)
	{
		if (!*start)
		{
			*start = new;
			return (*start);
		}
		ptr = cmd_lstlast(*start);
		ptr->next = new;
	}
	return (*start);
}

void	cmd_lstdelone(t_cmd *cmd, void (*del)(void *))
{
	if (!del || !cmd)
		return ;
	free_char_matrix(cmd->cmd_argv);
	(*del)(cmd->cmd_path);
	free(cmd);
}

void	cmd_lstclear(t_cmd **start, void (*del)(void *))
{
	t_cmd	*tmp;

	if (!start || !*start)
		return ;
	while (*start)
	{
		tmp = (*start)->next;
		cmd_lstdelone(*start, del);
		*start = tmp;
	}
}
