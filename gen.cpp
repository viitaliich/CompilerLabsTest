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
	buf = buf_printf(buf, "\tmov ebx, %d\n", prog->func_decl->stmt->expr->int_val);
}

void gen_float_exp() {
	prog->func_decl->stmt->expr->int_val = (int)prog->func_decl->stmt->expr->float_val;			// float to int
	buf = buf_printf(buf, "\tmov ebx, %d\n", prog->func_decl->stmt->expr->int_val);
}

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
	buf = buf_printf(buf, "\tret\n\nmain ENDP\n\n");
}

void gen_stmt() {
	if (prog->func_decl->stmt->kind == RET_STMT) {
		gen_ret_exp();
	}
	else {
		fatal("No expression to generate in function [%s]", prog->func_decl->name);
	}
}

void gen_data() {
	buf = buf_printf(buf, ".data\n\nbuff \t db 11 dup(?)\n\n");
}

void gen_code() {
	buf = buf_printf(buf, ".code\n\nstart:\n\n\tinvoke %s\n\n\
\tinvoke  NumbToStr, ebx, ADDR buff\n\
\tinvoke  StdOut, eax\n\
\tinvoke  ExitProcess, 0\n\n", prog->func_decl->name);
}

void gen_NumbToStr(){
	buf = buf_printf(buf, "NumbToStr PROC uses ebx x:DWORD,buffer:DWORD\n\n\
\tmov\tecx, buffer\n\
\tmov\teax, x\n\
\tmov\tebx, 10\n\
\tadd\tecx, ebx\n\
@@:\n\
\txor\tedx, edx\n\
\tdiv\tebx\n\
\tadd\tedx, 48\n\
\tmov\tBYTE PTR[ecx], dl\n\
\tdec\tecx\n\
\ttest\teax, eax\n\
\tjnz\t@b\n\
\tinc\tecx\n\
\tmov\teax, ecx\n\
\tret\n\
NumbToStr\tENDP\n\n");

	buf = buf_printf(buf, "END\tstart\n");

}

void gen_func_decl() {
	if (prog->func_decl->name == nullptr) fatal("No function name to generate");
	buf = buf_printf(buf, "NumbToStr\tPROTO :DWORD,:DWORD\n\
%s\tPROTO\n\n", prog->func_decl->name);
	gen_data();
	gen_code();

	buf = buf_printf(buf, "%s PROC\n", prog->func_decl->name);

	if (prog->func_decl->stmt == nullptr) fatal("No statement to generate in function [%s]", prog->func_decl->name);
	gen_stmt();

	gen_NumbToStr();
}

void gen_includes() {
	buf = buf_printf(buf, ".386\n.model flat, stdcall\noption casemap : none\n\
include     c:\\masm32\\include\\windows.inc\n\
include     c:\\masm32\\include\\kernel32.inc\n\
include     c:\\masm32\\include\\masm32.inc\n\
includelib  c:\\masm32\\lib\\kernel32.lib\n\
includelib  c:\\masm32\\lib\\masm32.lib\n\n");
}

void gen_prog() {
	gen_includes();
	
	if (prog->func_decl == nullptr) fatal("No function declaration to generate");
	gen_func_decl();
}

void code_gen() {
	buf = (char*)xmalloc(2048, "Can't allocate buffer for generated code");
	buf_cap = 2048;

	if (prog == nullptr) fatal("Nothing to generate");
	gen_prog();

	printf("%s", buf);
}