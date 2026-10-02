# MSI mux switch controller

## Preliminary Note

This project was created to simplify the use of the MUX switch on Linux laptops. <br>
**UEFI status can be obtained in user mode; mode switching requires root privileges**

## Compatibility

| Laptops | Modes                        |
| ------- | ---------------------------- |
| Pulse   | hybrid, discrete, integrated |
| Alpha   | hybrid, discrete             |

## TODO
| Description          | Status           |
| -------------------- | ---------------- |
| UEFI status reading: | done			  |
| UEFI Mode switching: | work in progress |
| EC Arm: 			   | work in progress |

## AI note

AI contributions are discouraged (for security reasons).
The program writes directly to the UEFI and the EC, so any incorrect manipulation could cause serious issues.

## Reverse note

MSI laptops with a MUX switch expose MUX configuration through firmware interfaces:
- UEFI variable: `MsiDCVarData`
- Embedded Controller: register `EC[0xD1]`
