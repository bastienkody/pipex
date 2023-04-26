#include <stdio.h>

int	counter(char *str, char c)
{
	int	i;

	i = 0;
	while (*str)
	{
		if (*str == c)
			i++;
		str++;
	}
	return (i);
}

char *space_to_minus(char *str)
{
	int		in_quote;
	char	space;
	char	quote;

	in_quote = 0;
	space = 32;
	quote = 39;
	while(*str)
	{
		if (!in_quote && *str == quote)
			in_quote = 1;
		else if (in_quote && *str == quote)
			in_quote = 0;
		else if (!in_quote && *str == space)
			*str = -1;
		str++;
	}
	return (str);
}

/*
 - si pas un '					--> on copie
 - si un ' 						--> on decremente 
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
		if (str[i] != c)
			new[++j] = str[i];
	}
	new[++j] = '\0';
	return (new);
}

/* 
	- quit if odd number of single quotes
	- replace <space> not between single quote by char -1
	- supress single quotes
	- ft_split on -1 later (parsing.c)
*/
int	main(int argc, char **argv)
{
	int	i;
	int count;

	i = 0;
	if (argc)
	{
		while (argv[i])
		{
			count = counter(argv[i], 39);
			printf("%s : %i\n", argv[i], count);
			if (count & 1)
				printf("Single quote error : odd number && EXIT_FAILURE\n");
			space_to_minus(argv[1]);
			if (count)
				argv[i] = dequote_single(argv[i]);
			printf("%s : %i (post)\n", argv[i], counter(argv[i], 39));
			i++;
		}
	}
	return (0);
}
