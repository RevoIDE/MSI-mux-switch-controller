#include "uefi.h"
#include "ec_controller.h"
#include "formatting.h"
#include "sys_io.h"

#include <errno.h>
#include <fcntl.h>
#include <limits.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>

static size_t	read_raw__(const char *path, unsigned char **dest)
{
	size_t	capacity = 64;
	size_t	len = 0;
	size_t	bytes_read;

	unsigned char *buffer;

    buffer = malloc(capacity);

    if (!buffer)
        ERROR("Unable to allocate memory for UEFI variable");

    FILE *f = fopen(path, "rb");
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

size_t	uefi_read_var(unsigned char **dest)
{
	if (!os_path_available(VAR))
		ERROR("Unknown UEFI variable");

	return (read_raw__(VAR, dest));
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
 * @brief Saves the raw UEFI variable (attributes + data) to
 * `BACKUP_DIR/MsiDCVarData_<YYYYmmdd-HHMMSS>.bin`.
 */
int	uefi_backup_var(const unsigned char *raw, size_t len)
{
	char		path[PATH_MAX];
	char		msg[PATH_MAX + 16];
	char		stamp[32];
	time_t		now;
	struct tm	tm;
	FILE		*f;

	if (mkdir(BACKUP_DIR, 0700) == -1 && errno != EEXIST)
		ERROR("Unable to create backup directory");

	now = time(NULL);
	if (!localtime_r(&now, &tm) || !strftime(stamp, sizeof(stamp), "%Y%m%d-%H%M%S", &tm))
		ERROR("Unable to build backup timestamp");

	if (snprintf(path, sizeof(path), BACKUP_DIR "/MsiDCVarData_%s.bin", stamp) >= (int) sizeof(path))
		ERROR("Backup path too long");

	f = fopen(path, "wb");
	if (!f)
		ERROR("Unable to create backup file");

	if (fwrite(raw, 1, len, f) != len)
	{
		fclose(f);
		ERROR("Incomplete write to backup file");
	}

	if (fclose(f) != 0)
		ERROR("Unable to close backup file");

	snprintf(msg, sizeof(msg), "Backup: %s", path);
	INFO(msg);

	return (0);
}

/**
 * @brief Restores the UEFI variable from a backup made by `uefi_backup_var`.
 */
int	uefi_restore_var(const char *path)
{
	unsigned char	*raw;
	unsigned char	*current;
	size_t			len;
	size_t			current_len;
	char			msg[PATH_MAX + 32];

	if (!os_path_available(path))
		ERROR("Unknown backup file");

	len = read_raw__(path, &raw);
	current_len = uefi_read_var(&current);
	free(current);

	if (len != current_len)
		ERROR("Backup size differs from current UEFI variable");

	uefi_write_var(raw, len);
	free(raw);

	snprintf(msg, sizeof(msg), "UEFI variable restored from %s", path);
	INFO(msg);

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
