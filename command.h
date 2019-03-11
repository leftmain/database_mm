#ifndef COMMAND_H
#define COMMAND_H

#include "record.h"

enum cmd_type
{
	CMD_NONE,
	QUIT,
	STOP,
	SELECT,
	INSERT,
	DELETE
};

enum cond_type
{
	COND_NONE,
	EQ,
	NE,
	LT,
	GT,
	LE,
	GE,
	LIKE
};

enum op_type
{
	OP_NONE,
	AND,
	OR
};

class Command
{
public:
	cmd_type type = CMD_NONE;
	cond_type c_name = COND_NONE;
	cond_type c_phone = COND_NONE;
	cond_type c_group = COND_NONE;
	op_type oper = OP_NONE;

private:
	Command() : Record() {}
	~Command() {}

	int parse(const char * str) {
		return 0;
	}
	void print() {
	}
	int check(const Record& element) {
	}

};

#endif

