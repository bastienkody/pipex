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

/* handles infile outfile here_doc infos into t_files */
t_files	file_parser(int argc, char **argv)
{
	t_files	files;

	if (!ft_strncmp(argv[1], "here_doc", ft_strlen("here_doc")))
	{
		files.infile = NULL;
		files.here_doc = 1;
		files.limiter = argv[2];
	}
	else
	{
		files.infile = argv[1];
		files.here_doc = 0;
		files.limiter = NULL;
		files.in_exist = access(argv[1], F_OK);
		files.in_is_readbl = access(argv[1], R_OK);
	}
	files.outfile = argv[argc - 1];
	files.out_exist = access(argv[argc -1], F_OK);
	files.out_is_writbl = access(argv[argc -1], W_OK);
	return (files);
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

/* stores each cmd given into a t_cmd llist */
t_cmd	*cmd_parser(char **argv, int here_doc)
{
	t_cmd	*start;
	t_cmd	*tmp;
	char	**cmd_spltd;

	argv++;
	if (here_doc)
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
	return (start);
}
