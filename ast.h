typedef enum ExprKind {
	INT,
	FLOAT,
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