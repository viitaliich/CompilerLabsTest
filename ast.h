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
}ExprKind;

typedef enum StmtKind {
	RET_STMT,
}StmtKind;

typedef struct Expression {
	ExprKind kind;
	union {
		int int_val;
		double float_val;
		const char* str_val;
		//double int_val;	// ???
	};
	Expression* exp_left;
	Expression* exp_right;

}Expression;

typedef struct Statement {
	Expression* expr;
	StmtKind kind;
}Statement;

typedef struct FuncDecl {
	const char* name;
	Statement* stmt;
}FuncDecl;

typedef struct Program {
	FuncDecl* func_decl;
}Program;