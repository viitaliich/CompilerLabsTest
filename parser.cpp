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
	// TO DO: could be errors in count methodology. Number of new lines (\n) is not taking into account
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

Expression* parse_expr();

Expression* parse_factor() {
	const char* expr_start = token.start;		// for error place detection
	Expression* expr = expression();
	switch (token.kind) {
	// TO DO: check correctness on code generation stage of TOKEN_LPAREN
	case TOKEN_LPAREN: {
		consume_token();
		while_spaces();
		expr = parse_expr();
		if (token.kind != TOKEN_RPAREN) 
			fatal("NO CLOSE PARENTHESE AT LINE [%d], POSITION [%d].", src_line, (size_t)((uintptr_t)stream - (uintptr_t)line_start + 1));
		consume_token();
		while_spaces();		// do we need this here?
		break;
	}
	case TOKEN_COMP: {
		expr->kind = EXP_UN_COMP;
		consume_token();
		while_spaces();
		expr->exp_right = parse_factor();
		break;
	}
	case TOKEN_NEG: {
		expr->kind = EXP_UN_NEG;
		consume_token();
		while_spaces();
		expr->exp_right = parse_factor();
		break;
	}
	case TOKEN_INT:
	case TOKEN_BIN:
	case TOKEN_OCT:
	case TOKEN_HEX:
		expr->int_val = token.int_val;
		expr->kind = EXP_INT;
		consume_token();		// do we need this here, this is the end ???
		break;
	case TOKEN_FLOAT:
		expr->float_val = token.float_val;
		expr->kind = EXP_FLOAT;
		consume_token();		// do we need this here, this is the end ???
		break;
	case TOKEN_STR:
		fatal("STRING EXPRESSION ISN'T ALLOWED. LINE [%d], POSITION [%d].", src_line, (size_t)((uintptr_t)expr_start - (uintptr_t)line_start + 1));
		break;
	default:
		fatal("INVALID EXPRESSION AT LINE [%d], POSITION [%d].", src_line, (size_t)((uintptr_t)expr_start - (uintptr_t)line_start + 1));
	}

	return expr;
}

Expression* parse_term() {
	Expression* expr_left = parse_factor();
	while_spaces();
	while (token.kind == TOKEN_MUL || token.kind == TOKEN_DIV) {
		Expression* expr = expression();
		if (token.kind == TOKEN_MUL) {
			expr->kind = EXP_BIN_MUL;
		}
		else expr->kind = EXP_BIN_DIV;
		consume_token();
		while_spaces();
		expr->exp_left = expr_left;
		expr->exp_right = parse_term();
		expr_left = expr;
	}
	return expr_left;
}

Expression* parse_notop() {
	Expression* expr_left = parse_term();
	while_spaces();
	while (token.kind == TOKEN_ADD || token.kind == TOKEN_NEG) {
		Expression* expr = expression();
		if (token.kind == TOKEN_ADD) {
			expr->kind = EXP_BIN_ADD;
		} else expr->kind = EXP_BIN_NEG;
		consume_token();
		while_spaces();
		expr->exp_left = expr_left;
		expr->exp_right = parse_term();
		expr_left = expr;
	}
	return expr_left;
}

Expression* parse_expr() {
	Expression* expr = expression();
	if (token.kind == TOKEN_KEYWORD && token.mod == KEYWORD_NOT) {
		expr->kind = EXP_UN_LOGNEG;
		consume_token();
		while_spaces();			
		expr->exp_right = parse_expr();
	}
	else {
		expr = parse_notop();		// could be errors (not shure) ???
		while_spaces();
	}
	return expr;
}

Statement* parse_stmt() {
	parse_spaces();
	if (space_count_new <= space_count_old) fatal("OUT OF SCOPE AT LINE [%d], POSITION [%d]", src_line, (size_t)((uintptr_t)stream - (uintptr_t)line_start + 1));
	expected_keyword(KEYWORD_RET);		
	while_spaces();
	Expression* expr = parse_expr();
	while_spaces();
	return statement(expr);				// TO DO: pay atencion, do like in expression parsing
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
	return program(func_decl);
}

void parse_file() {
	// "prog" is not declared here because this variable is used in "code generation" stage
	prog = parse_prog();
}