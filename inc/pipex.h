/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   pipex.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bguillau <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/02/25 18:41:25 by bguillau          #+#    #+#             */
/*   Updated: 2023/02/25 18:44:19 by bguillau         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef PIPEX_H
# define PIPEX_H

/* lib */
#include "../libft/libft.h"
#include <stdlib.h>
#include <unistd.h>

/* colors */
# define RED "\033[31m"
# define REDBOLD "\033[1;31m"
# define GREEN "\033[32m"
# define GREENBOLD "\033[1;32m"
# define END "\033[0m"

typedef	struct files
{
	char	*infile;
	char	*outfile;
	int		here_doc;
	char	*limiter;
}				t_files;

typedef	struct s_cmd
{
	char			*cmd_name;	// name (ls)								STACK
	char			*cmd_path;	// full path (/ur/bin/ls)					HEAP
	char			**cmd_argv;	// options + arg avec cmd_name en argv[0]	HEAP
	int				exist;		// F_OK										STACK
	int				is_exec;	// X_OK										STACK
	struct s_cmd	*next;
}					t_cmd;

/* aux */
void	print_path(t_list *path);
void	print_files(t_files files);
void	print_cmd_list(t_cmd *start);
void	free_matrix(char **matrix);

/* t_cmd fct */
t_cmd	*cmd_lstnew(char **cmd_spltd);
t_cmd	*cmd_lstadd_back(t_cmd **start, t_cmd *new);
void	cmd_lstclear(t_cmd **start, void (*del)(void *));

/* parsing */
t_list	*path_to_llist(char **envp);
t_files	file_parser(int argc, char **argv);
t_cmd	*cmd_parser(char **argv, int here_doc);
void	set_cmd_infos(t_cmd **start, t_list *path);

#endif
