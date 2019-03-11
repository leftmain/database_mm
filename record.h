#ifndef RECORD_H
#define RECORD_H

#include <memory>
#include <string.h>
#include <stdio.h>
#include "errors.h"

#define LEN 1024

classRecord 
{
private:
	std::unique_ptr<char []> name;
	int phone = 0;
	int group = 0;

	err_code init(const char * n = 0, int p = 0, int g = 0) {
		phone = p;
		group = g;
		if (n) {
//			name = std::unique_ptr<char []>(new char[strlen(n) + 1]);
			name = std::make_unique<char []>(strlen(n) + 1);
			if (!name) return MEM_ERR;
			strcpy(name.get(), n);
		} else name = 0;
		return ALL_RIGHT;
	}

public:
	Record(const Record& a) { init(a.name.get(), a.phone, a.group); }
	Record(Record&& a) { name = std::move(a.name); phone = a.phone; group = a.group; }
	Record(const char * n = nullptr, int p = 0, int g = 0) { init(n, p, g); }
	~Record() {}

	Record& operator=(const Record& rhs) {
		init(rhs.name.get(), rhs.p, rhs.g);
		return *this;
	}
	Record& operator=(Record&& rhs) {
		name = std::move(name);
		phone = rhs.phone;
		group = rhs.group;
		return *this;
	}
	void operator==(Record& rhs) {
		name = std::move(rhs.name);
		phone = rhs.phone;
		group = rhs.group;
	}
	int operator<(const Record& rhs) const {
/*
		if (name.get() && rhs.name.get()) {
			int res = strcmp(name.get(), rhs.name.get());
			if (res) return res;
			return phone - rhs.phone;
		}
		if (!name.get() && rhs.name.get()) return -1;
		if (name.get() && !rhs.name.get()) return 1;
		return phone - rhs.phone;
*/
		return strcmp(name, rhs.name);
	}
	err_code read(FILE * fp) {
		char buf[LEN];
		int p = 0, g = 0;
		if (fscanf(fp, "%s%d%d", buf, &p, &g) != 3) return CANNOT_READ;
		return init(buf, v);
	}
	void print(FILE * fp = stdout) const {
		fprintf(fp, "(%s %d %d)", name.get(), phone, group);
	}
	void printn(FILE * fp = stdout) const {
		fprintf(fp, "%s %d %d\n", name.get(), phone, group);
	}
	void swap(Record& a) {
		name.swap(a.name);
		int p = a.phone;
		phone = a.phone;
		a.phone = p;
		int g = a.group;
		group = a.group;
		a.group = g;
	}
	char * get_name() const { return name.get(); }
	int get_phone() const { return phone; }
	int get_group() const { return group; }

};

#endif

