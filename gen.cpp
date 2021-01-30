char* buf = nullptr;
size_t buf_len = 0;
size_t buf_cap = 0;

char* buf_printf(char* buf, const char* format, ...) {
	va_list args;
	va_start(args, format);
	// ??? may be errors (but may be not)
	if (buf_cap - buf_len < sizeof(args)) {
		(char*)xrealloc(buf, 2 * buf_cap + sizeof(args));
		buf_cap = buf_cap * 2 + sizeof(args);
	}
	int ret = vsprintf(buf + buf_len, format, args);
	buf_len += ret;
	va_end(args);
	return buf;
}

void gen_int_exp(Expression* expr) {
	buf = buf_printf(buf, "\tmov ebx, %d\n", expr->int_val);
}

void gen_float_exp(Expression* expr) {
	expr->int_val = (int)expr->float_val;			// float to int
	buf = buf_printf(buf, "\tmov ebx, %d\n", prog->func_decl->stmt->expr->int_val);
}

void gen_exp(Expression* expr);
void gen_unneg_exp(Expression* expr) {
	gen_exp(expr->exp_right);
	buf = buf_printf(buf, "\tneg ebx\n");
}

void gen_exp(Expression* expr) {
	switch(expr->kind){
	//switch (prog->func_decl->stmt->expr->kind) {
	case EXP_INT: {
		gen_int_exp(expr);
		break;
	}
	case EXP_FLOAT: {
		gen_float_exp(expr);
		break;
	}
	case EXP_UN_NEG: {
		gen_unneg_exp(expr);
		break;
	}
	case EXP_UN_COMP: {

		break;
	}
	case EXP_UN_LOGNEG: {

		break;
	}
	case EXP_BIN_ADD: {

		break;
	}
	case EXP_BIN_NEG: {

		break;
	}
	case EXP_BIN_MUL: {

		break;
	}
	case EXP_BIN_DIV: {

		break;
	}
	default: fatal("Nothing to generate in return expression in function [%s]", prog->func_decl->name);
	}

}

void gen_stmt() {
	if (prog->func_decl->stmt->kind == RET_STMT &&
		prog->func_decl->stmt->expr != nullptr) {
		gen_exp(prog->func_decl->stmt->expr);
		buf = buf_printf(buf, "\tret\n\nmain ENDP\n\n");

	}
	else {
		fatal("No expression to generate in function [%s]", prog->func_decl->name);
	}
}

void gen_data() {
	buf = buf_printf(buf, ".data\n\nbuff \t db 11 dup(?)\n\n");
}

void gen_code() {
	buf = buf_printf(buf, ".code\n\
\nstart:\n\
\n\tinvoke %s\n\n\
\tinvoke  NumbToStr, ebx, ADDR buff\n\
\tinvoke  StdOut, eax\n\
\tinvoke  ExitProcess, 0\n\n", prog->func_decl->name);
}

void gen_NumbToStr(){
	buf = buf_printf(buf, "NumbToStr PROC uses ebx x:DWORD,buffer:DWORD\n\n\
\tmov\tecx, buffer\n\
\tmov\teax, x\n\
\tmov\tebx, 10\n\
\tadd\tecx, ebx\n\n\
\tcmp eax, 0\n\
\tjge lbl\n\
\tneg eax\n\n\
@@:\n\
\txor\tedx, edx\n\
\tdiv\tebx\n\
\tadd\tedx, 48\n\
\tmov\tBYTE PTR[ecx+1], dl\n\
\tdec\tecx\n\
\ttest\teax, eax\n\
\tjnz\t@b\n\
\tmov	BYTE PTR[ecx+1], '-'\n\
\tinc\tecx\n\
\tmov\teax, ecx\n\
\tjmp stp\n\n\
lbl:\n\
\txor\tedx, edx\n\
\tdiv\tebx\n\
\tadd\tedx, 48\n\
\tmov\tBYTE PTR[ecx], dl\n\
\tdec\tecx\n\
\ttest\teax, eax\n\
\tjnz\tlbl\n\
\tinc\tecx\n\
\tmov\teax, ecx\n\
stp:\n\
\tret\n\n\
NumbToStr\tENDP\n\n");

	buf = buf_printf(buf, "END\tstart\n");

}

void gen_func_decl() {
	buf = buf_printf(buf, "%s PROC\n", prog->func_decl->name);
	
	if (prog->func_decl->stmt == nullptr) fatal("No statement to generate in function [%s]", prog->func_decl->name);
	gen_stmt();

	gen_NumbToStr();
}

// TO DO: path generator
void gen_includes() {
	buf = buf_printf(buf, 
		".386\n.model flat, stdcall\n\
option casemap : none\n\
include     c:\\masm32\\include\\windows.inc\n\
include     c:\\masm32\\include\\kernel32.inc\n\
include     c:\\masm32\\include\\masm32.inc\n\
includelib  c:\\masm32\\lib\\kernel32.lib\n\
includelib  c:\\masm32\\lib\\masm32.lib\n\n");
}

void gen_prog() {
	gen_includes();
	
	if (prog->func_decl == nullptr) fatal("No function declaration to generate");

	if (prog->func_decl->name == nullptr) fatal("No function name to generate");
	buf = buf_printf(buf, "NumbToStr\tPROTO :DWORD,:DWORD\n\
%s\tPROTO\n\n", prog->func_decl->name);
	gen_data();
	gen_code();

	gen_func_decl();
}

void code_gen() {
	buf_cap = 2048;		// TO DO: #define 2048		???
	buf = (char*)xmalloc(buf_cap, "Can't allocate buffer for generated code");
	
	if (prog == nullptr) fatal("Nothing to generate");
	gen_prog();

	printf("%s", buf);
}