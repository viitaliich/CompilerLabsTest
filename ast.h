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