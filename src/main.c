
#include "ec_controller.h"
#include "formatting.h"
#include "uefi.h"
#include "gpu_switch.h"

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

static int	get_mode_id_by_name(const char *name)
{
	for (int i = 0; i < 3; i++)
	{
		if (strcmp(name, MODES[i]) == 0)
			return (i);
	}
	return (-1);
}

static void	print_usage(const char *name)
{
	printf("Usage (root):\n"
		"  %s status\n"
		"  %s set hybrid|discrete|integrated [--dry-run]\n"
		"  %s cancel\n"
		"  %s restore <backup_file.bin>\n",
		name, name, name, name);
}

int	main(int argc, char *argv[])
{
	int	mode_id;

	if (argc == 2 && (!strcmp(argv[1], "-h") || !strcmp(argv[1], "--help")))
	{
		print_usage(argv[0]);
		return (0);
	}

	if (geteuid())
		ERROR("Should be run as root");

	if (argc == 1 || (argc == 2 && !strcmp(argv[1], "status")))
		display_status();
	else if (!strcmp(argv[1], "set")
		&& (argc == 3 || (argc == 4 && !strcmp(argv[3], "--dry-run"))))
	{
		mode_id = get_mode_id_by_name(argv[2]);
		if (mode_id == -1)
			ERROR("Invalid mode (hybrid|discrete|integrated)");
		gpu_set_mode(mode_id, argc == 4);
	}
	else if (argc == 2 && !strcmp(argv[1], "cancel"))
		gpu_cancel();
	else if (argc == 3 && !strcmp(argv[1], "restore"))
		uefi_restore_var(argv[2]);
	else
		ERROR("Invalid command or missing arguments (see --help)");

	return (0);
}
