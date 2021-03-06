typedef enum ExprKind {
	EXP_INT,
	EXP_FLOAT,
	EXP_UN_COMP,
	EXP_UN_NEG,
	EXP_UN_LOGNEG,
	EXP_BIN_ADD,
	EXP_BIN_NEG,
	EXP_BIN_MUL,
	EXP_BIN_DIV,
	EXP_BIN_MOD,
	EXP_BIN_AND,
	EXP_BIN_OR,
	EXP_BIN_EQL,
	EXP_BIN_NEQL,
	EXP_BIN_LESS,
	EXP_BIN_LESS_EQL,
	EXP_BIN_GREATER,
	EXP_BIN_GREATER_EQL,
	EXP_BIN_BIT_AND,
	EXP_BIN_BIT_OR,
	EXP_BIN_XOR,
	EXP_BIN_SH_LEFT,
	EXP_BIN_SH_RIGHT,
	EXP_ASSIGN,
	EXP_VAR,
	EXP_TERNARY,
	EXP_CALL,
}ExprKind;

typedef enum StmtKind {
	STMT_RET,
	STMT_EXP,
	STMT_IF,
	STMT_ELSE,
	STMT_FOR,
	STMT_WHILE,
	STMT_BREAK,
	STMT_CONTINUE,
}StmtKind;


typedef struct Expression {
	ExprKind kind;
	union {
		int int_val;
		double float_val;
		const char* str_val;
		const char* var;
	};
	Expression* exp_left;
	Expression* exp_right;
	Expression* exp_else;
	std::vector <Expression*>* args;
}Expression;

typedef std::vector <Expression*> FuncParams;

typedef std::vector <Expression*> FuncArgs;


typedef struct Statement {
	Expression* expr;			// range(a, ...	???
	Expression* expr_two;		// ... b, ...
	Expression* expr_three;		// ... c)
	Statement* stmt;
	StmtKind kind;
	std::queue <Statement*> *stmt_queue;
	std::queue <Statement*>* stmt_queue_two;
}Statement;

typedef std::queue <Statement*> StatementQueue;

typedef struct FuncDecl {
	const char* name;
	FuncParams* parameters;
	StatementQueue* stmt_queue;
}FuncDecl;

typedef std::vector <FuncDecl*> FuncQueue;

typedef struct Program {
	FuncQueue* func_queue;
	FuncQueue* defined_func;
}Program;