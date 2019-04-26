#include "header.h"
#include "database.h"

//#define MAIN_DEBUG

int main(int argc, char ** argv) {
	Database<Record> d;
	const char * file = "a.txt";
	FILE * fp = nullptr;
	int res = 0;

	if (argc > 1) file = argv[1];

#ifdef MAIN_DEBUG
//	const char * fcmd = \
		"/Users/admin/Downloads/users_data/commands0.txt";
//	const char * fcmd = "cmd.txt";
	const char * fcmd = "/home/username/Downloads/prog_files/commands5.txt";
	const char * fres = "res.txt";
	FILE * fr = fopen(fcmd, "r");
	if (!fr) { perror("fr\n"); return 1; }
	FILE * fw = fopen(fres, "w");
	if (!fw) { perror("fw\n"); fclose(fr); return 1; }
#endif

	if (!(fp = fopen(file, "r"))) {
		fprintf(stderr, "Cannot open file %s\n", file);
#ifdef MAIN_DEBUG
		fclose(fr);
		fclose(fw);
#endif
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
		case DATA_ERR:
			fprintf(stderr, "Data error\n");
			break;
		default:
			fprintf(stderr, "Unknown error\n");
		}
		fclose(fp);
#ifdef MAIN_DEBUG
		fclose(fr);
		fclose(fw);
#endif
		return 2;
	}
	fclose(fp);
	
#ifdef MAIN_DEBUG
	d.start(fr, fw);
//	d.start(fr, stdout);
	fclose(fr);
	fclose(fw);
#else
	d.start();
#endif

	return 0;
}
