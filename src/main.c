
#include "ec_controller.h"
#include "formatting.h"
#include "uefi.h"

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

const char *const MODES[4] =
{
	"hybrid",
	"discrete",
	"integrated",
	"unknown"
};

static const char	*yes_no(bool ok)
{
	return (ok ? GRN "✔ yes" RST : RED "✘ no" RST);
}

void	display_status(void)
{
	unsigned char	*uefi_raw = NULL;

	uefi_read_var(&uefi_raw);
	if (!uefi_raw)
		ERROR("UEFI var is empty !");

	t_status	s = uefi_decode((uint8_t) uefi_raw[OFF]);
	free(uefi_raw);

	printf("\n");
	printf(CYN "╭─" RST BOLD " GPU Status (UEFI)" RST "\n");
	printf(CYN "│" RST "\n");
	printf(CYN "│" RST "  Current mode     " DIM "▸" RST " " BOLD YEL "%s" RST "\n", MODES[s.current]);
	printf(CYN "│" RST "  After reboot     " DIM "▸" RST " " BOLD YEL "%s" RST "\n", MODES[s.next]);
	printf(CYN "│" RST "\n");
	printf(CYN "├─" RST BOLD " Supported modes" RST "\n");
	printf(CYN "│" RST "\n");
	printf(CYN "│" RST "  Switch           %s\n", yes_no(s.switch_ok));
	printf(CYN "│" RST "  Integrated       %s\n", yes_no(s.integrated_ok));
	printf(CYN "│" RST "  Discrete         %s\n", yes_no(s.discrete_ok));
	printf(CYN "╰─" RST "\n\n");
}

int	main(int argc, char *argv[])
{
	if (geteuid())
		ERROR("Should be run as root");

	switch (argc)
	{
		case 2:
			if (strcmp(argv[1], "status"))
				ERROR("Invalid command or missing arguments");
			display_status();
			break;
		default:
			display_status();
			break;
	}

	return (0);
}
