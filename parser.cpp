uint16_t space_count_old = 0;
uint16_t space_count_new = 0;

Program* prog = nullptr;

const char* parse_name() {
	const char* name = nullptr;
	if (is_kind(TOKEN_NAME)) {
		name = token.name;
		consume_token();
	} else fatal("INVALID NAME AT LINE [%d], POSITION [%d].", src_line, (size_t)((uintptr_t)stream - (uintptr_t)line_start + 1));
	return name;
}

void parse_spaces() {
	space_count_old = space_count_new;	// save previous number 
	space_count_new = 0;
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

void while_spaces() {
	while (is_kind(TOKEN_SPACE) || is_kind(TOKEN_TAB)) {
		consume_token();
	}
}

Expression* parse_expr() {
	const char* expr_start = token.start;
	Expression* expr = expression();
	switch (token.kind) {
	case TOKEN_INT:
	case TOKEN_BIN:
	case TOKEN_OCT:
	case TOKEN_HEX:
		expr->int_val = token.int_val;
		expr->kind = INT;
		consume_token();
		break;
	case TOKEN_FLOAT:
		expr->float_val = token.float_val;
		expr->kind = FLOAT;
		consume_token();
		break;
	case TOKEN_STR:
		fatal("STRING EXPRESSION ISN'T ALLOWED. LINE [%d], POSITION [%d].", src_line, (size_t)((uintptr_t)expr_start - (uintptr_t)line_start + 1));
		//expr->str_val = token.str_val;
		//consume_token();
		break;
	default:
		fatal("INVALID EXPRESSION AT LINE [%d], POSITION [%d].", src_line, (size_t)((uintptr_t)expr_start - (uintptr_t)line_start + 1));
	}
	return expr;
}

Statement* parse_stmt() {
	parse_spaces();
	if (space_count_new <= space_count_old) fatal("OUT OF SCOPE AT LINE [%d], POSITION [%d]", src_line, (size_t)((uintptr_t)stream - (uintptr_t)line_start + 1));
	expected_keyword(KEYWORD_RET);
	while_spaces();
	Expression* expr = parse_expr();
	return statement(expr);
}

FuncDecl* parse_func_decl() {
	parse_spaces();
	expected_keyword(KEYWORD_DEF);
	while_spaces();
	const char* name = parse_name();
	while_spaces();
	expected_token(TOKEN_LPAREN);
	while_spaces();
	expected_token(TOKEN_RPAREN);
	while_spaces();
	expected_token(TOKEN_COLON);
	while_spaces();
	expected_token(TOKEN_NEW_LINE);
	Statement* statement = parse_stmt();
	while_spaces();
	if (!is_kind(TOKEN_EOF)) fatal("UNEXPECTED TOKEN INSTEAD EOF AT LINE [%d], POSITION [%d].", src_line, (size_t)((uintptr_t)stream - (uintptr_t)line_start + 1));
	return func_decl(name, statement);
}

Program* parse_prog() {
	FuncDecl* func_decl = parse_func_decl();
	//if (!func_decl) {
	//	fatal("OUT OF SCOPE AT LINE [%d], POSITION [%d]", src_line, (size_t)((uintptr_t)stream - (uintptr_t)line_start + 1));
	//}
	return program(func_decl);
}

void parse_file() {
	prog = parse_prog();
}