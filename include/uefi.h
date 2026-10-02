#ifndef UEFI_H
# define UEFI_H

# include <stddef.h>
# include <stdint.h>
# include <stdbool.h>

typedef struct s_status
{
	int		next, current;
	bool 	switch_ok, integrated_ok, discrete_ok;
}	t_status;

size_t		uefi_read_var(unsigned char **dest);
int			uefi_write_var(unsigned char *raw, size_t len);
int			uefi_backup_var(const unsigned char *raw, size_t len);
int			uefi_restore_var(const char *path);
t_status	uefi_decode		(uint8_t b);

#endif
