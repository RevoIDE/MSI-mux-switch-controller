#include "sys_io.h"
#include "formatting.h"

#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

int	os_path_available(const char *path)
{
	if (!path)
		return (-1);

	struct stat buffer;
	return (stat(path, &buffer) == 0);
}

int	run_command(const char *const argv[], int check)
{
	pid_t	pid = fork();
	int		status;

	if (pid < 0)
	{
		perror("fork");
		return (-1);
	}

	if (pid == 0)
	{
		execvp(argv[0], (char * const *) argv);

		perror(argv[0]);
		_exit(127);
	}

	if (waitpid(pid, &status, 0) < 0)
	{
		perror("waitpid");
		return (-1);
	}

	if (WIFEXITED(status))
	{
		int rc = WEXITSTATUS(status);

		if (check && rc != 0)
		{
			ERROR("Unable to run command !");
		}

		return rc;
	}

	if (WIFSIGNALED(status))
	{
		ERROR("Unable to run command !");

		if (check)
			exit(EXIT_FAILURE);

		return 128 + WTERMSIG(status);
	}

	return (-1);
}

int	io_file_read(const char *path, char **buf, const char *mode, size_t *max_len, size_t offset)
{
	FILE	*fp;
	size_t	len;

	*buf = NULL;
	fp = fopen(path, mode);

	if (!fp)
		return (-1);

	if (fseek(fp, offset, SEEK_SET) != 0)
	{
		fclose(fp);
		return (-1);
	}

	len = ftell(fp);

	if (max_len && len > *max_len)
		len = *max_len;

	if (len < 0)
	{
		fclose(fp);
		return (-1);
	}

	if (fseek(fp, 0, SEEK_SET) != 0)
	{
		fclose(fp);
		return (-1);
	}

	*buf = malloc(len + 1);
	if (!*buf)
	{
		fclose(fp);
		return (-1);
	}

	if (fread(*buf, 1, len, fp) != len)
	{
		fclose(fp);
		free(*buf);
		*buf = NULL;
		return (-1);
	}

	fclose(fp);
	(*buf)[len] = '\0';
	return (0);
}
