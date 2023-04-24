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

/*
 - si pas un '								--> on copie
 - si un ' et nb > 1						--> on decremente 
 - si un c et nb == 1 et nb initial impair  --> on copie le dernier '
*/
char	*dequote_single(char *str, int nb, int is_odd)
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
		else if (nb > 1)
			nb--;
		else if (is_odd)
			new[++j] = str[i];

	}
	new[++j] = '\0';
	return (new);
}

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
			if (count > 1)
				argv[i] = dequote_single(argv[i], count, count % 2);
			printf("%s : %i (post)\n", argv[i], counter(argv[i], 39));
			i++;
		}
	}
	return (0);
}
