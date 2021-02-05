uint16_t space_count_old = 0;
uint16_t space_count_new = 0;

typedef enum {
	BLOCK_RIGHT,
	BLOCK_CENTRAL,
	BLOCK_LEFT,
	BLOCK_TERMINATE,
} SpacesBlock;

SpacesBlock spaces_block;

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
	// BUG: no empty lines between statements allowed
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
	case TOKEN_LPAREN: {
		consume_token();
		while_spaces();
		expr = parse_expr();
		if (token.kind != TOKEN_RPAREN) 
			fatal("NO CLOSE PARENTHESE AT LINE [%d], POSITION [%d].", src_line, (size_t)((uintptr_t)stream - (uintptr_t)line_start + 1));
		consume_token();
		while_spaces();
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
	case TOKEN_NAME: {
		expr->var = token.name;
		expr->kind = EXP_VAR;
		consume_token();
		while_spaces();
		break;
	}
	case TOKEN_INT:
	case TOKEN_BIN:
	case TOKEN_OCT:
	case TOKEN_HEX:
		expr->int_val = token.int_val;
		expr->kind = EXP_INT;
		consume_token();		
		break;
	case TOKEN_FLOAT:
		expr->float_val = token.float_val;
		expr->kind = EXP_FLOAT;
		consume_token();		
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
	while (token.kind == TOKEN_MUL || token.kind == TOKEN_DIV || token.kind == TOKEN_MOD) {
		Expression* expr = expression();
		if (token.kind == TOKEN_MUL) {
			expr->kind = EXP_BIN_MUL;
		}
		else if (token.kind == TOKEN_DIV) {
			expr->kind = EXP_BIN_DIV;
		}
		else expr->kind = EXP_BIN_MOD;
		consume_token();
		while_spaces();
		expr->exp_left = expr_left;
		expr->exp_right = parse_term();
		expr_left = expr;
	}
	return expr_left;
}

Expression* parse_add() {
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

Expression* parse_shift() {
	Expression* expr_left = parse_add();
	while_spaces();
	while (token.kind == TOKEN_SH_LEFT || token.kind == TOKEN_SH_RIGHT) {
		Expression* expr = expression();
		if (token.kind == TOKEN_SH_LEFT) {
			expr->kind = EXP_BIN_SH_LEFT;
		}
		else expr->kind = EXP_BIN_SH_RIGHT;
		consume_token();
		while_spaces();
		expr->exp_left = expr_left;
		expr->exp_right = parse_add();
		expr_left = expr;
	}
	return expr_left;
}

Expression* parse_bitand() {
	Expression* expr_left = parse_shift();
	while_spaces();
	while (token.kind == TOKEN_AND) {
		Expression* expr = expression();
		expr->kind = EXP_BIN_BIT_AND;
		consume_token();
		while_spaces();
		expr->exp_left = expr_left;
		expr->exp_right = parse_shift();
		expr_left = expr;
	}
	return expr_left;
}

Expression* parse_xor() {
	Expression* expr_left = parse_bitand();
	while_spaces();
	while (token.kind == TOKEN_XOR) {
		Expression* expr = expression();
		expr->kind = EXP_BIN_XOR;
		consume_token();
		while_spaces();
		expr->exp_left = expr_left;
		expr->exp_right = parse_bitand();
		expr_left = expr;
	}
	return expr_left;
}


Expression* parse_bitor() {
	Expression* expr_left = parse_xor();
	while_spaces();
	while (token.kind == TOKEN_OR) {
		Expression* expr = expression();
		expr->kind = EXP_BIN_BIT_OR;
		consume_token();
		while_spaces();
		expr->exp_left = expr_left;
		expr->exp_right = parse_xor();
		expr_left = expr;
	}
	return expr_left;
}

Expression* parse_equals() {
	Expression* expr_left = parse_bitor();
	while_spaces();
	while (token.kind == TOKEN_EQL || token.kind == TOKEN_NEQL
			|| token.kind == TOKEN_LESS || token.kind == TOKEN_LESS_EQL
			|| token.kind == TOKEN_GREATER || token.kind == TOKEN_GREATER_EQL) {
		Expression* expr = expression();
		if (token.kind == TOKEN_EQL) {
			expr->kind = EXP_BIN_EQL;
		}
		else if (token.kind == TOKEN_NEQL) {
			expr->kind = EXP_BIN_NEQL;
		}
		else if (token.kind == TOKEN_LESS) {
			expr->kind = EXP_BIN_LESS;
		}
		else if (token.kind == TOKEN_LESS_EQL) {
			expr->kind = EXP_BIN_LESS_EQL;
		}
		else if (token.kind == TOKEN_GREATER) {
			expr->kind = EXP_BIN_GREATER;
		}
		else expr->kind = EXP_BIN_GREATER_EQL;
		consume_token();
		while_spaces();
		expr->exp_left = expr_left;
		expr->exp_right = parse_bitor();
		expr_left = expr;
	}
	return expr_left;
}

// Could be errors ???
Expression* parse_assign() {
	if (token.kind == TOKEN_NAME) {
		Expression* expr = expression();
		
		TempToken* temp_token = temp_token_crt(token, stream);
		consume_token();
		while_spaces();
		if (token.kind == TOKEN_ASSIGN) {
			expr->kind = EXP_ASSIGN;
			expr->exp_left = expression();
			expr->exp_left->var = temp_token->token.name;
			consume_token();
			while_spaces();
			expr->exp_right = parse_expr();
		}
		else {
			stream = temp_token->stream;
			token = temp_token->token;
			expr = parse_equals();
		}
		return expr;
	}
	else return parse_equals();
}

Expression* parse_notop() {
	Expression* expr = expression();
	if (token.kind == TOKEN_KEYWORD && token.mod == KEYWORD_NOT) {
		expr->kind = EXP_UN_LOGNEG;
		consume_token();
		while_spaces();			
		expr->exp_right = parse_expr();
	}
	else {
		expr = parse_assign();		
		while_spaces();	
	}
	return expr;
}

Expression* parse_logand() {
	Expression* expr_left = parse_notop();
	while_spaces();
	while (token.kind == TOKEN_KEYWORD && token.mod == KEYWORD_AND) {
		Expression* expr = expression();
		expr->kind = EXP_BIN_AND;
		consume_token();
		while_spaces();
		expr->exp_left = expr_left;
		expr->exp_right = parse_notop();
		expr_left = expr;
	}
	return expr_left;
}

Expression* parse_expr() {		// second name - "parse_logor"
	Expression* expr_left = parse_logand();
	while_spaces();
	while (token.kind == TOKEN_KEYWORD && token.mod == KEYWORD_OR) {
		Expression* expr = expression();
		expr->kind = EXP_BIN_OR;
		consume_token();
		while_spaces();
		expr->exp_left = expr_left;
		expr->exp_right = parse_logand();
		expr_left = expr;
		while_spaces();	
	}
	return expr_left;
}

void set_block() {
	if (space_count_new > space_count_old) {
		spaces_block = BLOCK_RIGHT;
	} 
	else if (space_count_new == space_count_old) {
		spaces_block = BLOCK_CENTRAL;
	}
	else if (space_count_new < space_count_old) {
		spaces_block = BLOCK_LEFT;
	}
	else fatal("CAN'T SET BLOCK");
}

void change_block() {
	// BUG: new line is empty, block changes anyway
	if (token.kind == TOKEN_NEW_LINE) {
		consume_token();
		parse_spaces();
		set_block();
	}
	else if (token.kind == TOKEN_EOF) {
		spaces_block = BLOCK_LEFT;
	}
	else fatal("ERROR");		// TO DO: make it more specific
}

Statement* parse_stmt() {
	Statement* stmt = statement();
	// ???
	//set_block();
	// TO DO: make changes
	//if ((spaces_block == BLOCK_RIGHT && space_count_new <= space_count_old) ||
	//	(spaces_block == BLOCK_CENTRAL && space_count_new != space_count_old)) 
	//	fatal("OUT OF SCOPE AT LINE [%d], POSITION [%d]", src_line, (size_t)((uintptr_t)stream - (uintptr_t)line_start + 1));
	if (token.kind == TOKEN_KEYWORD && token.mod == KEYWORD_RET) {
		stmt->kind = STMT_RET;
		consume_token();
		while_spaces();
		Expression* expr = parse_expr();
		stmt->expr = expr;
		while_spaces();
	}
	else if (token.kind == TOKEN_KEYWORD && token.mod == KEYWORD_IF) {
		stmt->kind = STMT_IF;
		consume_token();
		while_spaces();
		stmt->expr = parse_expr();
		//Expression* expr = parse_expr();
		while_spaces();
		expected_token(TOKEN_COLON);
		while_spaces();
		expected_token(TOKEN_NEW_LINE);
		parse_spaces();
		set_block();
		if (spaces_block == BLOCK_RIGHT) {
			//Statement* statement = parse_stmt();
			stmt->stmt = parse_stmt();
			//statement_queue.push(statement);
			statement_queue.push(stmt->stmt);
		}
		else fatal("OUT OF SCOPE AT LINE [%d], POSITION [%d]", src_line, (size_t)((uintptr_t)stream - (uintptr_t)line_start + 1));
		
		while (spaces_block == BLOCK_CENTRAL) {
			//Statement* statement = parse_stmt();
			stmt->stmt = parse_stmt();
			//statement_queue.push(statement);
			statement_queue.push(stmt->stmt);
		}
		while_spaces();
		//change_block();
		spaces_block = BLOCK_CENTRAL;
	}
	else {
		stmt->kind = STMT_EXP;
		Expression* expr = parse_expr();
		stmt->expr = expr;
		while_spaces();
		change_block();
	}

	if (token.kind == TOKEN_EOF) {
		spaces_block = BLOCK_TERMINATE;
	}
	
	// BUG: new line is empty, block changes anyway
	//if (token.kind == TOKEN_NEW_LINE) {
	//	consume_token();
	//	parse_spaces();
	//	set_block();
	//} 
	//else if (token.kind == TOKEN_EOF) {
	//	spaces_block = BLOCK_LEFT;
	//}
	//else fatal("ERROR");		// TO DO: make it more specific
	

	return stmt;				
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
	// block marker for proper parce of spaces
	//spaces_block = BLOCK_RIGHT;
	parse_spaces();
	set_block();
	if (spaces_block == BLOCK_RIGHT) {
		Statement* statement = parse_stmt();
		statement_queue.push(statement);
	}
	else fatal("OUT OF SCOPE AT LINE [%d], POSITION [%d]", src_line, (size_t)((uintptr_t)stream - (uintptr_t)line_start + 1));
	//spaces_block = BLOCK_CENTRAL;

	//while (token.kind != TOKEN_EOF) {
	while (spaces_block == BLOCK_CENTRAL) {
		//expected_token(TOKEN_NEW_LINE);
		Statement* statement = parse_stmt();
		statement_queue.push(statement);
	}
	while_spaces();
	if (!is_kind(TOKEN_EOF)) fatal("UNEXPECTED TOKEN INSTEAD EOF AT LINE [%d], POSITION [%d].", src_line, (size_t)((uintptr_t)stream - (uintptr_t)line_start + 1));
	return func_decl(name);
}

Program* parse_prog() {
	FuncDecl* func_decl = parse_func_decl();
	return program(func_decl);
}

void parse_file() {
	// "prog" is used in "code generation" stage
	prog = parse_prog();
}