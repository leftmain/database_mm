#include "command.h"

int main(int argc, char ** argv) {
	Command c;
	char buf[LEN];

	while (fgets(buf, LEN, stdin) && c.parse(buf)) {
		c.print();
		c.clear();
	}
	c.print();

	return 0;
}
