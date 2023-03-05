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

/* transform char *PATH to t_list */
t_list	*path_to_llist(char **envp)
{
	t_list	*path;
	char	**tmp;
	int		i;

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
		i = -1;
		while (tmp[++i])
			ft_lstadd_back(&path, ft_lstnew(ft_strdup(tmp[i])));
		free_matrix(tmp);
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
	cmd->cmd_path = ft_strdup(cmd->cmd_name);
	cmd->exist = -1;
	cmd->is_exec = -1;
}

/* set cmd path + existence + execution rights. If '/' no search in PATH */
void	set_cmd_infos(t_cmd **start, t_list *path)
{
	t_cmd	*ptr;

	ptr = *start;
	while (ptr)
	{
		if (ft_strchr(ptr->cmd_name, '/'))
		{
			ptr->is_exec = access(ptr->cmd_name, X_OK);
			ptr->exist = access(ptr->cmd_name, F_OK);
			ptr->cmd_path = ft_strdup(ptr->cmd_name);
		}
		else
			path_finder(ptr, path);
		ptr = ptr->next;
	}
}

/* update cmdlist according to : 
if no infile or not readable : skip first command 
if no outfile : create one (and later execute last cmd)
if outfile no writable : skip lastcmd */
t_cmd	*first_last_cmd_vs_files(t_files files, t_cmd *cmd)
{
	t_cmd	*tmp;

	if (!files.here_doc && (files.in_exist || files.in_is_readbl))
	{
		if (files.in_exist)
			ft_fprintf(2, "zsh: no such file or directory: %s\n", files.infile);
		else if (files.in_is_readbl)
			ft_fprintf(2, "zsh: permission denied: %s\n", files.infile);
		tmp = cmd->next;
		cmd->next = NULL;
		cmd_lstclear(&cmd, free);
		cmd = tmp;
	}
	tmp = cmd;
	if (!files.out_exist && files.out_is_writbl)
	{
		while (tmp->next->next)
			tmp = tmp->next;
		cmd_lstclear(&tmp->next, free);
		tmp->next = NULL;
	}
	return (cmd);
}

/*	stores each cmd given into a t_cmd llist 
	get infos about the cmd path 
	erase (free then skip) first and last cmd if in/outfile pb*/
t_cmd	*cmd_parser(char **argv, t_files files, t_list *path)
{
	t_cmd	*start;
	t_cmd	*tmp;
	char	**cmd_spltd;

	argv++;
	if (files.here_doc)
		argv++;
	start = NULL;
	while (*(argv + 2))
	{
		cmd_spltd = ft_split(*(++argv), ' ');
		if (!cmd_spltd)
			return (NULL);
		tmp = cmd_lstnew(cmd_spltd);
		if (!tmp)
		{
			cmd_lstclear(&start, &free);
			return (NULL);
		}
		start = cmd_lstadd_back(&start, tmp);
	}
	set_cmd_infos(&start, path);
	start = first_last_cmd_vs_files(files, start);
	return (start);
}
