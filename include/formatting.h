#ifndef ERROR_HANDLING_H
# define ERROR_HANDLING_H

# include <stdlib.h>
# include <stdio.h>

/* ANSI colors */

#define RST   "\033[0m"
#define BOLD  "\033[1m"
#define DIM   "\033[2m"
#define RED   "\033[31m"
#define GRN   "\033[32m"
#define YEL   "\033[33m"
#define CYN   "\033[36m"

/* Messages */
# define ERROR(msg) \
    do { \
        fprintf(stderr, RED "Error: " RST "%s\n", msg); \
        exit(EXIT_FAILURE); \
    } while (0)

# define WARN(msg) \
    do { \
        fprintf(stderr, YEL "Warning: " RST "%s\n", msg); \
    } while (0)

# define INFO(msg) \
    do { \
        fprintf(stdout, CYN "Info: " RESET "%s\n", msg); \
    } while (0)

#endif
