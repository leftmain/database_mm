#ifndef RECORD_H
#define RECORD_H

#include "header.h"

static char buff[LEN];

class Record 
{
protected:
	std::unique_ptr<char []> name;
	int phone = 0;
	int group = 0;

	int init(const char * = nullptr, int = 0, int = 0);

public:
	Record(const Record&);
	Record(Record&&);
	Record(const char * = nullptr, int = 0, int = 0);
	~Record() {}

	Record& operator=(const Record&);
	Record& operator=(Record&&);
	void operator==(Record&);
	int operator<(const Record&) const;
	int read(FILE *);
	void print(FILE * = stdout) const;
	void print(int) const;
	void draw(FILE * = stdout) const;
	void draw(int = STDOUT_FILENO) const;
	void swap(Record&);
	void move(Record&);

	char * get_name() const { return name ? name.get() : nullptr; }
	int get_phone() const { return phone; }
	int get_group() const { return group; }

};

Record::Record(const Record& a) {
	init(a.name.get(), a.phone, a.group);
}

Record::Record(Record&& a) {
	name = std::move(a.name);
	phone = a.phone;
	group = a.group;
}

Record::Record(const char * n, int p, int g) {
	init(n, p, g);
}

int Record::init(const char * n, int p, int g) {
	phone = p;
	group = g;
	if (n) {
		name = std::unique_ptr<char []>(new char[strlen(n) + 1]);
		if (!name) return MEM_ERR;
		strcpy(name.get(), n);
	} else name = 0;
	return ALL_RIGHT;
}

Record& Record::operator=(const Record& rhs) {
	init(rhs.name.get(), rhs.phone, rhs.group);
	return *this;
}

Record& Record::operator=(Record&& rhs) {
	name = std::move(name);
	phone = rhs.phone;
	group = rhs.group;
	return *this;
}

void Record::operator==(Record& rhs) {
	name = std::move(rhs.name);
	phone = rhs.phone;
	group = rhs.group;
}

int Record::operator<(const Record& rhs) const {
	if (name.get() && rhs.name.get()) {
		int res = strcmp(name.get(), rhs.name.get());
		if (res) return res;
		if (phone == rhs.phone) return group - rhs.group;
		return phone - rhs.phone;
	}
	if (!name.get() && rhs.name.get()) return -1;
	if (name.get() && !rhs.name.get()) return 1;
	if (phone == rhs.phone) return group - rhs.group;
	return phone - rhs.phone;
}

int Record::read(FILE * fp) {
	int p = 0, g = 0;
	if (fscanf(fp, "%s%d%d", buff, &p, &g) != 3) return CANNOT_READ;
	return init(buff, p, g);
}

void Record::print(FILE * fp) const {
	fprintf(fp, "%s\t%d\t%d\n", name.get(), phone, group);
}

void Record::print(int fd) const {
	sprintf(buff, "%s\t%d\t%d\n", name.get(), phone, group);
	int l = strlen(buff);
	write(fd, &l, sizeof(int));
	write(fd, buff, l);
}

void Record::draw(FILE * fp) const {
	fprintf(fp, "[%s %d %d] ", name.get(), phone, group);
}

void Record::draw(int fd) const {
	dprintf(fd, "[%s %d %d] ", name.get(), phone, group);
}

void Record::swap(Record& a) {
	name.swap(a.name);
	int p = a.phone;
	phone = a.phone;
	a.phone = p;
	int g = a.group;
	group = a.group;
	a.group = g;
}

void Record::move(Record& a) {
	name = std::move(a.name);
	phone = a.phone;
	group = a.group;
}

#endif

