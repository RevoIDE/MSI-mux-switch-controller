#ifndef SYS_IO_H
# define SYS_IO_H

# include <stddef.h>

int	os_path_available(const char *path);
int	run_command(const char *const argv[], int check);
int	io_file_read(const char *path, char **buf, const char *mode, size_t *max_len, size_t offset);

#endif
