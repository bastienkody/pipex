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

void	launch_here_doc(t_files *files)	// int	write_end_of_pipe
{
	char	*line;
	char	*data;

	data = ft_strdup("");
	if (!data)
		return;
	while (1)
	{
		ft_fprintf(1, "here_doc > ");
		line = get_next_line(0);
		if (!line)
			return (free(data));
		if (!ft_strncmp(line, files->limiter, ft_strlen(files->limiter)))
		{
			if (ft_strlen(line) == ft_strlen(files->limiter) + 1)
				break ;
		}
		data = strj(data, line);
		if (!data)
			return;
	}
	//ft_fprintf(2, "%s\n", data);
	//ft_fprintf(2, "%i\n", write(files->in_fd, data, ft_strlen(data)));
	write(files->in_fd, data, ft_strlen(data));
	//ft_fprintf(files->in_fd, "%s", data);
	perror("printf data into heredoc");
	ft_fprintf(2, "%i\n", files->in_fd);
	free(data);
}

void	open_files(t_files *files)
{
	if (!files->here_doc && ((!files->out_exist && !files->out_is_writbl) || files->out_exist))	//open (or create) outfile in >
		files->out_fd = open(files->outfile, O_TRUNC|O_WRONLY|O_CREAT, 00644);
	if (files->here_doc && ((!files->out_exist && !files->out_is_writbl) || files->out_exist))	//open outfile in >>
	{
		files->out_fd = open(files->outfile, O_APPEND|O_WRONLY|O_CREAT, 00644);
		ft_fprintf(2, "outfd heredoc:%i\n", files->out_fd);
	}
	files->out_exist = access(files->outfile, F_OK);											//reset outfile infos
	files->out_is_writbl = access(files->outfile, W_OK);


	if (!files->here_doc && !files->in_exist && !files->in_is_readbl)	//open infile
		files->in_fd = open(files->infile, O_RDONLY);
	if (files->here_doc)
	{
		files->in_fd = open(".here_doc.txt", O_RDWR|O_CREAT, 00644);
		ft_fprintf(2, "here_doc fd:%i\n", files->in_fd);
		launch_here_doc(files);
	}
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
		files->in_exist = 0;
		files->in_is_readbl = 0;
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

