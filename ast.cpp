Pull ast_pull;

void* ast_alloc(size_t size) {
	assert(size != 0);
	void* ptr = pull_alloc(&ast_pull, size);
	memset(ptr, 0, size);		// set 0 in given memory chunk		// why do we do this?	???
	return ptr;
}

Program* program(FuncDecl* func_decl) {
	Program* prog = (Program*)ast_alloc(sizeof(FuncDecl));
	prog->func_decl = func_decl;
	return prog;
}

FuncDecl* func_decl(const char* name, Statement* statement) {
	FuncDecl* f_decl = (FuncDecl*)ast_alloc(sizeof(FuncDecl));
	f_decl->name = name;
	f_decl->stmt = statement;
	return f_decl;
}

Statement* statement(Expression* expr) {
	Statement* stmt = (Statement*)ast_alloc(sizeof(Expression));
	stmt->expr = expr;
	stmt->kind = RET_STMT;
	return stmt;
}

Expression* expression() {
	Expression* exp = (Expression*)ast_alloc(sizeof(Expression));
	return exp;
}