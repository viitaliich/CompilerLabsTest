char* buf = nullptr;
size_t buf_len = 0;
size_t buf_cap = 0;
size_t label_index = 0;
std::queue <size_t> label_indices;

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
	buf = buf_printf(buf, "\tmov ebx, %d\n", expr->int_val);
}

void gen_exp(Expression* expr);
void gen_unneg_exp(Expression* expr) {
	gen_exp(expr->exp_right);
	buf = buf_printf(buf, "\tneg ebx\n");
}

void gen_uncomp_exp(Expression* expr) {
	gen_exp(expr->exp_right);
	buf = buf_printf(buf, "\tnot ebx\n");
}

void gen_unlogneg_exp(Expression* expr) {
	gen_exp(expr->exp_right);
	buf = buf_printf(buf, 
		"\tcmp ebx, 0\n\
\tmov ebx, 0\n\
\tsete bl\n");			// or EBX		???
}

void gen_binadd_exp(Expression* expr) {
	gen_exp(expr->exp_left);
	buf = buf_printf(buf, "\tpush ebx\n");
	gen_exp(expr->exp_right);
	buf = buf_printf(buf, 
		"\tpop ecx\n\
\tadd ebx, ecx\n");
}

void gen_binsub_exp(Expression* expr) {
	gen_exp(expr->exp_right);
	buf = buf_printf(buf, "\tpush ebx\n");
	gen_exp(expr->exp_left);
	buf = buf_printf(buf,
		"\tpop ecx\n\
\tsub ebx, ecx\n");
}

void gen_binmul_exp(Expression* expr) {
	gen_exp(expr->exp_left);
	buf = buf_printf(buf, "\tpush ebx\n");
	gen_exp(expr->exp_right);
	buf = buf_printf(buf,
		"\tpop ecx\n\
\timul ebx, ecx\n");
}

void gen_bindiv_exp(Expression* expr) {
	// TO DO: division on zero warning		???
	gen_exp(expr->exp_left);
	buf = buf_printf(buf, "\tmov eax, ebx\n\
\tcdq\n");
	gen_exp(expr->exp_right);
	buf = buf_printf(buf, "\tmov ecx, ebx\n");
	buf = buf_printf(buf, "\tidiv ecx\n");
	buf = buf_printf(buf, "\tmov ebx, eax\n");
}

void gen_bin_and_exp(Expression* expr) {
	gen_exp(expr->exp_left);
	buf = buf_printf(buf, "\tcmp ebx, 0\n\
\tjne _label%d\n", label_index);
	label_indices.push(label_index);
	label_index++;
	buf = buf_printf(buf, "\tjmp _label%d\n", label_index);
	label_indices.push(label_index);
	label_index++;
	buf = buf_printf(buf, "_label%d:\n", label_indices.front());
	label_indices.pop();
	gen_exp(expr->exp_right);
	buf = buf_printf(buf,
		"\tcmp ebx, 0\n\
\tmov ebx, 0\n\
\tsetne bl\n\
_label%d:\n", label_indices.front());
	label_indices.pop();
}

void gen_bin_or_exp(Expression* expr) {
	gen_exp(expr->exp_left);
	buf = buf_printf(buf, "\tcmp ebx, 0\n\
\tje _label%d\n", label_index);
	label_indices.push(label_index);
	label_index++;
	buf = buf_printf(buf, "\tmov ebx, 1\n\
\tjmp _label%d\n", label_index);
	label_indices.push(label_index);
	label_index++;
	buf = buf_printf(buf, "_label%d:\n", label_indices.front());
	label_indices.pop();
	gen_exp(expr->exp_right);
	buf = buf_printf(buf,
		"\tcmp ebx, 0\n\
\tmov ebx, 0\n\
\tsetne bl\n\
_label%d:\n", label_indices.front());
	label_indices.pop();
}

void gen_bin_eql_exp(Expression* expr) {
	gen_exp(expr->exp_left);
	buf = buf_printf(buf, "\tpush ebx\n");
	gen_exp(expr->exp_right);
	buf = buf_printf(buf,
		"\tpop ecx\n\
\tcmp ecx, ebx\n\
\tmov ebx, 0\n\
\tsete bl\n");
}

void gen_bin_neql_exp(Expression* expr) {
	gen_exp(expr->exp_left);
	buf = buf_printf(buf, "\tpush ebx\n");
	gen_exp(expr->exp_right);
	buf = buf_printf(buf,
		"\tpop ecx\n\
\tcmp ecx, ebx\n\
\tmov ebx, 0\n\
\tsetne bl\n");
}

void gen_bin_less_exp(Expression* expr) {
	gen_exp(expr->exp_left);
	buf = buf_printf(buf, "\tpush ebx\n");
	gen_exp(expr->exp_right);
	buf = buf_printf(buf,
		"\tpop ecx\n\
\tcmp ecx, ebx\n\
\tmov ebx, 0\n\
\tsetl bl\n");
}

void gen_bin_less_eql_exp(Expression* expr) {
	gen_exp(expr->exp_left);
	buf = buf_printf(buf, "\tpush ebx\n");
	gen_exp(expr->exp_right);
	buf = buf_printf(buf,
		"\tpop ecx\n\
\tcmp ecx, ebx\n\
\tmov ebx, 0\n\
\tsetle bl\n");
}

void gen_bin_greater_exp(Expression* expr) {
	gen_exp(expr->exp_left);
	buf = buf_printf(buf, "\tpush ebx\n");
	gen_exp(expr->exp_right);
	buf = buf_printf(buf,
		"\tpop ecx\n\
\tcmp ecx, ebx\n\
\tmov ebx, 0\n\
\tsetg bl\n");
}

void gen_bin_greater_eql_exp(Expression* expr) {
	gen_exp(expr->exp_left);
	buf = buf_printf(buf, "\tpush ebx\n");
	gen_exp(expr->exp_right);
	buf = buf_printf(buf,
		"\tpop ecx\n\
\tcmp ecx, ebx\n\
\tmov ebx, 0\n\
\tsetge bl\n");
}

// TO DO: bug - it gives neg output when one of operandds is negative. SOLVED, but could be more ???
void gen_bin_mod_exp(Expression* expr) {
	gen_exp(expr->exp_left);
	buf = buf_printf(buf, "\tmov eax, ebx\n\
\tcdq\n");
	gen_exp(expr->exp_right);
	buf = buf_printf(buf, "\tmov ecx, ebx\n");
	buf = buf_printf(buf, "\tidiv ecx\n");
	buf = buf_printf(buf, "\tmov ebx, edx\n");
	buf = buf_printf(buf, "\tcmp eax, 0\n\
\tjge _label%d\n", label_index);
	label_indices.push(label_index);
	label_index++;
	buf = buf_printf(buf, "\tneg ebx\n");
	buf = buf_printf(buf, "_label%d:\n", label_indices.front());
	label_indices.pop();
}

void gen_bin_bitand_exp(Expression* expr) {
	gen_exp(expr->exp_left);
	buf = buf_printf(buf, "\tpush ebx\n");
	gen_exp(expr->exp_right);
	buf = buf_printf(buf,
		"\tpop ecx\n\
\tand ebx, ecx\n");
}

void gen_bin_bitor_exp(Expression* expr) {
	gen_exp(expr->exp_left);
	buf = buf_printf(buf, "\tpush ebx\n");
	gen_exp(expr->exp_right);
	buf = buf_printf(buf,
		"\tpop ecx\n\
\tor ebx, ecx\n");
}

void gen_bin_xor_exp(Expression* expr) {
	gen_exp(expr->exp_left);
	buf = buf_printf(buf, "\tpush ebx\n");
	gen_exp(expr->exp_right);
	buf = buf_printf(buf,
		"\tpop ecx\n\
\txor ebx, ecx\n");
}

void gen_bin_shleft_exp(Expression* expr) {
	gen_exp(expr->exp_right);
	buf = buf_printf(buf, "\tpush ebx\n");
	gen_exp(expr->exp_left);
	buf = buf_printf(buf,
		"\tpop ecx\n\
\tshl ebx, cl\n");
}

void gen_bin_shright_exp(Expression* expr) {
	gen_exp(expr->exp_right);
	buf = buf_printf(buf, "\tpush ebx\n");
	gen_exp(expr->exp_left);
	buf = buf_printf(buf,
		"\tpop ecx\n\
\tshr ebx, cl\n");
}


void gen_exp(Expression* expr) {
	switch(expr->kind){
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
		gen_uncomp_exp(expr);
		break;
	}
	case EXP_UN_LOGNEG: {
		gen_unlogneg_exp(expr);
		break;
	}
	case EXP_BIN_ADD: {
		gen_binadd_exp(expr);
		break;
	}
	case EXP_BIN_NEG: {
		gen_binsub_exp(expr);
		break;
	}
	case EXP_BIN_MUL: {
		gen_binmul_exp(expr);
		break;
	}
	case EXP_BIN_DIV: {
		gen_bindiv_exp(expr);
		break;
	}
	case EXP_BIN_AND: {
		gen_bin_and_exp(expr);
		break;
	}
	case EXP_BIN_OR: {
		gen_bin_or_exp(expr);
		break;
	}
	case EXP_BIN_EQL: {
		gen_bin_eql_exp(expr);
		break;
	}
	case EXP_BIN_NEQL: {
		gen_bin_neql_exp(expr);
		break;
	}
	case EXP_BIN_LESS: {
		gen_bin_less_exp(expr);
		break;
	}
	case EXP_BIN_LESS_EQL: {
		gen_bin_less_eql_exp(expr);
		break;
	}
	case EXP_BIN_GREATER: {
		gen_bin_greater_exp(expr);
		break;
	}
	case EXP_BIN_GREATER_EQL: {
		gen_bin_greater_eql_exp(expr);
		break;
	}
	case EXP_BIN_MOD: {
		gen_bin_mod_exp(expr);
		break;
	}
	case EXP_BIN_BIT_AND: {
		gen_bin_bitand_exp(expr);
		break;
	}
	case EXP_BIN_BIT_OR: {
		gen_bin_bitor_exp(expr);
		break;
	}
	case EXP_BIN_XOR: {
		gen_bin_xor_exp(expr);
		break;
	}
	case EXP_BIN_SH_LEFT: {
		gen_bin_shleft_exp(expr);
		break;
	}
	case EXP_BIN_SH_RIGHT: {
		gen_bin_shright_exp(expr);
		break;
	}


	default: fatal("Nothing to generate in return expression in function [%s]", prog->func_decl->name);
	}

}

void gen_stmt(Statement* stmt) {
	if (stmt->kind == STMT_RET &&
		stmt->expr != nullptr) {
		gen_exp(stmt->expr);
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

void gen_stmt_queue() {
	while (!statement_queue.empty()) {
		gen_stmt(statement_queue.front());
		statement_queue.pop();
	}
}

void gen_func_decl() {
	buf = buf_printf(buf, "%s PROC\n", prog->func_decl->name);
	
	if(statement_queue.size() == 0) fatal("No statement to generate in function [%s]", prog->func_decl->name);
	gen_stmt_queue();
	
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