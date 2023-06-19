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

void	close_n_free(t_info *info)
{
	if (info->files)
	{
		close_files(info->files);
		if (info->files->here_doc)
			unlink("/tmp/.here_doc.txt");
		free(info->files);
	}
	if (info->path)
		ft_lstclear(&(info->path), &free);
	if (info->cmd_start)
		cmd_lstclear(&(info->cmd_start), &free);
	free(info);
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

int	analyze_ex_code(int status, t_info *info)
{
	if (info->files->out_w)
		return (1);
	info->cmd = cmd_lstlast(info->cmd_start);
	if (info->cmd->exist)
		return (127);
	if (info->cmd->is_exec)
		return (126);
	if (WIFEXITED(status))
		return (WEXITSTATUS(status));
	if (WIFSIGNALED(status))
		return (WTERMSIG(status));
	return (status);
}
