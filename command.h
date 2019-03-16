#ifndef COMMAND_H
#define COMMAND_H

#include "record.h"

// fields
#define F_NAME  (1 << 0)
#define F_PHONE (1 << 1)
#define F_GROUP (1 << 2)
// order
#define ORDER_N_1 (1 << 0) // name first
#define ORDER_P_1 (1 << 1) // phone first
#define ORDER_G_1 (1 << 2) // group first
#define ORDER_N_2 (1 << 3) // name second
#define ORDER_P_2 (1 << 4) // phone second
#define ORDER_G_2 (1 << 5) // ..
#define ORDER_N_3 (1 << 6)
#define ORDER_P_3 (1 << 7)
#define ORDER_G_3 (1 << 8)

#define SPACE 		"( ),."
#define SPACE_END 	"( ),.;"


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

class Command : public Record
{
private:
	cmd_type type = CMD_NONE;
	cond_type c_name = COND_NONE;
	cond_type c_phone = COND_NONE;
	cond_type c_group = COND_NONE;
	op_type oper1 = OP_NONE;
	op_type oper2 = OP_NONE;
	int out_field = 0;
	int cond_order = 0;

public:
	Command() : Record() {}
	~Command() {}

	int get_next_word(const char ** c, char * buf) {
		*c += strspn(*c, SPACE);
		int i = strcspn(*c, SPACE);
		memcpy(buf, *c, i); buf[i] = 0; *c += i;
		if (**c == ';') { perror("syntax error"); return 1; }
		return 0;
	}
	int get_field(const char ** c, char * buf) {
		*c += strspn(*c, SPACE_END);
		int i = strcspn(*c, SPACE_END);
		memcpy(buf, *c, i); buf[i] = 0; *c += i;
		if (**c == '\n') { perror("syntax error"); return 1; }
		return 0;
	}
	int learn_field(char * buf, int * field) {
		if (!strcmp(buf, "name")) *field = F_NAME;
		else if (!strcmp(buf, "phone")) *field = F_PHONE;
		else if (!strcmp(buf, "group")) *field = F_GROUP;
		else return 1;
		return 0;
	}
	int set_cond(char * buf, int field) {
		cond_type c = COND_NONE;
		if (!strcmp(buf, "=")) c = EQ;
		else if (!strcmp(buf, "<>")) c = NE;
		else if (!strcmp(buf, "<")) c = LT;
		else if (!strcmp(buf, ">")) c = GT;
		else if (!strcmp(buf, "<=")) c = LE;
		else if (!strcmp(buf, ">=")) c = GE;
		else if (!strcmp(buf, "like")) c = LIKE;
		else return 1;
		if (field & F_NAME) c_name = c;
		else if (c == LIKE) return 1;
		else if (field & F_PHONE) c_phone = c;
		else if (field & F_GROUP) c_group = c;
		else return 1;
		return 0;
	}
	int set_field(char * buf, int field, int order) {
		order *= 3;
		switch (field) {
			case F_NAME:
				cond_order |= (ORDER_N_1 << order);
//				name = std::make_unique<char []>(strlen(buf) + 1);
				name = std::unique_ptr<char []>(new char[strlen(buf) + 1]);
				if (!name) {
					perror("memory error");
					return 1;
				}
				strcpy(name.get(), buf);
				break;
			case F_PHONE:
				cond_order |= (ORDER_P_1 << order);
				if ((phone = atoi(buf)) <= 0) {
					perror("syntax error");
					return 1;
				}
				break;
			case F_GROUP:
				cond_order |= (ORDER_G_1 << order);
				if ((group = atoi(buf)) <= 0) {
					perror("syntax error");
					return 1;
				}
				break;
		}
		return 0;
	}
	int parse_one_cond(const char ** c, char * buf, int order) {
		int curr_field = 0;
		if (get_next_word(c, buf)) return 1;
		if (learn_field(buf, &curr_field)) return 1;
		if (get_next_word(c, buf)) return 1;
		if (set_cond(buf, curr_field)) return 1;
		if (get_field(c, buf)) return 1;
		if (set_field(buf, curr_field, order)) return 1;
		return 0;
	}
	int parse_cond(const char * c, char * buf) {
		if (parse_one_cond(&c, buf, 0)) return 1;

		if (get_next_word(&c, buf)) return 0;
		if (!strcmp(buf, "and")) oper1 = AND;
		else if (!strcmp(buf, "or")) oper1 = OR;
		else return 0;

		if (parse_one_cond(&c, buf, 1)) return 1;

		if (get_next_word(&c, buf)) return 0;
		if (!strcmp(buf, "and")) oper2 = AND;
		else if (!strcmp(buf, "or")) oper2 = OR;
		else return 0;

		if (parse_one_cond(&c, buf, 2)) return 1;

		if (get_next_word(&c, buf)) return 0;

		return 0;
	}
	int parse(const char * str) {
		const char * c = str;
		char buf[LEN];
		int i = 0;
		if (get_field(&c, buf)) return 0;
		if (!strcmp(buf, "quit")) {
			type = QUIT;
			return 0;
		} else if (!strcmp(buf, "stop")) {
			type = STOP;
		} else if (!strcmp(buf, "select")) {
			type = SELECT;

			if (get_next_word(&c, buf)) return 0;
			if (*c == '*') out_field = F_NAME | F_PHONE | F_GROUP;
			if (get_next_word(&c, buf)) return 0;

			if (parse_cond(c, buf)) return 0;
		} else if (!strcmp(buf, "insert")) {
			type = INSERT;

			if (get_field(&c, buf)) return 0;
			if (set_field(buf, F_NAME, 0)) return 0;

			if (get_field(&c, buf)) return 0;
			if (set_field(buf, F_PHONE, 1)) return 0;

			if (get_field(&c, buf)) return 0;
			if (set_field(buf, F_GROUP, 2)) return 0;
		} else if (!strcmp(buf, "delete")) {
			type = DELETE;
			if (get_next_word(&c, buf)) return 0;
			if (strcmp(buf, "where")) {
				perror("wrong 'delete' input");
				return 0;
			}
			if (parse_cond(c, buf)) return 0;
		}
		return 1;
	}
	void print_c_cond(cond_type t) {
		switch (t) {
			case EQ: printf(" ="); return;
			case NE: printf(" <>"); return;
			case LT: printf(" <"); return;
			case GT: printf(" >"); return;
			case LE: printf(" <="); return;
			case GE: printf(" >="); return;
			case LIKE: printf(" like"); return;
			default: return;
		}
	}
	void print_one_cond(int field) {
		switch (field) {
			case F_NAME:
				printf(" name");
				print_c_cond(c_name);
				printf(" %s", name.get());
				break;
			case F_PHONE:
				printf(" phone");
				print_c_cond(c_phone);
				printf(" %d", phone);
				break;
			case F_GROUP:
				printf(" group");
				print_c_cond(c_group);
				printf(" %d", group);
				break;
		}
	}
	void print_cond() {
		if (cond_order & ORDER_N_1) print_one_cond(F_NAME);
		else if (cond_order & ORDER_P_1) print_one_cond(F_PHONE);
		else if (cond_order & ORDER_G_1) print_one_cond(F_GROUP);
		else return;

		if (oper1 == AND) printf(" and");
		else if (oper1 == OR) printf(" or");
		else return;

		if (cond_order & ORDER_N_2) print_one_cond(F_NAME);
		else if (cond_order & ORDER_P_2) print_one_cond(F_PHONE);
		else if (cond_order & ORDER_G_2) print_one_cond(F_GROUP);
		else return;

		if (oper2 == AND) printf(" and");
		else if (oper2 == OR) printf(" or");
		else return;

		if (cond_order & ORDER_N_3) print_one_cond(F_NAME);
		else if (cond_order & ORDER_P_3) print_one_cond(F_PHONE);
		else if (cond_order & ORDER_G_3) print_one_cond(F_GROUP);

		return;
	}
	void print() {
		switch (type) {
			case QUIT:
				printf("quit;\n\n");
				return;
			case STOP:
				printf("stop;\n");
				return;
			case SELECT:
				printf("select * where");
				print_cond();
				printf(";\n");
				return;
			case INSERT:
				printf("insert (%s, %d, %d);\n", name.get(), phone, group);
				return;
			case DELETE:
				printf("delete where");
				print_cond();
				printf(";\n");
				return;
			default:
				return;
		}
	}
	int check(const Record& element) {
		return 0;
	}
	cmd_type get_type(void) const { return type; }
	cond_type get_c_name(void) const { return c_name; }
	cond_type get_c_phone(void) const { return c_phone; }
	cond_type get_c_group(void) const { return c_group; }
	op_type get_oper(void) const { return oper1; }
	op_type get_oper1(void) const { return oper1; }
	op_type get_oper2(void) const { return oper2; }
	void clear() {
		type = CMD_NONE;
		c_name = COND_NONE;
		c_phone = COND_NONE;
		c_group = COND_NONE;
		oper1 = OP_NONE;
		oper2 = OP_NONE;
		cond_order = out_field = 0;
	}

};

#endif

