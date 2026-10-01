#include "ec_controller.h"
#include "formatting.h"
#include "sys_io.h"
#include "utils.h"

#include <stdbool.h>
#include <stdlib.h>
#include <unistd.h>

bool __check_write(const char *path)
{
	char	*buf;

	io_file_read(path, &buf);
	bool	wr = (buf && *str_strip(buf) == 'Y');
	free(buf);

	return wr;
}

void ec_ready(bool write)
{
	if (!os_path_available(EC_PATH))
		run_command((CmdArgs){"mount", "-t", "debugfs", "none", "/sys/kernel/debug", NULL}, 0);
	bool	loaded = os_path_available(WRITE_SUPPORT_PATH);
	bool	wr = (loaded && __check_write(WRITE_SUPPORT_PATH));

	if (write && !wr)
	{
		if (loaded)
			run_command((CmdArgs){"modprob", "-r", "ec_sys"}, false);
		run_command((CmdArgs){"modprobe", "ec_sys", "write_support=1"}, true);
	}
	else if (!loaded)
		run_command((CmdArgs){"modprobe", "ec_sys"}, false);
	if (!os_path_available(EC_IO))
		ERROR("EC_SYS / DEBUGFS Unavailable");
}
