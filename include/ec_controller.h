#ifndef EC_CONTROLLER_H
# define EC_CONTROLLER_H

# include <stdbool.h>
# include <stdint.h>
# include <sys/types.h>

# define VAR			"/sys/firmware/efi/efivars/MsiDCVarData-dd96baaf-145e-4f56-b1cf-193256298e99"
# define EC_IO			"/sys/kernel/debug/ec/ec0/io"
# define OFF			(4 + 5) // 4: efivarfs + 5: data
# define EC_ARM 		0xD1
# define BACKUP_DIR		"/root/msi-mux-backup"

# define EC_PATH "/sys/kernel/debug/ec"
# define WRITE_SUPPORT_PATH "/sys/module/ec_sys/parameters/write_support"

extern const char *const MODES[4];

typedef const char *CmdArgs[];

void	ec_ready(bool write);

int		ec_read(off_t	off);
void	ec_write(off_t off, uint8_t val);

#endif
