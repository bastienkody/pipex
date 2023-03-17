/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   files.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bguillau <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/03/05 19:20:15 by bguillau          #+#    #+#             */
/*   Updated: 2023/03/05 19:20:18 by bguillau         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../inc/pipex.h"

char	*launch_here_doc(t_files *files)	// int	write_end_of_pipe
{
	char	*line;
	char	*data;

	data = ft_strdup("");
	if (!data)
		return (NULL);
	while (1)
	{
		ft_fprintf(1, "here_doc > ");
		line = get_next_line(0);
		if (!line)
			return (free(data), NULL);
		if (!ft_strncmp(line, files->limiter, ft_strlen(files->limiter)))
		{
			if (ft_strlen(line) == ft_strlen(files->limiter) + 1)
				break ;
		}
		data = strj(data, line);
		if (!data)
			return (NULL);
	}
	ft_fprintf(1, "%s\n", data);
	// write(fd[WRITE_END], data, ft_strlen(data));
	return (data);
}

/*	open files according to:
	-here_doc (if here_doc : launch gnl + no infile + outfile = open 0_APPEND )
	-infile (dont exist or not readable : fd = -2)
	-infile (if exists + readble = open RD_ONLY)
	-outfile (if exists + writble = open W_ONLY | TRUNC)
	-outfile (dont exist : open 0_CREATE W_ONLY)
	-outfile (exists but no wrtible = nothing)*/
void	open_files(t_files *files)
{
	if (!files->here_doc && !files->out_exist && !files->out_is_writbl)	//open outfile in >
		files->out_fd = open(files->outfile, O_TRUNC|O_WRONLY);
	if (files->here_doc && !files->out_exist && !files->out_is_writbl)	//open outfile in >>
		files->out_fd = open(files->outfile, O_APPEND|O_WRONLY);
	if (files->out_exist)						 						// create outfile chmod 700 in >
		files->out_fd = open(files->outfile, O_CREAT|O_WRONLY, 00644);
	if (!files->out_exist && files->out_is_writbl)						// exist but cant write outfile
		files->out_fd = -2;
	if (!files->here_doc && !files->in_exist && !files->in_is_readbl)	//open infile
		files->in_fd = open(files->infile, O_RDONLY);
	if ((!files->here_doc && (files->in_exist || files->in_is_readbl))
		|| files->here_doc)												//cant read infile or no need bc of here_doc : fd = -2
		files->in_fd = -2;
	//if (files->here_doc)												// a lancer depuis exec_first_cmd ??
		//launch_here_doc(files);
}

/* handles infile outfile here_doc infos into t_files */
t_files	*file_parser(int argc, char **argv)
{
	t_files	*files;

	files = malloc(1 * sizeof(t_files));
	if (!files)
		return (NULL);
	if (!ft_strncmp(argv[1], "here_doc", ft_strlen("here_doc")))
	{
		files->infile = NULL;
		files->here_doc = 1;
		files->limiter = argv[2];
	}
	else
	{
		files->infile = argv[1];
		files->here_doc = 0;
		files->limiter = NULL;
		files->in_exist = access(argv[1], F_OK);
		files->in_is_readbl = access(argv[1], R_OK);
	}
	files->outfile = argv[argc - 1];
	files->out_exist = access(argv[argc -1], F_OK);
	files->out_is_writbl = access(argv[argc -1], W_OK);
	open_files(files);
	return (files);
}

void	close_files(t_files *files)
{
	if (files->in_fd > 0)
		close(files->in_fd);
	if (files->out_fd > 0)
		close(files->out_fd);
}

/* maybe need to diff malloc and pipe error */
int	**get_pipefd(t_info *info)
{
	int	i;
	int	**pipefd;

	pipefd = malloc((info->cmd_nb - 1) * sizeof(int *));
	if (!pipefd)
		return (NULL);
	i = 0;
	while (i < info->cmd_nb - 1)
	{
		pipefd[i] = malloc(2 * sizeof(int));
		if (!pipefd[i] || pipe(pipefd[i]) == -1)
		{
			perror("pipex");
			free_int_matrix(pipefd, i);
			return (NULL);
		}
		else
		{
			ft_fprintf(2, "pipe%i, read:%i\n", i, pipefd[i][0]);
			ft_fprintf(2, "pipe%i, write:%i\n", i, pipefd[i][1]);
		}
		i++;
	}
	return (pipefd);
}

void	close_pipefd(int cmd_nb, int **pipefd)
{
	int	i;

	if (!cmd_nb || !pipefd)
	{
		if (!cmd_nb)
			ft_fprintf(2, "cmd_nb = 0 --> no pipefd to close\n");
		if (!pipefd)
			ft_fprintf(2, "pipefd is NULL -> no pipefd to close\n");
		return ;
	}
	i = 0;
	while (i < cmd_nb - 1)
	{
		close(pipefd[i][WRITE_END]);
		close(pipefd[i][READ_END]);
		i++;
	}
}
