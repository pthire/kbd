//========================================================================
//
//========================================================================

#include <cstdlib>
#include "kbd_f.h"  // NOLINT(build/include)

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <termios.h>
#include <unistd.h>
#include <iostream>

//--------------------------------------------------------------------------

std::string GetLine () {

	std::string chaine;

	std::getline (std::cin, chaine);

	return chaine;
}

//--------------------------------------------------------------------------

std::string GetKey () {
	char* c_chaine;
	struct termios t;
	int len;

	c_chaine = (char *)malloc(1000);
	tcgetattr(0, &t);
	t.c_lflag &= ~(ECHO | ICANON);

	len = read (0, c_chaine, 1);

	t.c_lflag |= (ECHO | ICANON);

	if(len == -1) {
		c_chaine[0] = '\0';
	} else {
		c_chaine[len] = '\0';
	}

	std::string chaine(c_chaine);
	delete (c_chaine);
	return chaine;
}

//--------------------------------------------------------------------------

int SetEcho (int new_status) {

	struct termios t;

	tcgetattr(0, &t);
	if(new_status == 0)
	{
		t.c_lflag &= ~(ECHO);
		tcsetattr(0, TCSANOW, &t);
	}
	else
	{
		t.c_lflag |= ECHO;
		tcsetattr(0, TCSANOW, &t);
	}

	return  new_status;
}

//--------------------------------------------------------------------------

int SetCanonical (int new_status) {

	struct termios t;

	tcgetattr(0, &t);

	if(new_status == 0) {
		t.c_lflag &= ~(ICANON);
		tcsetattr(0, TCSANOW, &t);
	} else {
		t.c_lflag |= ICANON;
		tcsetattr(0, TCSANOW, &t);
	}

	return  new_status;
}
