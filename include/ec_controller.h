#ifndef EC_CONTROLLER_H
# define EC_CONTROLLER_H

# define VAR			"/sys/firmware/efi/efivars/MsiDCVarData-dd96baaf-145e-4f56-b1cf-193256298e99"
# define EC_IO			"/sys/kernel/debug/ec/ec0/io"
# define OFF			4 + 5 // 4: efivarfs + 5: data
# define EC_ARM 		0xD1
# define BACKUP_DIR		"/root/msi-mux-backup"

extern const char *const MODES[4];

//int	read_var(char **raw);

#endif
