#include "uefi.h"
#include "ec_controller.h"
#include "formatting.h"
#include "sys_io.h"

#include <errno.h>
#include <fcntl.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>

size_t	uefi_read_var(unsigned char **dest)
{
	size_t	capacity = 64;
	size_t	len = 0;
	size_t	bytes_read;

	unsigned char *buffer;

    if (!os_path_available(VAR))
        ERROR("Unknown UEFI variable");

    buffer = malloc(capacity);

    if (!buffer)
        ERROR("Unable to allocate memory for UEFI variable");

    FILE *f = fopen(VAR, "rb");
    if (!f)
        ERROR("Unable to read UEFI variable");

    while (1)
    {
        bytes_read = fread(buffer + len, 1, capacity - len, f);
        len += bytes_read;

        if (len >= capacity)
        {
            buffer = realloc(buffer, capacity *= 2);
            if (!buffer)
                ERROR("Unable to allocate memory for UEFI variable");
        }

        if (bytes_read == 0)
        {
            if (ferror(f))
                ERROR("Unable to read UEFI variable, ferror");

            break;
        }
    }

    *dest = buffer;

    fclose(f);
    return (len);
}

int	uefi_write_var(unsigned char *raw, size_t len)
{
	int				fd;
	ssize_t			bytes_written;
	size_t			clen;
	int				ok;
	unsigned char	*check;

	run_command((CmdArgs){"chattr", "-i", VAR, NULL}, false);

	fd = open(VAR, O_WRONLY);
	if (fd == -1)
		ERROR("Unable to open UEFI variable for writing");

	bytes_written = write(fd, raw, len);

	close(fd);
	run_command((CmdArgs){"chattr", "+i", VAR, NULL}, false);

	if (bytes_written < 0)
		ERROR("Unable to write UEFI variable");

	if ((size_t) bytes_written != len)
		ERROR("Incomplete write to UEFI variable");

	clen = uefi_read_var(&check);
	ok = (clen == len && memcmp(check, raw, len) == 0);

	free(check);

	if (!ok)
		ERROR("UEFI write verification failed");

	return (0);
}

/**
 * @brief Decodes the UEFI status byte into a `t_status` struct.
 */
t_status	uefi_decode(uint8_t b)
{
	return (t_status)
	{
		.next 			= b & 0x03,
		.current 		= (b >> 2) & 0x03,
		.switch_ok 		= (b & 0x10) != 0,
		.integrated_ok 	= (b & 0x20) != 0,
		.discrete_ok 	= (b & 0x40) == 0
	};
}
