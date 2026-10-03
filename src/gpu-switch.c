

#include "ec_controller.h"
#include "formatting.h"
#include "uefi.h"
#include "gpu_switch.h"

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

void	gpu_set_mode(int mode, bool dry_run)
{
	unsigned char	*dest;
	uint8_t			byte;
	t_status		status;
	size_t			len;
	char			msg[64];

	if (mode < 0 || mode > 2)
		ERROR("Unknown mode");

	len = uefi_read_var(&dest);
	byte = dest[OFF];

	status = uefi_decode(byte);

	if (!status.switch_ok)
		ERROR("Switch not OK");
	if (mode == 2 && !status.integrated_ok)
		ERROR("Integrated mode not supported by firmware");
	if (mode == 1 && !status.discrete_ok)
		ERROR("Discrete mode not supported by firmware");
	if (mode == status.current && status.next == status.current)
	{
		free(dest);
		INFO("Already in the requested mode");
		return;
	}

	uint8_t		new_byte = (uint8_t)((byte & ~0b11) | mode);
	ec_ready(!dry_run);
	int			ec_byte = ec_read(EC_ARM);

	if (ec_byte < 0)
		ERROR("Unable to read EC arm register");

	uint8_t		new_ec_byte = (uint8_t)((ec_byte & ~0b11) | 0b01);

	printf("UEFI byte 5 : 0x%02X -> 0x%02X\n", byte, new_byte);
	printf("EC[0x%02X]    : 0x%02X -> 0x%02X\n", EC_ARM, ec_byte, new_ec_byte);

	if (dry_run)
	{
		free(dest);
		INFO("Dry run: nothing was written");
		return;
	}

	uefi_backup_var(dest, len);
	dest[OFF] = new_byte;
	uefi_write_var(dest, len);
	ec_write(EC_ARM, new_ec_byte);

	free(dest);

	snprintf(msg, sizeof(msg), "Switching to %s mode on next reboot", MODES[mode]);
	INFO(msg);
}

void	gpu_cancel(void)
{
	unsigned char	*dest;
	t_status		status;
	size_t			len;
	int				ec_byte;
	char			msg[64];

	len = uefi_read_var(&dest);
	status = uefi_decode(dest[OFF]);

	ec_ready(true);
	ec_byte = ec_read(EC_ARM);

	uefi_backup_var(dest, len);
	dest[OFF] = (uint8_t)((dest[OFF] & ~0b11) | status.current);
	uefi_write_var(dest, len);
	free(dest);

	if (ec_byte < 0)
		ERROR("Unable to read EC arm register");
	ec_write(EC_ARM, (uint8_t)(ec_byte & ~0b11));

	snprintf(msg, sizeof(msg), "Switch cancelled, staying in %s mode", MODES[status.current]);
	INFO(msg);
}
