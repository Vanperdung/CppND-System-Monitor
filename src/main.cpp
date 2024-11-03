#include "ncurses_display.h"
#include "system.h"

int main(int argc, char* argv[])
{
	System system;
	NCursesDisplay::Display(system);
}