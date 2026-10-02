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
t_status	uefi_decode		(uint8_t b);

#endif
