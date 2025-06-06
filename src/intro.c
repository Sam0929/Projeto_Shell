#include <stdio.h>
#include <stdlib.h>
#include <unistd.h> // For usleep

// Define some colors (ANSI escape codes)
#define RESET   "\033[0m"
#define BOLD    "\033[1m"
#define DIM     "\033[2m"
#define BLACK   "\033[30m"      /* Black */
#define RED     "\033[31m"      /* Red */
#define GREEN   "\033[32m"      /* Green */
#define YELLOW  "\033[33m"      /* Yellow */
#define BLUE    "\033[34m"      /* Blue */
#define MAGENTA "\033[35m"      /* Magenta */
#define CYAN    "\033[36m"      /* Cyan */
#define WHITE   "\033[37m"      /* White */
#define BOLDBLACK   "\033[1m\033[30m"      /* Bold Black */
#define BOLDRED     "\033[1m\033[31m"      /* Bold Red */
#define BOLDGREEN   "\033[1m\033[32m"      /* Bold Green */
#define BOLDYELLOW  "\033[1m\033[33m"      /* Bold Yellow */
#define BOLDBLUE    "\033[1m\033[34m"      /* Bold Blue */
#define BOLDMAGENTA "\033[1m\033[35m"      /* Bold Magenta */
#define BOLDCYAN    "\033[1m\033[36m"      /* Bold Cyan */
#define BOLDWHITE   "\033[1m\033[37m"      /* Bold White */

// Function to clear the screen (cross-platformish)
void clear_screen() {
#ifdef _WIN32
    system("cls");
#else
    // POSIX
    printf("\033[2J\033[H"); // Clears screen and moves cursor to top-left
#endif
}

// Function to print with a slight delay for typing effect
void type_effect(const char *str, unsigned int delay_us) {
    for (int i = 0; str[i] != '\0'; i++) {
        putchar(str[i]);
        fflush(stdout); // Important to ensure char is printed immediately
        usleep(delay_us);
    }
}

void print_loading_bar(int width) {
    printf(DIM "[" RESET);
    for (int i = 0; i < width; ++i) {
        printf(BOLDGREEN "#" RESET);
        fflush(stdout);
        usleep(20000); // 20ms delay
    }
    printf(DIM "]" RESET);
    printf(BOLDGREEN " Done!" RESET "\n\n");
}

void exibir_intro_terminal() {
    clear_screen();
    // --- Welcome Message ---
    printf(BOLDWHITE "  Welcome to " BOLDMAGENTA "NEX" BOLDWHITE " Terminal" RESET "\n");
    usleep(300000);

    const char *slogan = BOLDYELLOW "  The Next Experience in Command Line." RESET "\n\n";
    type_effect(slogan, 50000); // 50ms per char
    usleep(300000);

    // --- Initialization/Loading ---
    printf(DIM "  Initializing systems..." RESET "\n");
    usleep(200000);
    print_loading_bar(30); // Width of the loading bar

    // --- System Info (Example) ---
    printf(BOLDCYAN "  NEX Core Version:" RESET BOLDWHITE " 1.0.0 Alpha" RESET "\n");
    printf(BOLDCYAN "  System Status:" RESET BOLDGREEN " All Systems Nominal" RESET "\n");
    usleep(500000);
    printf("\n");

    // --- Ready Prompt ---
    type_effect(BOLDWHITE "  Type '" BOLDGREEN "help" BOLDWHITE "' for a list of commands." RESET "\n", 30000);
    type_effect(BOLDWHITE "  Ready for your input." RESET "\n\n", 30000);
    usleep(1000000); // 1 sec delay before showing the actual prompt

    printf(BOLDMAGENTA "\n  NEX Terminal Initialized. Ready for commands.\n\n" RESET);

    fflush(stdout);

}