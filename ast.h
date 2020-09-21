typedef enum ExprKind {
	RET_EXPR,
}ExprKind;

typedef struct Expression {
	ExprKind kind;
	union {
		int int_val;
		double float_val;
		const char* str_val;
		//double int_val;	// ???
	};
}Expression;

typedef struct Statement {
	Expression* expr;
}Statement;

typedef struct FuncDecl {
	const char* name;
	Statement* stmt;
}FuncDecl;

typedef struct Program {
	FuncDecl* func_decl;
}Program;

