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

#include "../libft/libft.h"
#include <stdlib.h>
#include <unistd.h>

typedef	struct files
{
	char	*infile;
	char	*outfile;
	int		here_doc;
	char	*limiter;
}				t_files;

typedef	struct s_cmd
{
	char	*cmd_name;	// name (ls)
	char	*cmd_path;	// full path (/ur/bin/ls)
	char	**cmd_argv;	// options + arg avec cmd_name en argv[0]
	int		exist;		// F_OK
	int		is_exec;	// X_OK
}				t_cmd;

/* aux */
void	print_path(t_list *path);
void	print_files(t_files files);

/* parsing */
t_list	*path_to_llist(char **envp);
t_files	file_parser(int argc, char **argv);
#endif
