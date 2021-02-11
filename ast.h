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
}ExprKind;

typedef enum StmtKind {
	STMT_RET,
	STMT_EXP,
	STMT_IF,
	STMT_ELSE,
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
}Expression;

typedef struct Statement {
	Expression* expr;
	Statement* stmt;
	StmtKind kind;
	std::queue <Statement*> *stmt_queue;
	std::queue <Statement*>* stmt_queue_two;
}Statement;

typedef std::queue <Statement*> StatementQueue;

typedef struct FuncDecl {
	const char* name;
	std::queue <Statement*>* stmt_queue;
}FuncDecl;

typedef struct Program {
	FuncDecl* func_decl;
}Program;