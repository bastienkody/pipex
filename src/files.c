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

void	launch_here_doc(t_files *files)
{
	char	*tmp;
	char	*data;

	data = ft_strdup("");
	tmp = ft_strdup("");
	while (ft_strncmp(tmp, files->limiter, ft_strlen(files->limiter)))
	{
		free(tmp);
		tmp = get_next_line(0);
		data = strj(data, tmp);
	}
	// write data into the pipe that will be received by first cmd
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
	if (!files->here_doc && files->out_exist)						 	// create outfile chmod 700 in >
		files->out_fd = open(files->outfile, O_CREAT|O_WRONLY, 00700);
	if (files->here_doc && files->out_exist)						 	// create outfile chmod 700 in >> (nsp si necessaire)
		files->out_fd = open(files->outfile, O_CREAT|O_APPEND|O_WRONLY, 00700);
	if (!files->out_exist && files->out_is_writbl)						// exist but cant write outfile
		files->out_fd = -2;
	if (!files->here_doc && !files->in_exist && !files->in_is_readbl)	//open infile
		files->in_fd = open(files->infile, O_RDONLY);
	if ((!files->here_doc && (files->in_exist || files->in_is_readbl))
		|| files->here_doc)												//cant read infile or no nedd bc of here_doc : fd = -2
		files->in_fd = -2;
	//if (files->here_doc)												// here_doc via gnl
		//launch_here_doc(files);
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
	open_files(&files);
	return (files);
}

void	close_files(t_files *files)
{
	if (files->in_fd > 0)
		close(files->in_fd);
	if (files->out_fd > 0)
		close(files->out_fd);
}
