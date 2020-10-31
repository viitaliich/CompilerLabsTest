char* buf = nullptr;
size_t buf_len = 0;
size_t buf_cap = 0;

char* buf_printf(char* buf, const char* format, ...) {
	va_list args;
	va_start(args, format);
	if (buf_cap - buf_len < sizeof(args)) (char*)xrealloc(buf, 2 * buf_cap + sizeof(args));
	buf_cap = buf_cap * 2 + sizeof(args);
	int ret = vsprintf(buf + buf_len, format, args);
	buf_len += ret;
	va_end(args);
	return buf;
}

void gen_int_exp() {
	buf = buf_printf(buf, "\t mov eax, %d \n", prog->func_decl->stmt->expr->int_val);
}

void gen_float_exp() {
	prog->func_decl->stmt->expr->int_val = (int)prog->func_decl->stmt->expr->float_val;	// float --> int
	buf = buf_printf(buf, "\t mov eax, %d \n", prog->func_decl->stmt->expr->int_val);
}

//void gen_str_exp() {
//	buf = buf_printf(buf, "\t ret %d \n", prog->func_decl->stmt->expr->int_val);
//	printf("%s", buf);
//}

void gen_ret_exp() {
	if (prog->func_decl->stmt->expr->kind == FLOAT) {
		gen_float_exp();
	}
	else if (prog->func_decl->stmt->expr->kind == INT) {
		gen_int_exp();
	}
	else {
		fatal("Nothing to generate in return expression in function [%s]", prog->func_decl->name);
	}
	//if (prog->func_decl->stmt->expr->int_val != NULL) gen_str_exp(); return;
	buf = buf_printf(buf, "\t ret \n");
}

void gen_stmt() {
	if (prog->func_decl->stmt->kind == RET_STMT) {
		gen_ret_exp();
	}
	else {
		fatal("No expression to generate in function [%s]", prog->func_decl->name);
	}
}

void gen_func_decl() {
	if (prog->func_decl->name == nullptr) fatal("No function name to generate");
	buf = buf_printf(buf, "%s: \n", prog->func_decl->name);

	if (prog->func_decl->stmt == nullptr) fatal("No statement to generate in function [%s]", prog->func_decl->name);
	gen_stmt();
}

void gen_prog() {
	if (prog->func_decl == nullptr) fatal("No function declaration to generate");
	gen_func_decl();
}

void code_gen() {
	if (prog == nullptr) fatal("Nothing to generate");
	
	buf = (char*)xmalloc(1024, "Can't allocate buffer for generated code");
	buf_cap = 1024;

	gen_prog();

	printf("%s", buf);
}