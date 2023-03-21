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
	char	*line;
	char	*data;

	data = ft_strdup("");
	if (!data)
		return ;
	while (1)
	{
		ft_fprintf(1, "here_doc > ");
		line = get_next_line(0);
		if (!line)
			return (free(data));
		if (!ft_strncmp(line, files->lim, ft_strlen(files->lim)))
		{
			if (ft_strlen(line) == ft_strlen(files->lim) + 1)
				break ;
		}
		data = strj(data, line);
		if (!data)
			return ;
	}
	write(files->in_fd, data, ft_strlen(data));
	free(line);
	free(data);
}

/*	open (or create) out in > 
	open (or create) out in >>
	reset out infos
	open inf
	fulfill here_doc n close + make it inf n open */
void	open_files(t_files *files)
{
	if (!files->here_doc && ((!files->out_e && !files->out_w) || files->out_e))
		files->out_fd = open(files->out, O_TRUNC | O_WRONLY | O_CREAT, 00644);
	if (files->here_doc && ((!files->out_e && !files->out_w) || files->out_e))
		files->out_fd = open(files->out, O_APPEND | O_WRONLY | O_CREAT, 00644);
	files->out_e = access(files->out, F_OK);
	files->out_w = access(files->out, W_OK);
	if (!files->here_doc && !files->in_e && !files->in_r)
		files->in_fd = open(files->inf, O_RDONLY);
	if (files->here_doc)
	{
		files->in_fd = open("/tmp/.here_doc.txt", O_WRONLY | O_CREAT, 00644);
		launch_here_doc(files);
		close(files->in_fd);
		files->in_fd = open("/tmp/.here_doc.txt", O_RDONLY);
	}
}

/* handles inf out here_doc infos into t_files */
t_files	*file_parser(int argc, char **argv)
{
	t_files	*files;

	files = malloc(1 * sizeof(t_files));
	if (!files)
		return (NULL);
	if (!ft_strncmp(argv[1], "here_doc", ft_strlen("here_doc")))
	{
		files->inf = NULL;
		files->here_doc = 1;
		files->lim = argv[2];
		files->in_e = 0;
		files->in_r = 0;
	}
	else
	{
		files->inf = argv[1];
		files->here_doc = 0;
		files->in_e = access(argv[1], F_OK);
		files->in_r = access(argv[1], R_OK);
	}
	files->out = argv[argc - 1];
	files->out_e = access(argv[argc -1], F_OK);
	files->out_w = access(argv[argc -1], W_OK);
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
