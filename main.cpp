#include "command.h"
#include "list.h"

int main(int argc, char ** argv) {
	Command c;
	List<Record> l;
	char buf[LEN];
	const char * file = "a.txt";
	double t = 0;
	FILE * fp = nullptr;
	err_code res = ALL_RIGHT;
	int res_i = 0;

	if (argc > 1) file = argv[1];

	if (!(fp = fopen(file, "r"))) {
		fprintf(stderr, "Cannot open file %s\n", file);
		return 1;
	}

	if ((res = l.read(fp)) != ALL_RIGHT) {
		switch (res) {
		case CANNOT_OPEN:
			fprintf(stderr, "Cannot open file %s\n", file);
			break;
		case CANNOT_READ:
			fprintf(stderr, "Cannot read file %s\n", file);
			break;
		case MEM_ERR:
			fprintf(stderr, "Memory error\n");
			break;
		case EXIT: break;
		case ALL_RIGHT: break;
		}
		fclose(fp);
		return 2;
	}
	fclose(fp);
	
	l.print();

	t = clock();
	while (fgets(buf, LEN, stdin)) {
		res_i = c.parse(buf);
		if (res_i) l.apply_command(c);
		else if (c.get_type() == QUIT) break;
		c.clear();
	}
	printf("\n");
	t = (clock() - t) / CLOCKS_PER_SEC;

	fprintf(stderr, "Time: %.2lf\n", t);

	return 0;
}
