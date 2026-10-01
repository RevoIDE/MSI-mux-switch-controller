

#include <ctype.h>
#include <string.h>

char	*str_strip(char *str)
{
	char *end;

	if (!str)
		return (NULL);

	end = str + strlen(str);
	while (end > str && isspace((unsigned char)end[-1]))
		--end;
	*end = '\0';
	return (str);
}
