/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bguillau <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/04/26 15:18:10 by bguillau          #+#    #+#             */
/*   Updated: 2023/04/26 15:18:40 by bguillau         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../inc/pipex.h"

/* nb of single quotes not escapped by \ */
int	counter(char *str, char c)
{
	int		i;

	i = 0;
	while (*str)
	{
		if (*str == c)
			i++;
		if (i > 0 && *str == c && *(str - 1) == '\\')
			i--;
		str++;
	}
	ft_fprintf(2, "counter=%i\n", i);
	return (i);
}

/*
	- bool pour savoir si on est entre single quote (==quoted)
	- si pas quoted, si \ et next <space>	--> skip \ et ' (str++ puis str++)
	- si pas quoted, si ' et prev != \ 		--> on replace par -1
	- ascii : 39=', 92=\
*/
char	*space_to_minus(char *str)
{
	int			in_quote;
	int			i;

	in_quote = 0;
	i = -1;
	while (str[++i])
	{
		if (!in_quote && str[i] == 39)
			in_quote = 1;
		else if (in_quote && str[i] == 39)
			in_quote = 0;
		else if (!in_quote && str[i] == 92 && str [i + 1] == ' ')
			i++;
		else if (!in_quote && str[i] == ' ')
			str[i] = -1;
	}
	//ft_fprintf(2, "spaceto-1:%s\n", str);
	return (str);
}

/*
 - si pas un ' ni un \			--> on copie le char
 - si \ et que next pas un '	--> on copie le \
 - si ' et que previous \		--> on copie le '
 - si un ' 						--> on skip
*/
char	*dequote_single(char *str)
{
	int			i;
	int			j;
	char		*new;
	static char	c = 39;

	i = -1;
	j = -1;
	new = str;
	while (str[++i])
	{
		if (str[i] != c && str[i] != '\\')
			new[++j] = str[i];
		else if (str[i] == '\\' && str[i + 1] != c)
			new[++j] = str[i];
		else if (i > 0 && str[i] == c && str[i - 1] == '\\')
			new[++j] = str[i];
		//ft_fprintf(2, "duringd:%s\n", new);
	}
	new[++j] = '\0';
	//ft_fprintf(2, "dequote:%s\n", new);
	//ft_fprintf(2, "--------------------\n", new);
	return (new);
}

/* 
	- exit failure + msg if odd number of single quotes
	- replace <space> not between single quote by char -1
	- supress single quotes
	- ft_split on -1 later (parsing.c)
*/
char	**quote_space_parser(int argc, char **argv)
{
	int	i;
	int	count;

	i = 0;
	if (argc > 1)
	{
		while (argv[i])
		{
			count = counter(argv[i], 39);
			if (count & 1)
			{
				ft_fprintf(2, "Quote error (odd number). Pipex exiting.\n");
				exit(EXIT_FAILURE);
			}
			argv[i] = space_to_minus(argv[i]);
			if (count)
				argv[i] = dequote_single(argv[i]);
			i++;
		}
	}
	return (argv);
}
