#include "uefi.h"
#include "ec_controller.h"
#include "formatting.h"
#include "sys_io.h"

#include <stdint.h>
#include <stdio.h>

void	uefi_read_var(unsigned char **dest)
{
	static const size_t	CAPACITY = 16;

	unsigned char *buffer;

    if (!os_path_available(VAR))
        ERROR("Unknown UEFI variable");

    buffer = malloc(CAPACITY);

    FILE *f = fopen(VAR, "rb");
    if (!f)
        ERROR("Unable to read UEFI variable");

    size_t	len = 0;

    while (1)
    {
	   	size_t	read = fread(buffer + len, 1, CAPACITY - len, f);
	   	len += read;

		if (read == 0)
		{
			if (ferror(f))
			{
				free(buffer);
				fclose(f);
				ERROR("Unable to read UEFI variable, ferror");
			}

			break;
		}
    }

    *dest = buffer;

    fclose(f);
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
