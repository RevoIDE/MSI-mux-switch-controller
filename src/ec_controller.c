#include "ec_controller.h"
#include "formatting.h"
#include "sys_io.h"
#include "utils.h"

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>

#include <sys/types.h>

bool check_write__(const char *path)
{
	char	*buf;

	io_file_read(path, &buf, "r", NULL, 0);
	bool	wr = (buf && *str_strip(buf) == 'Y');
	free(buf);

	return wr;
}

void ec_ready(bool write)
{
	if (!os_path_available(EC_PATH))
		run_command((CmdArgs){"mount", "-t", "debugfs", "none", "/sys/kernel/debug", NULL}, 0);
	bool	loaded = os_path_available(WRITE_SUPPORT_PATH);
	bool	wr = (loaded && check_write__(WRITE_SUPPORT_PATH));

	if (write && !wr)
	{
		if (loaded)
			run_command((CmdArgs){"modprobe", "-r", "ec_sys", NULL}, false);
		run_command((CmdArgs){"modprobe", "ec_sys", "write_support=1", NULL}, true);
	}
	else if (!loaded)
		run_command((CmdArgs){"modprobe", "ec_sys", NULL}, false);
	if (!os_path_available(EC_IO))
		ERROR("EC_SYS / DEBUGFS Unavailable");
}

int		ec_read(off_t	off)
{
	uint8_t	val;
	int		fd;
	ssize_t	bytes_read;

	fd = open(EC_IO, O_RDONLY);
	if (fd < 0)
		return (-1);

	bytes_read = pread(fd, &val, 1, off);
	close(fd);

	return (bytes_read == 1) ? val : -1;
}

int		ec_write(off_t off, uint8_t val)
{
	int	fd = open(EC_IO, O_WRONLY);
	if (fd == -1)
		ERROR("Failed to open EC var file for writing");

	if (fd < 0)
		return (-1);

	if (pwrite(fd, &val, 1, off) != 1)
	{
		close(fd);
		ERROR("Failed to write to EC var file");
	}

	if (ec_read(off) != val)
		WARN("checked EC[0x%02X] != 0x%02X. The firmware may have changed it");

	close(fd);
	return (0);
}
