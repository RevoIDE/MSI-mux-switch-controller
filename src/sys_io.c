#include "sys_io.h"

#include <sys/stat.h>

int	os_path_available(const char *path)
{
	if (!path)
		return (-1);

	struct stat buffer;
	return (stat(path, &buffer) == 0);
}
