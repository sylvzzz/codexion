#include "codexion.h"

long	get_time_ms(void)
{
	struct timeval	tv;

	gettimeofday(&tv, NULL);
	return (tv.tv_sec * 1000 + tv.tv_usec / 1000);
}

size_t	ft_strlen(const char *s)
{
	int	i;

	i = 0;
	while (s[i])
	{
		i++;
	}
	return (i);
}


char	*ft_strchr(const char *s, int c)
{
	size_t	i;
	size_t	n;

	n = ft_strlen(s);
	i = 0;
	while (i <= n)
	{
		if ((unsigned char)s[i] == (unsigned char)c)
			return ((char *)&s[i]);
		i++;
	}
	return (NULL);
}


int	ft_atoi(const char *s, long *out)
{
	long	neg;
	long	result;

	neg = 1;
	result = 0;
	if (*s == '-' && ++neg)
		s++;
	if (*s == '+')
		s++;
	if (*s < '0' || *s > '9')
		return (1);
	while (*s >= '0' && *s <= '9')
	{
		result = result * 10 + (*s - '0');
		if (result < 0)
			return (1);
		s++;
	}
	if (*s != '\0')
		return (1);
	*out = result * neg;
	return (0);
}