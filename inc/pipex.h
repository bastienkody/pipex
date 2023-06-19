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
# include "../libft/libft.h"
# include <stdlib.h>
# include <stdio.h>
# include <unistd.h>
# include <sys/types.h>
# include <sys/wait.h>
# include <fcntl.h>

/* colors */
# define RED "\033[31m"
# define REDBOLD "\033[1;31m"
# define GREEN "\033[32m"
# define GREENBOLD "\033[1;32m"
# define BOLD "\033[1m"
# define UNDRLN "\033[4m"
# define END "\033[0m"

/* fd */
# define STDIN 0
# define STDOUT 1
# define STDERR 2

/* pipes */
# define READ_END 0
# define WRITE_END 1

/* custom err msg */
# define NSFD "pipex: no such file or directory: "
# define PDND "pipex: permission denied: "
# define CNFD "pipex: command not found: "

typedef struct files
{
	char	*inf;
	int		in_e;
	int		in_r;
	int		in_fd;
	char	*out;
	int		out_e;
	int		out_w;
	int		out_fd;
	int		here_doc;
	char	*lim;
}				t_files;

typedef struct s_cmd
{
	int				index;
	char			*cmd_name;
	char			*cmd_path;
	char			**cmd_argv;
	int				exist;
	int				is_exec;
	struct s_cmd	*next;
}					t_cmd;

typedef struct s_info
{
	t_files	*files;
	t_list	*path;
	t_cmd	*cmd;
	t_cmd	*cmd_start;
	int		cmd_nb;
	pid_t	last_pid;
	int		exit_code;
}				t_info;

/* printers */
void	print_path(t_list *path);
void	print_files(t_files files);
void	print_cmd_list(t_cmd *start);
void	ft_lstprint(t_list *lst);

/* aux */
void	close_n_free(t_info *info);
int		arg_checker(int argc, char **argv);
int		analyze_ex_code(int status, t_info *info);
void	free_char_matrix(char **matrix);
void	free_int_matrix(int **matrix, int size);

/* t_cmd fct */
t_cmd	*cmd_lstnew(char **cmd_spltd, int index);
t_cmd	*cmd_lstlast(t_cmd *start);
t_cmd	*cmd_lstadd_back(t_cmd **start, t_cmd *new);
void	cmd_lstclear(t_cmd **start, void (*del)(void *));
int		cmd_lstsize(t_cmd *start);

/* cmd parsing */
t_list	*path_to_llist(char **envp, t_info *info);
t_cmd	*cmd_parser(char **argv, t_info *info);

/* quotes spaces parsing*/
char	**quote_space_parser(int argc, char **argv);

/* files */
t_files	*file_parser(int argc, char **argv);
void	close_files(t_files *files);

/* execution */
void	execute(t_cmd *cmd, t_info *info, char **envp);
void	wait_cmds(t_info *info);

/* fork_pipe_dup */
void	fork_pipe_n_dup(t_cmd *cmd, t_info *info, int *prevpipe, char **envp);
void	fork_pipe_n_dup_lst_cmd(t_cmd *cmd, t_info *info, int *prevpipe, char **envp);

/* duppers */
int	dupper(int old_fd, int new_fd, t_info *info);
int	dup_infile(t_info *info);
int	dup_outfile(t_info *info);

#endif
