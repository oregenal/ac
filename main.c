#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>

const char input[] = "2 + 3 + 4 + 5 - 6 + 7 - 8 + 9 - 4 + 8 - 7 + 6 - 5 - 9";
// const char input[] = "2 + 3 - 1";

typedef enum {
	NUM,
	OP,
} Token_type;

typedef enum {
	PLUS,
	MINUS,
} OP_type;

typedef struct _token Token;
struct _token {
	Token_type type;
	union {
		int value;
		OP_type op;
	};
	Token *left, *right;
};

typedef struct {
	Token **data;
	size_t size;
	size_t capacity;
} Tokens;

#define append(arr, token) \
	do { \
		if ((arr)->size >= (arr)->capacity) { \
			int new_size = (arr)->size * 2 * sizeof((arr)->data); \
			(arr)->data = realloc(((arr)->data), new_size); \
			(arr)->capacity = new_size; \
		} \
		(arr)->data[((arr)->size)++] = (token); \
	} while(0)

Token *new_token(Token_type type, OP_type op)
{
	Token *result = malloc(sizeof *result);
	result->type = type;
	result->op = op;
	result->left = NULL;
	result->right = NULL;
	return result;
}

Tokens *tokinizer(const char *input, size_t input_len)
{
	Tokens *tokens = malloc(sizeof(Tokens));
	tokens->data = malloc(sizeof(tokens->data) * 16);
	tokens->capacity = 16;

	printf("%s\n", input);
	printf("size :%zu\n", input_len);

	for (size_t i = 0; i < input_len; ++i) {
		if (isspace(input[i])) {
			continue;
		} else if (isdigit(input[i])) {
			append(tokens, new_token(NUM, atoi(&input[i])));
			continue;
		} else if (input[i] == '+') {
			append(tokens, new_token(OP, PLUS));
			continue;
		} else if (input[i] == '-') {
			append(tokens, new_token(OP, MINUS));
			continue;
		}
		printf("|%c|", input[i]);
	}

	printf("tokens :%zu\n", tokens->size);

	return tokens;
}

void print_tokens(Tokens *tokens)
{
	for (size_t i = 0; i < tokens->size; ++i) {
		switch (tokens->data[i]->type) {
			case NUM:
				printf("|%d|", tokens->data[i]->value);
				break;
			case OP:
				printf("|%s|", tokens->data[i]->op == 0 ? "PLUS" : "MINUS");
				break;
			default:
				fprintf(stderr, "ERROR: wrong tokey type\n");
				exit(88);
		}
	}
	putchar('\n');
}

Token *make_tree(Tokens *tokens)
{
	Token *tree = tokens->data[0];

	for (size_t i = 0; i < tokens->size; ++i) {
		switch (tokens->data[i]->type) {
			case NUM:
				break;
			case OP: {
				Token *tmp = tree;
				tree = tokens->data[i];
				tree->left = tmp;
				tree->right = tokens->data[i+1];
				break; }
			default:
				fprintf(stderr, "ERROR: wrong token type\n");
				exit(1);
		}
	}

	return tree;
}

int process_tree(Token *tree)
{
	if (!tree) {
		fprintf(stderr, "ERROR: broken tree\n");
		exit(2);
	}

	switch (tree->type == OP) {
		case OP:
			switch (tree->op) {
				case PLUS:
					return process_tree(tree->left) + process_tree(tree->right);
					break;
				case MINUS:
					return process_tree(tree->left) - process_tree(tree->right);
					break;
			}
			break;
		case NUM:
			return tree->value;
	}
	return 0;
}

int main(void)
{
	size_t input_len = sizeof(input) / sizeof(input[0]) - 1;

	Tokens *tokens = tokinizer(input, input_len);

	print_tokens(tokens);

	Token *tree = make_tree(tokens);

	printf("result: %d\n", process_tree(tree));

	printf("\nDone!\n");

	return 0;
}
