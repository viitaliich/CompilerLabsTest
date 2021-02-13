Pull ast_pull;		// memory pull for keeping AST nodes

void* ast_alloc(size_t size) {
	assert(size != 0);
	void* ptr = pull_alloc(&ast_pull, size);
	memset(ptr, 0, size);
	return ptr;
}

Program* program() {
	Program* prog = (Program*)ast_alloc(sizeof(Program));
	prog->func_queue = new FuncQueue();
	prog->defined_func = new FuncQueue();
	return prog;
}

FuncDecl* func_decl(const char* name) {
	FuncDecl* f_decl = (FuncDecl*)ast_alloc(sizeof(FuncDecl));
	f_decl->name = name;
	f_decl->parameters = new FuncParams();
	f_decl->stmt_queue = new StatementQueue();
	return f_decl;
}

Statement* statement() {
	Statement* stmt = (Statement*)ast_alloc(sizeof(Statement));
	stmt->stmt_queue = new StatementQueue();
	stmt->stmt_queue_two = new StatementQueue();
	return stmt;
}

Expression* expression() {
	Expression* exp = (Expression*)ast_alloc(sizeof(Expression));
	exp->args = new FuncArgs();
	return exp;
}