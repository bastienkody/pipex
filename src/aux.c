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

void	free_n_quit(t_list *path, t_cmd **cmd_list)
{
	ft_lstclear(&path, free);
	cmd_lstclear(cmd_list, free);
}

int	arg_checker(int argc, char **argv)
{
	if (argc < 5)
	{
		ft_fprintf(2, "%sProvide at least 4 arguments%s\n", BOLD, END);
		ft_fprintf(2, "%sUsage%s : ", UNDRLN, END);
		ft_fprintf(2, "\"infile\" \"cmd1\" \"cmd2\" ... ");
		ft_fprintf(2, "\"cmdn\" \"outfile\"\n");
		return (0);
	}
	if (!ft_strncmp(argv[1], "here_doc", ft_strlen("here_doc")) && argc < 6)
	{
		ft_fprintf(2, "%s5 arguments needed if here_doc%s\n", BOLD, END);
		ft_fprintf(2, "%sUsage%s : ", UNDRLN, END);
		ft_fprintf(2, "here_doc \"Limiter\" \"cmd1\" \"cmd2\" ... ");
		ft_fprintf(2, "\"cmdn\" \"outfile\"\n");
		return (0);
	}
	return (1);
}

void	free_matrix(char **matrix)
{
	int	i;

	i = 0;
	while (matrix[i])
		free(matrix[i++]);
	free(matrix);
}

void	exec_in_child_OG(t_cmd *cmd, char **envp, t_files *files, int pipefd[2])
{
	if (cmd->exist)														// cmd error : shell cmd vs exec
	{
		if (!ft_strchr(cmd->cmd_name, '/'))
			ft_fprintf(2, "pipex: command not found: %s\n", cmd->cmd_name);
		else
			ft_fprintf(2, "pipex: no such file or directory: %s\n", cmd->cmd_name);
		return ;
	}
	if (cmd->next)														// pas la derniere commande : stdout to pipe entry
		dup2(pipefd[1], 1);
	else if (!files->out_is_writbl)										// derniere cmd et outfile writbl (wether just created or existing)
		dup2(files->out_fd, 1);
	if (!files->out_exist && files->out_is_writbl && cmd->next == NULL)	// last cmd, outfile no_wrtbl : error printing + exit (no exec of lst cmd)
	{
			ft_fprintf(2, "pipex: permission deNied (outfile ; lst cmd wont be executted): %s for cmd %s\n", files->outfile, cmd->cmd_path);
			return ;
	}
	if (cmd->index == 0 && !files->in_is_readbl)						// first cmd infile ok : read stdin from infile
		dup2(files->in_fd, 0);
	if (cmd->index == 0 && files->in_is_readbl)							// first cmd infile no_redbl : 
		return ;
	if (cmd->index != 0)												// autres cmd : read stdin from pipe exit
		dup2(pipefd[0], 0);
	close(pipefd[1]);
	close(pipefd[0]);
	close_files(files);													// close infile n outfile
	if (execve(cmd->cmd_path, cmd->cmd_argv, envp) == -1) 				// handles args error (ie "ls : no such file or dir: /trucmuachinchose")
		perror("pipex");
}