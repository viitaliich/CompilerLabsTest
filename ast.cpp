Pull ast_pull;		// memory pull for keeping AST nodes

void* ast_alloc(size_t size) {
	assert(size != 0);
	void* ptr = pull_alloc(&ast_pull, size);
	memset(ptr, 0, size);
	return ptr;
}

Program* program(FuncDecl* func_decl) {
	Program* prog = (Program*)ast_alloc(sizeof(FuncDecl));
	prog->func_decl = func_decl;
	return prog;
}

FuncDecl* func_decl(const char* name) {
	FuncDecl* f_decl = (FuncDecl*)ast_alloc(sizeof(FuncDecl));
	f_decl->name = name;
	return f_decl;
}

Statement* statement() {
	Statement* stmt = (Statement*)ast_alloc(sizeof(Statement));
	return stmt;
}

Expression* expression() {
	Expression* exp = (Expression*)ast_alloc(sizeof(Expression));
	return exp;
}