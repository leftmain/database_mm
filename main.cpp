#include "header.h"
#include "database.h"

int main(int argc, char ** argv) {
	Database<Record> d;
	const char * file = "a.txt";
	FILE * fp = nullptr;
	int res = 0;

	if (argc > 1) file = argv[1];

	if (!(fp = fopen(file, "r"))) {
		fprintf(stderr, "Cannot open file %s\n", file);
		return 1;
	}

	if ((res = d.read(fp)) != ALL_RIGHT) {
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
		case ALL_RIGHT: break;
		}
		fclose(fp);
		return 2;
	}
	fclose(fp);
	
	d.start();

	return 0;
}
