/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parsing.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bguillau <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/02/26 14:38:45 by bguillau          #+#    #+#             */
/*   Updated: 2023/02/26 14:39:15 by bguillau         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../inc/pipex.h"

t_list	*make_path_list(char **tmp, t_info *info)
{
	int		i;
	char	*content;
	t_list	*new;
	t_list	*path;

	i = -1;
	path = NULL;
	while (tmp[++i])
	{
		content = ft_strdup(tmp[i]);
		new = ft_lstnew(content);
		if (!content || !new)
		{
			free(content);
			free(new);
			ft_lstclear(&path, &free);
			free_char_matrix(tmp);
			free(info);
			exit(-2);
		}
		ft_lstadd_back(&path, new);
	}
	free_char_matrix(tmp);
	return (path);
}

/* transform char *PATH to t_list */
t_list	*path_to_llist(char **envp, t_info *info)
{
	t_list	*path;
	char	**tmp;

	while (*envp)
	{
		if (!ft_strncmp("PATH", *envp, 4))
			break ;
		envp++;
	}
	path = NULL;
	if (*envp)
	{
		tmp = ft_split(ft_strchr(*envp, '=') + 1, ':');
		if (!tmp)
		{
			free(info);
			exit(-2);
		}
		path = make_path_list(tmp, info);
	}
	return (path);
}

/* search for cmd in the PATH */
void	path_finder(t_cmd *cmd, t_list *path)
{
	char	*tmp_path;
	char	*tmp_name;

	tmp_name = ft_strjoin("/", cmd->cmd_name);
	if (!tmp_name)
		return ;
	while (path)
	{
		tmp_path = ft_strjoin(path->content, tmp_name);
		if (!tmp_path)
			return (free(tmp_name));
		if (!access(tmp_path, F_OK))
		{
			cmd->cmd_path = tmp_path;
			cmd->exist = access(tmp_path, F_OK);
			cmd->is_exec = access(tmp_path, X_OK);
			return (free(tmp_name));
		}
		free(tmp_path);
		path = path->next;
	}
	free(tmp_name);
	cmd->cmd_path = ft_strdup(cmd->cmd_name);			// malloc protected later (if !ptr->cmd_path)
	cmd->exist = -1;
	cmd->is_exec = -1;
}

/* set cmd path + existence + execution rights. If '/' no search in PATH */
int	set_cmd_infos(t_cmd **start, t_list *path)
{
	t_cmd	*ptr;

	ptr = *start;
	while (ptr)
	{
		if (ft_strchr(ptr->cmd_name, '/'))
		{
			ptr->is_exec = access(ptr->cmd_name, X_OK);
			ptr->exist = access(ptr->cmd_name, F_OK);
			ptr->cmd_path = ft_strdup(ptr->cmd_name);		// malloc protected later (if !ptr->cmd_path)
		}
		else
			path_finder(ptr, path);
		if (!ptr->cmd_path)									// malloc of cmd_path=strdup protected here 
		{
			cmd_lstclear(start, &free);
			return (-2);
		}
		ptr = ptr->next;
	}
	return (0);
}

/*	stores each cmd given into a t_cmd llist 
	get infos about the cmd path*/
t_cmd	*cmd_parser(char **argv, t_info *info)
{
	t_cmd	*start;
	t_cmd	*tmp;
	char	**cmd_spltd;
	int		index;

	argv++;
	if (info->files->here_doc)
		argv++;
	start = NULL;
	index = -1;
	while (*(argv + 2))
	{
		cmd_spltd = ft_split(*(++argv), ' ');
		if (!cmd_spltd)
			return (NULL);
		tmp = cmd_lstnew(cmd_spltd, ++index);
		if (!tmp)
		{
			cmd_lstclear(&start, &free);
			return (NULL);
		}
		start = cmd_lstadd_back(&start, tmp);
	}
	if (set_cmd_infos(&start, info->path) == -2)			// protection of the malloc of cmd_path
		return (NULL);
	return (start);
}
