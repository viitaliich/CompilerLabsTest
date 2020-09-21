//uint8_t tab_count = 0;
uint16_t space_count_old = 0;
uint16_t space_count_new = 0;

const char* parse_name() {
	const char* name = token.name;
	expected_token(TOKEN_NAME, true);
	return name;
}

void parse_spaces() {
	space_count_old = space_count_new;	// save previous number 
	while (expected_token(TOKEN_SPACE, false) || expected_token(TOKEN_TAB, false)) {
		if (expected_token(TOKEN_SPACE, false)) space_count_new++;
		else space_count_new += 4;
	}
}

Expression* parse_expr() {
	Expression* expr = expression();
	switch (token.kind) {
	case TOKEN_INT:
	case TOKEN_BIN:
	case TOKEN_OCT:
	case TOKEN_HEX:
		expr->int_val = token.int_val;
		break;
	case TOKEN_FLOAT:
		expr->float_val = token.float_val;
		break;
	case TOKEN_STR:
		expr->str_val = token.str_val;
		break;
	default:
		//fatal("INVALID EXPRESSION [%c] AT LINE [%d], POSITION [%d].", *stream, src_line, (size_t)(stream - line_start));
		fatal("E");
	}
	return expr;
}

Statement* parse_stmt() {
	parse_spaces();
	if (space_count_new <= space_count_old) fatal("e4");
	expected_keyword(KEYWORD_RET, true);
	expected_token(TOKEN_SPACE, false);
	Expression* expr = parse_expr();
	expr->kind = RET_EXPR;
	return statement(expr);
}

FuncDecl* parse_func_decl() {
	parse_spaces();
	expected_keyword(KEYWORD_DEF, true);
	expected_token(TOKEN_SPACE, false);
	const char* name = parse_name();
	expected_token(TOKEN_LPAREN, true);
	expected_token(TOKEN_RPAREN, true);
	expected_token(TOKEN_COLON, true);
	Statement* statement = parse_stmt();
	return func_decl(name, statement);
}

Program* parse_prog() {
	FuncDecl* func_decl = parse_func_decl();
	if (!func_decl) {
		fatal("e3");
	}
	return program(func_decl);
}

void parse_file() {
	Program* program = parse_prog();
}