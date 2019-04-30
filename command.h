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

static int string_like(const char *, const char *);

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

static char buf[LEN];

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

	int get_next_word(const char **, char *);
	int get_field(const char **, char *);
	int learn_field(char *, int *);
	int set_cond(char *, int);
	int set_field(char *, int, int);
	int parse_one_cond(const char **, char *, int);
	int parse_cond(const char *, char *);
	void print_c_cond(cond_type);
	void print_one_cond(int);
	void print_cond();

public:
	Command() : Record() {}
	~Command() {}

	int parse(const char *);
	void print(FILE * = stdout);
	void dprint(int = STDOUT_FILENO);
	void clear();
	int check(const Record&);

	Record * get_record(void) { return (Record *)this; }
	cmd_type get_type(void) const { return type; }
	cond_type get_c_name(void) const { return c_name; }
	cond_type get_c_phone(void) const { return c_phone; }
	cond_type get_c_group(void) const { return c_group; }
	op_type get_oper(void) const { return oper1; }
	op_type get_oper1(void) const { return oper1; }
	op_type get_oper2(void) const { return oper2; }

};

int Command::get_next_word(const char ** c, char * buf) {
	*c += strspn(*c, SPACE);
	int i = strcspn(*c, SPACE);
	memcpy(buf, *c, i); buf[i] = 0; *c += i;
	if (**c == ';') { perror("syntax error"); return 1; }
	return 0;
}

int Command::get_field(const char ** c, char * buf) {
	*c += strspn(*c, SPACE_END);
	int i = strcspn(*c, SPACE_END);
	memcpy(buf, *c, i); buf[i] = 0; *c += i;
	if (**c == '\n') { perror("syntax error"); return 1; }
	return 0;
}

int Command::learn_field(char * buf, int * field) {
	if (!strcmp(buf, "name")) *field = F_NAME;
	else if (!strcmp(buf, "phone")) *field = F_PHONE;
	else if (!strcmp(buf, "group")) *field = F_GROUP;
	else return 1;
	return 0;
}

int Command::set_cond(char * buf, int field) {
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

int Command::set_field(char * buf, int field, int order) {
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
		default:
			perror("unknown field");
	}
	return 0;
}

int Command::parse_one_cond(const char ** c, char * buf, int order) {
	int curr_field = 0;
	if (get_next_word(c, buf)) return 1;
	if (learn_field(buf, &curr_field)) return 1;
	if (get_next_word(c, buf)) return 1;
	if (set_cond(buf, curr_field)) return 1;
	if (get_field(c, buf)) return 1;
	if (set_field(buf, curr_field, order)) return 1;
	return 0;
}

int Command::parse_cond(const char * c, char * buf) {
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

int Command::parse(const char * str) {
	const char * c = str;
	if (get_field(&c, buf)) return 0;
	if (!strcmp(buf, "quit")) {
		type = QUIT;
		return 0;
	} else if (!strcmp(buf, "stop")) {
		type = STOP;
		return 0;
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

void Command::print_c_cond(cond_type t) {
	switch (t) {
		case EQ: strcat(buf, " = "); return;
		case NE: strcat(buf, " <> "); return;
		case LT: strcat(buf, " < "); return;
		case GT: strcat(buf, " > "); return;
		case LE: strcat(buf, " <= "); return;
		case GE: strcat(buf, " >= "); return;
		case LIKE: strcat(buf, " like "); return;
		case COND_NONE: return;
//		default: perror("unknown cond");
	}
}
void Command::print_one_cond(int field) {
	switch (field) {
		case F_NAME:
			strcat(buf, " name");
			print_c_cond(c_name);
			strcat(buf, name.get());
			break;
		case F_PHONE:
			strcat(buf, " phone");
			print_c_cond(c_phone);
			sprintf(buf + strlen(buf), "%d", phone);
	//		itoa(phone, buf + strlen(buf), 10);
			break;
		case F_GROUP:
			strcat(buf, " group");
			print_c_cond(c_group);
			sprintf(buf + strlen(buf), "%d", group);
	//		itoa(group, buf + strlen(buf), 10);
			break;
		default:
			perror("unknown field");
	}
}

void Command::print_cond() {
	if (cond_order & ORDER_N_1) print_one_cond(F_NAME);
	else if (cond_order & ORDER_P_1) print_one_cond(F_PHONE);
	else if (cond_order & ORDER_G_1) print_one_cond(F_GROUP);
	else { perror("print_cond_1 error"); return; }

	if (oper1 == AND) strcat(buf, " and");
	else if (oper1 == OR) strcat(buf, " or");
	else return;

	if (cond_order & ORDER_N_2) print_one_cond(F_NAME);
	else if (cond_order & ORDER_P_2) print_one_cond(F_PHONE);
	else if (cond_order & ORDER_G_2) print_one_cond(F_GROUP);
	else { perror("print_cond_2 error"); return; }

	if (oper2 == AND) strcat(buf, " and");
	else if (oper2 == OR) strcat(buf, " or");
	else return;

	if (cond_order & ORDER_N_3) print_one_cond(F_NAME);
	else if (cond_order & ORDER_P_3) print_one_cond(F_PHONE);
	else if (cond_order & ORDER_G_3) print_one_cond(F_GROUP);
	else { perror("print_cond_3 error"); return; }
}

void Command::print(FILE * fp) {
	dprint(fileno(fp));
}

void Command::dprint(int fd) {
	memset(buf, 0, LEN);
	switch (type) {
		case QUIT:
			dprintf(fd, "quit;\n");
			return;
		case STOP:
			dprintf(fd, "stop;\n");
			return;
		case SELECT:
			strcat(buf, "select * where");
			print_cond();
			dprintf(fd, "%s;\n", buf);
			return;
		case INSERT:
			dprintf(fd, "insert (%s, %d, %d);\n", \
					name.get(), phone, group);
			return;
		case DELETE:
			strcat(buf, "delete where");
			print_cond();
			dprintf(fd, "%s;\n", buf);
			return;
		case CMD_NONE:
			return;
	}
}

int Command::check(const Record& el) {
	int n = 1, p = 1, g = 1;
	int c1 = 0, c2 = 0, c3 = 0;
	switch(c_name) {
		case EQ:
			if (strcmp(el.get_name(), name.get())) n = 0;
			break;
		case NE:
			if (!strcmp(el.get_name(), name.get())) n = 0;
			break;
		case LT:
			if (strcmp(el.get_name(), name.get()) >= 0) n = 0;
			break;
		case GT:
			if (strcmp(el.get_name(), name.get()) <= 0) n = 0;
			break;
		case LE:
			if (strcmp(el.get_name(), name.get()) > 0) n = 0;
			break;
		case GE:
			if (strcmp(el.get_name(), name.get()) < 0) n = 0;
			break;
		case LIKE:
			n = string_like(el.get_name(), name.get());
			break;
		case COND_NONE: n = 0;
	}
	switch(c_phone) {
		case EQ:
			if (el.get_phone() != phone) p = 0;
			break;
		case NE:
			if (el.get_phone() == phone) p = 0;
			break;
		case LT:
			if (el.get_phone() >= phone) p = 0;
			break;
		case GT:
			if (el.get_phone() <= phone) p = 0;
			break;
		case LE:
			if (el.get_phone() > phone) p = 0;
			break;
		case GE:
			if (el.get_phone() < phone) p = 0;
			break;
		case LIKE:
			if (el.get_phone() != phone) p = 0;
			break;
		case COND_NONE: p = 0;
	}
	switch(c_group) {
		case EQ:
			if (el.get_group() != group) g = 0;
			break;
		case NE:
			if (el.get_group() == group) g = 0;
			break;
		case LT:
			if (el.get_group() >= group) g = 0;
			break;
		case GT:
			if (el.get_group() <= group) g = 0;
			break;
		case LE:
			if (el.get_group() > group) g = 0;
			break;
		case GE:
			if (el.get_group() < group) g = 0;
			break;
		case LIKE:
			if (el.get_group() != group) g = 0;
			break;
		case COND_NONE: g = 0;
	}
	if (c_name == COND_NONE && c_phone == COND_NONE
							&& c_group == COND_NONE) return 1;
	if (cond_order & ORDER_N_1) c1 = n;
	else if (cond_order & ORDER_P_1) c1 = p;
	else if (cond_order & ORDER_G_1) c1 = g;
	if (cond_order & ORDER_N_2) c2 = n;
	else if (cond_order & ORDER_P_2) c2 = p;
	else if (cond_order & ORDER_G_2) c2 = g;
	if (cond_order & ORDER_N_3) c3 = n;
	else if (cond_order & ORDER_P_3) c3 = p;
	else if (cond_order & ORDER_G_3) c3 = g;
	switch (oper1) {
		case AND:
			switch (oper2) {
				case AND: return c1 * c2 * c3;
				case OR: return c1 * c2 + c3;
				case OP_NONE: return c1 * c2;
			}
		case OR:
			switch (oper2) {
				case AND: return c1 + c2 * c3;
				case OR: return c1 + c2 + c3;
				case OP_NONE: return c1 + c2;
			}
		case OP_NONE: return c1;
	}
	return c1 + c2 + c3;
}

static int string_like(const char * s, const char * x) {
	int i = 0;
	int l = 0;
	for (; *x && *s; x++, s++) {
		if (*x == '\\') {
			x++;
			if (*x != *s) return 0;
		} else if (*x != '_') {
			if (*x != '%') {
				if (*x != *s) return 0;
			} else {
				l = strlen(s);
				for (i = 0; i <= l; i++)
					if (string_like(s + i, x + 1)) return 1;
				if (i == l + 1) return 0;
			}
		}
	}
	if (*s) return 0;
	while (*x) {
		if (*x != '%') return 0;
		x++;
	}

	return 1;
}

void Command::clear() {
	type = CMD_NONE;
	c_name = COND_NONE;
	c_phone = COND_NONE;
	c_group = COND_NONE;
	oper1 = OP_NONE;
	oper2 = OP_NONE;
	cond_order = out_field = 0;
}

#endif

