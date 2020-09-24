//uint8_t tab_count = 0;
uint16_t space_count_old = 0;
uint16_t space_count_new = 0;

const char* parse_name() {
	const char* name = token.name;
	expected_token(TOKEN_NAME);
	return name;
}

void parse_spaces() {
	space_count_old = space_count_new;	// save previous number 
	while (is_kind(TOKEN_SPACE) || is_kind(TOKEN_TAB)) {
		if (is_kind(TOKEN_SPACE)) {
			space_count_new++;
			consume_token();
		}
		else {
			space_count_new += 4;
			consume_token();
		}
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
	expected_keyword(KEYWORD_RET);
	expected_token(TOKEN_SPACE);
	Expression* expr = parse_expr();
	expr->kind = RET_EXPR;
	return statement(expr);
}

FuncDecl* parse_func_decl() {
	parse_spaces();
	expected_keyword(KEYWORD_DEF);
	expected_token(TOKEN_SPACE);
	const char* name = parse_name();
	expected_token(TOKEN_LPAREN);
	expected_token(TOKEN_RPAREN);
	expected_token(TOKEN_COLON);
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