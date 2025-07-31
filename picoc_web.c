#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stddef.h>

typedef enum {
    TOK_INT, TOK_FLOAT, TOK_CHAR, TOK_ID, TOK_NUM, TOK_CHAR_LIT,
    TOK_ASSIGN, TOK_PRINT, TOK_IF, TOK_WHILE,
    TOK_LPAREN, TOK_RPAREN, TOK_LBRACE, TOK_RBRACE,
    TOK_PLUS, TOK_MINUS, TOK_MUL, TOK_DIV, TOK_SEMI,
    TOK_EQ, TOK_NEQ, TOK_LT, TOK_GT, TOK_LE, TOK_GE, TOK_EOF
} TokenType;

typedef struct {
    TokenType type;
    char lexeme[64];
} Token;

char *src; int pos = 0;
Token curr;

typedef struct Node {
    enum { N_ASSIGN, N_PRINT, N_BINOP, N_NUM, N_VAR, N_CHAR, N_FLOAT, N_IF, N_WHILE } type;
    union {
        struct { char var[10]; struct Node* val; } assign;
        struct Node* print;
        struct { struct Node *l, *r; char op[3]; } bin;
        float fval;
        int ival;
        char cval;
        char var[10];
        struct { struct Node* cond; struct Node* body; } ctrl;
    } data;
    struct Node* next;
} Node;


Node* expr();
Node* statement();
Node* block();

Token nextToken() {
    while (isspace(src[pos])) pos++;
    Token t = {TOK_EOF, ""};

    if (isdigit(src[pos])) {
        int i = 0;
        while (isdigit(src[pos]) || src[pos] == '.') t.lexeme[i++] = src[pos++];
        t.lexeme[i] = '\0';
        t.type = strchr(t.lexeme, '.') ? TOK_FLOAT : TOK_NUM;
    } else if (isalpha(src[pos])) {
        int i = 0;
        while (isalnum(src[pos])) t.lexeme[i++] = src[pos++];
        t.lexeme[i] = '\0';
        if (!strcmp(t.lexeme, "int")) t.type = TOK_INT;
        else if (!strcmp(t.lexeme, "float")) t.type = TOK_FLOAT;
        else if (!strcmp(t.lexeme, "char")) t.type = TOK_CHAR;
        else if (!strcmp(t.lexeme, "print")) t.type = TOK_PRINT;
        else if (!strcmp(t.lexeme, "if")) t.type = TOK_IF;
        else if (!strcmp(t.lexeme, "while")) t.type = TOK_WHILE;
        else t.type = TOK_ID;
    } else if (src[pos] == '\'') {
        t.lexeme[0] = src[++pos]; t.lexeme[1] = '\0';
        pos += 2; t.type = TOK_CHAR_LIT;
    } else {
        switch (src[pos]) {
            case '+': t.type = TOK_PLUS; break;
            case '-': t.type = TOK_MINUS; break;
            case '*': t.type = TOK_MUL; break;
            case '/': t.type = TOK_DIV; break;
            case '=':
                if (src[pos + 1] == '=') { t.type = TOK_EQ; strcpy(t.lexeme, "=="); pos += 2; return t; }
                else t.type = TOK_ASSIGN; break;
            case '!':
                if (src[pos + 1] == '=') { t.type = TOK_NEQ; strcpy(t.lexeme, "!="); pos += 2; return t; }
                break;
            case '<':
                if (src[pos + 1] == '=') { t.type = TOK_LE; strcpy(t.lexeme, "<="); pos += 2; return t; }
                else t.type = TOK_LT; break;
            case '>':
                if (src[pos + 1] == '=') { t.type = TOK_GE; strcpy(t.lexeme, ">="); pos += 2; return t; }
                else t.type = TOK_GT; break;
            case ';': t.type = TOK_SEMI; break;
            case '(': t.type = TOK_LPAREN; break;
            case ')': t.type = TOK_RPAREN; break;
            case '{': t.type = TOK_LBRACE; break;
            case '}': t.type = TOK_RBRACE; break;
        }
        if (!t.lexeme[0]) {
            t.lexeme[0] = src[pos];
            t.lexeme[1] = '\0';
            pos++;
        }
    }
    return t;
}

void advance() { curr = nextToken(); }

Node* factor() {
    if (curr.type == TOK_NUM) {
        Node* n = calloc(1, sizeof(Node)); n->type = N_NUM;
        n->data.ival = atoi(curr.lexeme); advance(); return n;
    }
    if (curr.type == TOK_FLOAT) {
        Node* n = calloc(1, sizeof(Node)); n->type = N_FLOAT;
        n->data.fval = atof(curr.lexeme); advance(); return n;
    }
    if (curr.type == TOK_CHAR_LIT) {
        Node* n = calloc(1, sizeof(Node)); n->type = N_CHAR;
        n->data.cval = curr.lexeme[0]; advance(); return n;
    }
    if (curr.type == TOK_ID) {
        Node* n = calloc(1, sizeof(Node)); n->type = N_VAR;
        strncpy(n->data.var, curr.lexeme, 9);
        n->data.var[9] = '\0';
        advance(); return n;
    }
    return NULL;
}

Node* comparison() {
    Node* left = expr();
    if (curr.type >= TOK_EQ && curr.type <= TOK_GE) {
        Node* n = calloc(1, sizeof(Node)); n->type = N_BINOP;
        n->data.bin.l = left;
        strcpy(n->data.bin.op, curr.lexeme);
        advance();
        n->data.bin.r = expr();
        return n;
    }
    return left;
}

Node* term() {
    Node* left = factor();
    while (curr.type == TOK_MUL || curr.type == TOK_DIV) {
        Node* n = calloc(1, sizeof(Node)); n->type = N_BINOP;
        n->data.bin.l = left;
        sprintf(n->data.bin.op, "%c", curr.lexeme[0]);
        advance();
        n->data.bin.r = factor();
        left = n;
    }
    return left;
}

Node* expr() {
    Node* left = term();
    while (curr.type == TOK_PLUS || curr.type == TOK_MINUS) {
        Node* n = calloc(1, sizeof(Node)); n->type = N_BINOP;
        n->data.bin.l = left;
        sprintf(n->data.bin.op, "%c", curr.lexeme[0]);
        advance();
        n->data.bin.r = term();
        left = n;
    }
    return left;
}

Node* block() {
    Node* head = NULL;
    if (curr.type == TOK_LBRACE) {
        advance();
        Node* head = NULL, *last = NULL;
        while (curr.type != TOK_RBRACE && curr.type != TOK_EOF) {
            Node* stmt = statement();
            if (stmt != NULL) {
                if (head == NULL) {
                    head = stmt;
                } else {
                    last->next = stmt;
                }
                last = stmt;
            }
        }
        if (curr.type == TOK_RBRACE) {
            advance();
        }
        return head;
    }
    return statement(); 
}

Node* statement() {
    if (curr.type == TOK_LBRACE) {
        return block();
    }
    if (curr.type == TOK_PRINT) {
        advance();
        Node* n = calloc(1, sizeof(Node)); n->type = N_PRINT;
        n->data.print = expr();
        if (curr.type == TOK_SEMI) advance();
        return n;
    }
    if (curr.type == TOK_INT || curr.type == TOK_FLOAT || curr.type == TOK_CHAR) {
        advance(); 
        char var[10];
        strncpy(var, curr.lexeme, 9);
        var[9] = '\0';
        advance();
        if (curr.type == TOK_ASSIGN) {
            advance();
            Node* rhs = expr();
            Node* n = calloc(1, sizeof(Node)); n->type = N_ASSIGN;
            strcpy(n->data.assign.var, var);
            n->data.assign.val = rhs;
            if (curr.type == TOK_SEMI) advance();
            return n;
        }
    }
    if (curr.type == TOK_IF) {
        advance(); if (curr.type == TOK_LPAREN) advance();
        Node* cond = comparison(); if (curr.type == TOK_RPAREN) advance();
        Node* body = block();
        Node* n = calloc(1, sizeof(Node)); n->type = N_IF;
        n->data.ctrl.cond = cond;
        n->data.ctrl.body = body;
        return n;
    }
    if (curr.type == TOK_WHILE) {
        advance(); if (curr.type == TOK_LPAREN) advance();
        Node* cond = comparison(); if (curr.type == TOK_RPAREN) advance();
        Node* body = block();
        Node* n = calloc(1, sizeof(Node)); n->type = N_WHILE;
        n->data.ctrl.cond = cond;
        n->data.ctrl.body = body;
        return n;
    }
    
    if (curr.type != TOK_EOF) {
        advance();
    }
    return NULL;
}

float vars[26];

float eval(Node* n) {
    if (!n) return 0;
    switch (n->type) {
        case N_NUM: return n->data.ival;
        case N_FLOAT: return n->data.fval;
        case N_CHAR: return n->data.cval;
        case N_VAR: return vars[n->data.var[0] - 'a'];
        case N_ASSIGN: {
            float v = eval(n->data.assign.val);
            vars[n->data.assign.var[0] - 'a'] = v;
            return v;
        }
        case N_PRINT: {
            float v = eval(n->data.print);
            printf("Output: %.2f\n", v); return v;
        }
        case N_BINOP: {
            float l = eval(n->data.bin.l), r = eval(n->data.bin.r);
            if (!strcmp(n->data.bin.op, "+")) return l + r;
            if (!strcmp(n->data.bin.op, "-")) return l - r;
            if (!strcmp(n->data.bin.op, "*")) return l * r;
            if (!strcmp(n->data.bin.op, "/")) return r != 0 ? l / r : 0;
            if (!strcmp(n->data.bin.op, "==")) return l == r;
            if (!strcmp(n->data.bin.op, "!=")) return l != r;
            if (!strcmp(n->data.bin.op, "<")) return l < r;
            if (!strcmp(n->data.bin.op, "<=")) return l <= r;
            if (!strcmp(n->data.bin.op, ">")) return l > r;
            if (!strcmp(n->data.bin.op, ">=")) return l >= r;
        }
        case N_IF: if (eval(n->data.ctrl.cond)) {
            Node* b = n->data.ctrl.body;
            while (b) { eval(b); b = b->next; }
        } break;
        case N_WHILE: while (eval(n->data.ctrl.cond)) {
            Node* b = n->data.ctrl.body;
            while (b) { eval(b); b = b->next; }
        } break;
    }
    return 0;
}

int main() {
    FILE* f = fopen("input.txt", "r");
    if (f == NULL) {
        printf("Runtime Error: input.txt not found or could not be opened.\n");
        return 1;
    }

    static char input[4096], line[256];
    input[0] = '\0';
    size_t current_len = 0;
    while (fgets(line, sizeof(line), f)) {
        size_t line_len = strlen(line);
        if (current_len + line_len < sizeof(input)) {
            strcat(input, line);
            current_len += line_len;
        } else {
            break; 
        }
    }
    fclose(f);

    src = input;
    pos = 0;
    advance();

    
    Node* head = NULL, *last = NULL;
    while (curr.type != TOK_EOF) {
        Node* stmt = statement();
        if (stmt != NULL) {
            if (head == NULL) {
                head = stmt;
            } else {
                last->next = stmt;
            }
            last = stmt;
        }
    }

    
    Node* ast = head;
    while (ast) {
        eval(ast);
        ast = ast->next;
    }
    return 0;
}




