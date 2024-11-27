#include "ncurses_display.h"
#include "system.h"

int main(int argc, char* argv[])
{
	(void)argc;
	(void)argv;
	System system;
	NCursesDisplay::Display(system);
}