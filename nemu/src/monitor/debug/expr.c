#include "nemu.h"

/* We use the POSIX regex functions to process regular expressions.
 * Type 'man regex' for more information about POSIX regex functions.
 */
#include <sys/types.h>
#include <regex.h>

enum {
	NOTYPE = 256, EQ,NEHENUM, HENUM, NENUM, NUM, REG,UNEQ, AND, OR, NOT, POINTER, VAR

	/* TODO: Add more token types */

};

static struct rule {
	char *regex;
	int token_type;
} rules[] = {

	/* TODO: Add more rules.
	 * Pay attention to the precedence level of different rules.
	 */
	{" +",	NOTYPE},
	{"-0[Xx][0-9a-fA-F][0-9a-fA-F]*", NEHENUM},
	{"0[Xx][0-9a-fA-F][0-9a-fA-F]*", HENUM},				// spaces
	{"-[0-9][0-9]*", NENUM},
	{"[0-9][0-9]*", NUM},
	{"$[a-z]{3}", REG},
	{"\\+", '+'},					// plus
	{"-", '-'},
	{"\\*", '*'},
	{"/", '/'},
	{"\\(", '('},
	{"\\)", ')'},
	{"==", EQ},						// equal
	{"!=", UNEQ},
	{"&&", AND},
	{"\\|\\|", OR},
	{"!", NOT},
	{"[_a-zA-Z][_a-zA-Z0-9]*", VAR}
};

#define NR_REGEX (sizeof(rules) / sizeof(rules[0]) )

static regex_t re[NR_REGEX];

/* Rules are used for many times.
 * Therefore we compile them only once before any usage.
 */
void init_regex() {
	int i;
	char error_msg[128];
	int ret;

	for(i = 0; i < NR_REGEX; i ++) {
		ret = regcomp(&re[i], rules[i].regex, REG_EXTENDED);
		if(ret != 0) {
			regerror(ret, &re[i], error_msg, 128);
			Assert(ret == 0, "regex compilation failed: %s\n%s", error_msg, rules[i].regex);
		}
	}
}

typedef struct token {
	int type;
	char str[32];
} Token;

Token tokens[32];
int nr_token;

static bool make_token(char *e) {
	memset(tokens, 0, sizeof(tokens));
	int position = 0;
	int i;
	regmatch_t pmatch;
	
	nr_token = 0;

	while(e[position] != '\0') {
		/* Try all rules one by one. */
		for(i = 0; i < NR_REGEX; i ++) {
			if(regexec(&re[i], e + position, 1, &pmatch, 0) == 0 && pmatch.rm_so == 0) {
				char *substr_start = e + position;
				int substr_len = pmatch.rm_eo;

				Log("match rules[%d] = \"%s\" at position %d with len %d: %.*s", i, rules[i].regex, position, substr_len, substr_len, substr_start);
				position += substr_len;

				/* TODO: Now a new token is recognized with rules[i]. Add codes
				 * to record the token in the array `tokens'. For certain types
				 * of tokens, some extra actions should be performed.
				 */
				Assert(nr_token < 32, "Expression too long!");
				switch(rules[i].token_type) {
					case NOTYPE:
						break;
					case NEHENUM:
						if(nr_token != 0 && tokens[nr_token - 1].type != '(' && tokens[nr_token - 1].type != ')' && (tokens[nr_token - 1].type == NUM || tokens[nr_token - 1].type == NENUM || tokens[nr_token - 1].type == HENUM)) {
							tokens[nr_token].type = '-';
							nr_token++;
							tokens[nr_token].type = HENUM;
							Assert(substr_len - 3 <= 32, "Token too long!");
							strncpy(tokens[nr_token].str, substr_start + 3, substr_len - 3);
							/*test point*/
							// printf("success!  %s\n", tokens[nr_token].str);
						}
						else {
							tokens[nr_token].type = NEHENUM;
							Assert(substr_len - 2 <= 32, "Token too long!");
							strncpy(tokens[nr_token].str + 1, substr_start + 3, substr_len - 3);
							tokens[nr_token].str[0] = '-';
							/*test point*/
							// printf("success!  %s\n", tokens[nr_token].str);
						}
						nr_token++;
						break;
					case HENUM:
						tokens[nr_token].type = HENUM;
						Assert(substr_len - 2 <= 32, "Token too long!");
						strncpy(tokens[nr_token].str, substr_start + 2, substr_len - 2);
						/*test point*/
						// printf("success!  %s\n", tokens[nr_token].str);
						nr_token++;
						break;
					case NENUM:
						if(nr_token != 0 && tokens[nr_token - 1].type != '(' && tokens[nr_token - 1].type != ')' && (tokens[nr_token - 1].type == NUM || tokens[nr_token - 1].type == NENUM || tokens[nr_token - 1].type == HENUM)) {
							tokens[nr_token].type = '-';
							nr_token++;
							tokens[nr_token].type = NUM;
							Assert(substr_len - 1 <= 32, "Token too long!");
							strncpy(tokens[nr_token].str, substr_start + 1, substr_len - 1);
							/*test point*/
							// printf("success!  %s\n", tokens[nr_token].str);
						}
						else {
							tokens[nr_token].type = NENUM;
							Assert(substr_len <= 32, "Token too long!");
							strncpy(tokens[nr_token].str, substr_start, substr_len);
							/*test point*/
							// printf("success!  %s\n", tokens[nr_token].str);
						}
						nr_token++;
						break;
					case NUM:
						tokens[nr_token].type = NUM;
						Assert(substr_len <= 32, "Token too long!");
						strncpy(tokens[nr_token].str, substr_start, substr_len);
						/*test point*/
						// printf("success!  %s\n", tokens[nr_token].str);
						nr_token++;
						break;
					case REG:
						tokens[nr_token].type = REG;
						Assert(substr_len <= 32, "Token too long!");
						strncpy(tokens[nr_token].str, substr_start, substr_len);
						/*test point*/
						// printf("success!  %s\n", tokens[nr_token].str);
						nr_token++;
						break;
					case '+':
						tokens[nr_token].type = '+';
						nr_token++;
						break;
					case '-':
						if(nr_token == 0) {
							tokens[nr_token].type = NUM;
							strncpy(tokens[nr_token].str, "0", 1);
							nr_token++;
						}
						tokens[nr_token].type = '-';
						nr_token++;
						break;
					case '*':
						if(nr_token != 0 && tokens[nr_token - 1].type != '(' && tokens[nr_token - 1].type != ')' && (tokens[nr_token - 1].type == NUM || tokens[nr_token - 1].type == NENUM || tokens[nr_token - 1].type == HENUM)) {
							tokens[nr_token].type = '*';
						}
						else {
							tokens[nr_token].type = POINTER;
						}
						nr_token++;
						break;
					case '/':
						tokens[nr_token].type = '/';
						nr_token++;
						break;
					case '(':
						tokens[nr_token].type = '(';
						nr_token++;
						break;
					case ')':
						tokens[nr_token].type = ')';
						nr_token++;
						break;
					case EQ:
						tokens[nr_token].type = EQ;
						nr_token++;
						break;
					case UNEQ:
						tokens[nr_token].type = UNEQ;
						nr_token++;
						break;
					case AND:
						tokens[nr_token].type = AND;
						nr_token++;
						break;
					case OR:
						tokens[nr_token].type = OR;
						nr_token++;
						break;
					case NOT:
						tokens[nr_token].type = NOT;
						nr_token++;
						break;
					case VAR:
						tokens[nr_token].type = VAR;
						nr_token++;
						break;
					default: panic("please implement me");
				}

				break;
			}
		}

		if(i == NR_REGEX) {
			printf("no match at position %d\n%s\n%*.s^\n", position, e, position, "");
			return false;
		}
	}

	return true; 
}

bool check_parentheses(int p, int q){
	int top = 0;
	if(tokens[p].type == '(') {
		int i;
		for(i = p + 1; i<= q - 1; i++) {
			if(tokens[i].type == '(') {
				top++;
			}
			else if(tokens[i].type == ')') {
				if(top > 0) {
					top--;
				}
			}
		}
		if(tokens[q].type == ')' && top == 0) {
			return true;
		}
	}
	return false;
}

u_int32_t eval(int p, int q, bool *success) {
	if(*success == false) {
		return 0;
	}
	
	/*test point*/
	printf("tokens\n");
	int i;
	for(i = p; i <= q; i++) {
		printf("%d %s", tokens[i].type, tokens[i].str);
	}
	printf("\n");
	printf("%d\n", check_parentheses(p, q));

	if(p > q) {
		*success = false;
		printf("ERROR_1!\n");
		return 0;
	}
	else if(p == q) {
		if(tokens[p].type == NUM || tokens[p].type == NENUM || tokens[p].type == HENUM || tokens[p].type == NEHENUM) {
			int i;
			int e = 1;
			int sum = 0;
			if(tokens[p].type == NUM || tokens[p].type == NENUM) {
				for(i = 31; i >= 0; i--) {
					char tmp = tokens[p].str[i];
					if(tmp == '\0') {
						continue;
					}
					else if(tmp >= '0' && tmp <= '9') {
						sum += e * (int)(tmp - '0');
						e *= 10;
					}
					else if(i == 0 && tmp == '-') {
						sum = -sum;
					}
				}
			}
			else {
				for(i = 31; i >= 0; i--) {
					char tmp = tokens[p].str[i];
					if(tmp == '\0') {
						continue;
					}
					else if(tmp >= '0' && tmp <= '9') {
						sum += e * (int)(tmp - '0');
						e *= 16;
					}
					else if(tmp >= 'a' && tmp <= 'f') {
						sum += e * ((int)(tmp - 'a') + 10);
						e *= 16;
					}
					else if(tmp >= 'A' && tmp <= 'F') {
						sum += e * ((int)(tmp - 'A') + 10);
						e *= 16;
					}
					else if(i == 0 && tmp == '-') {
						sum = -sum;
					}
				}
			}
			return sum;
		}
		else if(tokens[p].type == VAR) {
			return 0;
		}
		else {
			*success = false;
			printf("ERROR_2!\n");
			return 0;
		}
	}
	else if (check_parentheses(p, q) == true) {
		return eval(p + 1, q - 1, success);
	}
	else {
		int stack[32] = {};
		int stack_i[32] = {};
		int top = 0;
		int i;
		for(i = p; i <= q; i++) {
			int tmp = tokens[i].type;
			int tmp_s = stack[top - 1];
			if((top == 0 && tmp != NENUM && tmp != NUM && tmp != NEHENUM && tmp != HENUM && tmp != VAR) || tmp == '(') {
				stack[top] = tmp;
				stack_i[top] = i;
				top++;
			}
			else if(tmp == ')') {
				while(top > 0){
					top--;
					if(stack[top] == '(') {
						break;
					}
				}
				if(stack[top] != '(') {
					*success = false;
					printf("%c  %d\n", stack[top], top);
					printf("ERROR_3!\n");
					return 0;
				}
			}
			else if(tmp == NOT || tmp == POINTER) {
				if(tmp_s == '(') {
					stack[top] = tmp;
					stack_i[top] = i;
					top++;
				}
			}
			else if(tmp == '*' || tmp == '/') {
				if(tmp_s == '(') {
					stack[top] = tmp;
					stack_i[top] = i;
					top++;
				}
				else if(tmp_s == NOT || tmp_s == POINTER || tmp_s == '*' || tmp_s == '/') {
					top--;
					stack[top] = tmp;
					stack_i[top] = i;
					top++;
				}
			}
			else if(tmp == '+' || tmp == '-') {
				if(stack[top - 1] == '(') {
					stack[top] = tmp;
					stack_i[top] = i;
					top++;
				}
				else if(tmp_s == NOT || tmp_s == POINTER || tmp_s == '*' || tmp_s == '/' || tmp_s == '+' || tmp_s == '-') {
					top--;
					stack[top] = tmp;
					stack_i[top] = i;
					top++;
				}
			}
			else if(tmp == EQ || tmp == UNEQ) {
				if(tmp_s == '(') {
					stack[top] = tmp;
					stack_i[top] = i;
					top++;
				}
				else if(tmp_s == NOT || tmp_s == POINTER || tmp_s == '*' || tmp_s == '/' || tmp_s == '+' || tmp_s == '-' || tmp_s == EQ || tmp_s == UNEQ) {
					top--;
					stack[top] = tmp;
					stack_i[top] = i;
					top++;
				}
			}
			else if(tmp == AND) {
				if(tmp_s == '(') {
					stack[top] = tmp;
					stack_i[top] = i;
					top++;
				}
				else if(tmp_s == NOT || tmp_s == POINTER || tmp_s == '*' || tmp_s == '/' || tmp_s == '+' || tmp_s == '-' || tmp_s == EQ || tmp_s == UNEQ || tmp_s == AND) {
					top--;
					stack[top] = tmp;
					stack_i[top] = i;
					top++;
				}
			}
			else if(tmp == OR) {
				if(tmp_s == '(') {
					stack[top] = tmp;
					stack_i[top] = i;
					top++;
				}
				else {
					top--;
					stack[top] = tmp;
					stack_i[top] = i;
					top++;
				}
			}
			/*test point*/
			// int j;
			// for(j = 0; j < 32; j++){
			// 	printf("%d ", stack_i[j]);
			// }
			// printf("\n");
			// for(j = 0; j < 32; j++){
			// 	printf("%c ", stack[j]);
			// }
			// printf("----top%d --i%d\n", top, i);

		}
		
		/*test point*/
		// printf("top at %d\n", top);
		
		if(!(top == 1 && stack[0] != '(' && stack[0] != ')')) {
			*success = false;
			printf("ERROR_4!\n");
			return 0;
		}
		int op = stack_i[0];
		int op_type = stack[0];

		/*test point*/
		// printf("op\n%d%c %d\n", op_type, op_type, op);
		if(op_type == NOT || op_type == POINTER) {
			if(op != p) {
				*success = false;
				printf("ERROR5!\n");
				return 0;
			}
			else {
				int val_1 = eval(op + 1, q, success);
				switch(op_type) {
					case NOT: return !val_1;break;
					case POINTER: panic("No pointer!");
					default:
						*success = false;
						printf("ERROR6!\n");
						return 0;
				}
			}
		}
		else {
			int val_1 = eval(p, op - 1, success);
			int val_2 = eval(op + 1, q, success);
			switch(op_type) {
				case '*': return (val_1 * val_2);break;
				case '/': return (val_1 / val_2);break;
				case '+': return (val_1 + val_2);break;
				case '-': return (val_1 - val_2);break;
				case EQ: return (val_1 == val_2);break;
				case UNEQ: return (val_1 != val_2);break;
				case AND: return (val_1 && val_2);break;
				case OR: return (val_1 || val_2);break;
				default: 
					*success = false;
					printf("ERROR_6!\n");
					return 0;
			}
		}
	}
}

uint32_t expr(char *e, bool *success) {
	if(!make_token(e)) {
		*success = false;
		return 0;
	}
	/* TODO: Insert codes to evaluate the expression. */
	int res = eval(0, nr_token - 1, success);
	return res;
	//panic("please implement me");
	//return 0;
}

//p ((((((4+5)*7)-8)*6)-76+4)-99+-9)
//p (3*(75-65)+!0+(1&&1))+(1||0)