#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>
#include <stdbool.h>
#include <errno.h>
#include <assert.h>
#include <string.h>

// TODO Brakets.
// Looks like brackets can be parsed just as a flag of OP

typedef enum {
	NUM,
	OP,
} Token_type;

typedef enum {
	PLUS,
	MINUS,
	MUL,
	TOTAL_OPERATORS,
} OP_type;

unsigned char precedence[TOTAL_OPERATORS] = {
	[PLUS]  = 80,
	[MINUS] = 80,
	[MUL]   = 160,
};

typedef struct _token Token;
struct _token {
	Token_type type;
	union {
		int value;
		OP_type op;
	};
	struct _token *left, *right;
};

typedef struct {
	Token *data;
	size_t size;
	size_t capacity;
} Tokens;

Tokens *tokens_init()
{
#define INITIAL_ARRAY_CAPACITY 16
	Tokens *tokens = malloc(sizeof(Tokens));
	if (tokens == NULL) {
		fprintf(stderr, "Memory fail\n");
		exit(1);
	}

	tokens->data = malloc(sizeof(*tokens->data) * INITIAL_ARRAY_CAPACITY);
	if (tokens->data == NULL) {
		fprintf(stderr, "Memory fail\n");
		exit(1);
	}

	tokens->capacity = INITIAL_ARRAY_CAPACITY;
	tokens->size = 0;

	return tokens;
}

void append(Tokens *arr, Token token)
{
	if (arr->size >= arr->capacity) {
		arr->capacity *= 2;
		arr->data = realloc(arr->data, sizeof(*arr->data) * arr->capacity);
		if (arr->data == NULL) {
			fprintf(stderr, "Not enough memory\n");
			exit(1);
		}
	}

	arr->data[arr->size++] = token;
}

void new_token(Tokens *tokens, Token_type type, int value)
{
	Token result = {0};
	result.type = type;
	result.value = value;

	append(tokens, result);
}

void tokinizer(Tokens *tokens, const char *input)
{
	memset(tokens->data, 0, tokens->capacity*sizeof(tokens->data[0]));

	size_t i = 0;
	while (input[i] != '\0') {
		if (isspace(input[i])) {
			++i;
		} else if (isdigit(input[i])) {
			new_token(tokens, NUM, atoi(&input[i]));
			do {
				++i;
			} while (isdigit(input[i]));
		} else {
			switch (input[i]) {
				case '+':
					new_token(tokens, OP, PLUS);
					break;
				case '-':
					new_token(tokens, OP, MINUS);
					break;
				case '*':
					new_token(tokens, OP, MUL);
					break;
				default:
					fprintf(stderr, "ERROR: unknown symbol '%c'\n", input[i]);
					exit(4);
			}
			++i;
		}
	}
}

void print_tokens(Tokens *tokens)
{
	printf("tokens :%zu\n", tokens->size);

	for (size_t i = 0; i < tokens->size; ++i) {
		switch (tokens->data[i].type) {
			case NUM:
				printf("|%d|", tokens->data[i].value);
				break;
			case OP:
				switch (tokens->data[i].op) {
					case PLUS:
						printf("|PLUS|");
						break;
					case MINUS:
						printf("|MINUS|");
						break;
					case MUL:
						printf("|MUL|");
						break;
					default:
						fprintf(stderr, "ERROR: unknown operand\n");
				}
				break;
			default:
				fprintf(stderr, "ERROR: wrong token type\n");
				exit(88);
		}
	}
	putchar('\n');
}

Token *make_tree(Tokens *tokens)
{
	Token *tree = tokens->data;

	for (size_t i = 0; i < tokens->size; ++i) {
		switch (tokens->data[i].type) {
			case NUM:
				break;
			case OP: {
				Token *tmp = tree;
				tree = &tokens->data[i];
				tree->left = tmp;
				tree->right = &tokens->data[i+1];

				if (tree->op == MUL 
						&& tree->left->type == OP
						&& tree->left->op != MUL) {
					tmp = tree;
					tree = tmp->left;
					tmp->left = tree->right;
					tree->right = tmp;
				}
				break; }
			default:
				fprintf(stderr, "ERROR: wrong token type\n");
				exit(1);
		}
	}

	return tree;
}

Token *parse_expression(Token *curr_token)
{
	Token *next_token = curr_token+1;

	switch (curr_token->type) {
		case NUM:
			if (next_token->type == OP) {
				next_token->left = curr_token;
				next_token->right = parse_expression(curr_token+2);
				
				if (next_token->right->type == OP && 
						precedence[next_token->op] 
						> precedence[next_token->right->op]) {
					Token *tmp = next_token->right;
					next_token->right = tmp->left;
					tmp->left = next_token;
					next_token = tmp;
				}

				return next_token;
			} else {
				return curr_token;
			}
		case OP:
			fprintf(stderr, "Syntax error\n");
			exit(1);
		default:
			fprintf(stderr, "UNREACHABLE\n");
			exit(1);
	}

	return NULL; // UNREACHABLE
}

Token *make_tree_recursive(Tokens *tokens)
{
	Token *tree = parse_expression(tokens->data);

	return tree;
}

int process_tree(Token *tree)
{
	if (!tree) {
		fprintf(stderr, "ERROR: broken tree\n");
		exit(2);
	}

	switch (tree->type) {
		case OP:
			switch (tree->op) {
				case PLUS:
					return process_tree(tree->left) + process_tree(tree->right);
				case MINUS:
					return process_tree(tree->left) - process_tree(tree->right);
				case MUL:
					return process_tree(tree->left) * process_tree(tree->right);
				case TOTAL_OPERATORS:
					assert(false && "Wrong operator");
			}
			break;
		case NUM:
			return tree->value;
	}
	return 0;
}

#define READ_BUFFER_SIZE 512
char read_buffer[READ_BUFFER_SIZE];

void process_input(char *input, Tokens *tokens)
{
	tokinizer(tokens, input);

	Token *recursive_tree = make_tree_recursive(tokens);

	printf("%d\n", process_tree(recursive_tree));
}

int main(int argc, char **argv)
{
	FILE *input_file;

	if (argc < 2)
		input_file = stdin;
	else
		if (*argv[1] == '-')
			input_file = stdin;
		else
			assert(false && "unemplemented");

	char *result = NULL;

	Tokens *tokens = tokens_init();

	do {
		errno = 0;

		result = fgets(read_buffer, READ_BUFFER_SIZE, input_file);

		if (result) {
			tokens->size = 0;
			process_input(result, tokens);
		}
	} while (result);

	free(tokens->data);
	free(tokens);

	if (errno) {
		perror("Input read failed");
		return 1;
	}

	return 0;
}
