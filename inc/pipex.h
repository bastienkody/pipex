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


/* aux */
void	print_path(t_list *path);

/* parsing */
t_list	*path_to_llist(char **envp);

#endif
