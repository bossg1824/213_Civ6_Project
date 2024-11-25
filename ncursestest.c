//clang ncursestest.c -o ncursestest -lncurses

#include <ncurses.h>
int main() {
    initscr();               // Initialize ncurses
    start_color();           // Initialize color functionality
    raw();                   // Disable line buffering
    noecho();                // Don't echo user input
    curs_set(0);             // Hide the cursor

    // Step 2: Define color pairs
    init_pair(1, COLOR_RED, COLOR_BLACK);   // Red text on black background
    init_pair(2, COLOR_GREEN, COLOR_BLACK); // Green text on black background
    init_pair(3, COLOR_BLUE, COLOR_BLUE);  // Blue text on white background

    // Step 3: Print some text using colors

    // Use color pair 1 (Red text on black background)
    char user_info[] = "Press a key to change map display";
    //size_t length = strlen(user_info);
    mvprintw(0,10,user_info);

    attron(COLOR_PAIR(1));    // Turn on color pair 1
    mvprintw(1, 0, "0");
    attroff(COLOR_PAIR(1));   // Turn off color pair 1

    // Use color pair 2 (Green text on black background)
    attron(COLOR_PAIR(2));    // Turn on color pair 2
    mvprintw(2, 1, "1");
    attroff(COLOR_PAIR(2));   // Turn off color pair 2

    // Use color pair 3 (Blue text on white background)
    attron(COLOR_PAIR(3));    // Turn on color pair 3
    mvprintw(3, 2, "2");
    attroff(COLOR_PAIR(3));   // Turn off color pair 3

    // Step 4: Refresh the screen to show the changes
    refresh();                // Update the screen with the printed text

    // Wait for user input
    getch();                  // Wait for the user to press a key

    clear();

     attron(COLOR_PAIR(3));    // Turn on color pair 1
    mvprintw(0, 2, "0");
    attroff(COLOR_PAIR(3));   // Turn off color pair 1

    // Use color pair 2 (Green text on black background)
    attron(COLOR_PAIR(3));    // Turn on color pair 2
    mvprintw(1, 0, "1");
    attroff(COLOR_PAIR(3));   // Turn off color pair 2

    // Use color pair 3 (Blue text on white background)
    attron(COLOR_PAIR(3));    // Turn on color pair 3
    mvprintw(2, 1, "2");
    attroff(COLOR_PAIR(3));   // Turn off color pair 3

    // Step 4: Refresh the screen to show the changes
    refresh();                // Update the screen with the printed text

    // Wait for user input
    getch();   

    // Step 5: End ncurses and restore the terminal
    endwin();                 // Clean up and close ncurses

    return 0;
}