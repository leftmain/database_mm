#include "command.h"

int main() {
	Command c;
	char buf[LEN];
	double t = 0;

	t = clock();
	while (fgets(buf, LEN, stdin) && c.parse(buf)) {
		c.print();
		c.clear();
	}
	c.print();
	printf("\n");
	t = (clock() - t) / CLOCKS_PER_SEC;

	fprintf(stderr, "Time: %.2lf\n", t);

	return 0;
}
