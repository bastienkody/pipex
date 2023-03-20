/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   aux2.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bguillau <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/03/20 16:53:49 by bguillau          #+#    #+#             */
/*   Updated: 2023/03/20 16:53:54 by bguillau         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../inc/pipex.h"

void	free_char_matrix(char **matrix)
{
	int	i;

	i = 0;
	while (matrix[i])
		free(matrix[i++]);
	free(matrix);
}

void	free_int_matrix(int **matrix, int size)
{
	int	i;

	i = 0;
	while (i < size - 1)
		free(matrix[i++]);
	free(matrix);
}

int	cmd_lstsize(t_cmd *start)
{
	int	i;

	i = 0;
	while (start)
	{
		i++;
		start = start->next;
	}
	return (i);
}
