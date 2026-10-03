# MSI mux switch controller

## Preliminary Note

This project was created to simplify the use of the MUX switch on Linux laptops. <br>
**UEFI status can be obtained in user mode; mode switching requires root privileges**

## Compatibility

| Laptops | Modes                        |
| ------- | ---------------------------- |
| Pulse   | hybrid, discrete, integrated |
| Alpha   | hybrid, discrete             |

## Available Commands

| Command | Description | Arguments |
| ------- | ----------- | --------- |
| --help  | Displays the help panel | — |
| status  | Shows the current GPU mode, the mode scheduled for next boot, firmware support flags and the EC arm register (0xD1) | — |
| set     | Schedules a GPU mode switch for next reboot (backs up the UEFI variable, writes it, then arms the EC) | `[mode]` : `hybrid` \| `discrete` \| `integrated`<br>`--dry-run` : prints the changes without writing anything |
| cancel  | Cancels a pending switch: resets the next-boot mode to the current one and disarms the EC | — |
| restore | Restores the UEFI variable from a backup file created by `set` or `cancel` | `[backup-name.bin]` : file from `/root/msi-mux-backup/` |
 
> **Note:** `restore` only rewrites the UEFI variable and does not touch the EC arm register (0xD1). To undo a pending switch, use `cancel` instead.


## TODO
| Description          | Status           |
| -------------------- | ---------------- |
| UEFI status reading: | done			  |
| UEFI Mode switching: | done             |
| EC Arm: 			   | done             |
| restore command:     | done             |

## Encountered Issues

If you encounter any issues with your firmware, follow this guide to reset your embedded controller: [Resetting the EC](https://www.msi.com/support/technical_details/NB_EC_RESET)

## AI note

AI contributions are discouraged (for security reasons).
The program writes directly to the UEFI and the EC, so any incorrect manipulation could cause serious issues.

## Reverse note

MSI laptops with a MUX switch expose MUX configuration through firmware interfaces:
- UEFI variable: `MsiDCVarData`
- Embedded Controller: register `EC[0xD1]`
