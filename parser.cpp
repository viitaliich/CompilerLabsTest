uint16_t space_count_old = 0;		// number of spaces on previous line or statement
uint16_t space_count_new = 0;		// number of spaces on the current line or statement

// scope
typedef struct Block {
	int index;			// index of scope
	int num_spaces;		// number of spaces in the current scope
};
Block block;

std::stack <Block> block_stack;		// stack of blocks

// kinds of possible blocks
typedef enum {
	BLOCK_RIGHT,
	BLOCK_CENTRAL,
	BLOCK_LEFT,
	BLOCK_TERMINATE,	// ??? do we need this?
} SpacesBlock;

SpacesBlock spaces_block;	// ??? do we need this?

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
	while (is_kind(TOKEN_SPACE) || is_kind(TOKEN_TAB)) {		// TO DO: change all "is_kind(...)" on "token.kind == ..."
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

		if (token.kind == TOKEN_LPAREN) {
			expr->kind = EXP_CALL;
			consume_token();
			while_spaces();
			
			expr->args->push_back(parse_expr());

			while_spaces();
			while (token.kind == TOKEN_COMA){
				consume_token();
				while_spaces();
				expr->args->push_back(parse_expr());
				while_spaces();

			}
			expected_token(TOKEN_RPAREN);
			while_spaces();
		}

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

Expression* parse_logor() {
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

Expression* parse_expr() {		// second name - "parse_logor"
	Expression* expr_left = parse_logor();
	while_spaces();
	if (token.kind == TOKEN_KEYWORD && token.mod == KEYWORD_IF) {
		consume_token();
		while_spaces();
		Expression* expr = expression();
		expr->kind = EXP_TERNARY;
		expr->exp_left = expr_left;
		expr->exp_right = parse_expr();
		while_spaces();
		expected_keyword(KEYWORD_ELSE);
		while_spaces();
		expr->exp_else = parse_expr();
		while_spaces();
		expr_left = expr;
	}
	return expr_left;
}

void set_block() {
	if (space_count_new > space_count_old) {		// create new block
		block.index++;
		block.num_spaces = space_count_new;
	} 
	else if (space_count_new == space_count_old) {
		// no changes, do nothing
	}
	else if (space_count_new < space_count_old) {
		while (space_count_new != block_stack.top().num_spaces && !block_stack.empty()) {
			block_stack.pop();
		}
		if (block_stack.empty()) fatal("ERR1");		// ???
		
		block = block_stack.top();
	}
	else fatal("CAN'T SET BLOCK");
}

void change_block() {
	// BUG: new line is empty, block changes anyway
	// could be errors ???

	if (token.kind == TOKEN_NEW_LINE) {
		consume_token();
		parse_spaces();
		set_block();
	}
}

Statement* parse_stmt() {
	Statement* stmt = statement();

	if (token.kind == TOKEN_KEYWORD && token.mod == KEYWORD_RET) {
		stmt->kind = STMT_RET;
		consume_token();
		while_spaces();
		Expression* expr = parse_expr();
		stmt->expr = expr;
		while_spaces();
		change_block();
	}
	else if (token.kind == TOKEN_KEYWORD && token.mod == KEYWORD_IF) {
		stmt->kind = STMT_IF;
		consume_token();
		while_spaces();
		stmt->expr = parse_expr();
		while_spaces();
		expected_token(TOKEN_COLON);
		while_spaces();

		change_block();

		Block blk;

		if (!block_stack.empty() && block.index > block_stack.top().index) {
			block_stack.push(block);
			blk = block;		// save current block for future changes back

			Statement* statement = parse_stmt();
			stmt->stmt_queue->push(statement);
		}
		else fatal("OUT OF SCOPE AT LINE [%d], POSITION [%d]", src_line, (size_t)((uintptr_t)stream - (uintptr_t)line_start + 1));
		
		while (block.index == blk.index && token.kind != TOKEN_EOF && token.mod != KEYWORD_ELSE) {
			Statement* statement = parse_stmt();
			stmt->stmt_queue->push(statement);
		}

		while_spaces();

		change_block();

		if (token.kind == TOKEN_KEYWORD && token.mod == KEYWORD_ELSE) {
			consume_token();
			while_spaces();
			expected_token(TOKEN_COLON);
			while_spaces();
			
			change_block();

			Block blk;

			if (!block_stack.empty() && block.index > block_stack.top().index) {
				block_stack.push(block);

				blk = block;	// save current block for future changes back

				Statement* statement = parse_stmt();
				stmt->stmt_queue_two->push(statement);
			}
			else fatal("OUT OF SCOPE AT LINE [%d], POSITION [%d]", src_line, (size_t)((uintptr_t)stream - (uintptr_t)line_start + 1));

			while (block.index == blk.index && token.kind != TOKEN_EOF) {
				Statement* statement = parse_stmt();
				stmt->stmt_queue_two->push(statement);
			}

			while_spaces();
			change_block();
		}
	}
	
	else {
		stmt->kind = STMT_EXP;
		Expression* expr = parse_expr();
		stmt->expr = expr;
		while_spaces();
		change_block();
	}
	return stmt;				
}

FuncDecl* parse_func_decl() {
	expected_keyword(KEYWORD_DEF);
	while_spaces();
	const char* name = parse_name();
	FuncDecl* f_decl = func_decl(name);
	while_spaces();
	expected_token(TOKEN_LPAREN);
	while_spaces();

	if (token.kind == TOKEN_NAME) {
		f_decl->parameters->push_back(parse_name());
		while_spaces();
		while (token.kind == TOKEN_COMA) {
			consume_token();
			while_spaces();
			f_decl->parameters->push_back(parse_name());
			while_spaces();
		}
	}

	expected_token(TOKEN_RPAREN);
	while_spaces();
	expected_token(TOKEN_COLON);
	while_spaces();

	change_block();

	Block blk;

	if (!block_stack.empty() && block.index > block_stack.top().index) {		// ??? Can be errors
		block_stack.push(block);

		blk = block;

		Statement* statement = parse_stmt();
		f_decl->stmt_queue->push(statement);
	}
	else fatal("OUT OF SCOPE AT LINE [%d], POSITION [%d]", src_line, (size_t)((uintptr_t)stream - (uintptr_t)line_start + 1));

	while (block.index == blk.index && token.kind != TOKEN_EOF) {
		Statement* statement = parse_stmt();
		f_decl->stmt_queue->push(statement);
	}
	while_spaces();
	change_block();

	return f_decl;
}

Program* parse_prog() {
	Program* prog = program();
	// global variable initialization, first block
	// set block, where functions will be
	block = { 0, 0 };		
	if (token.kind == TOKEN_SPACE || token.kind == TOKEN_TAB)
		fatal("FUNCTION DECLARATION MUST BE ON FIRST CURSOR POSITION");
	else block_stack.push(block);

	Block blk;
	blk = block;

	FuncDecl* func_decl = parse_func_decl();
	//prog->func_queue->push(func_decl);
	
	// Check definition of this function	???
	if (!prog->func_queue->empty()) {
		bool found = false;
		for (size_t i = 0; i < prog->func_queue->size(); i++) {
			if (prog->func_queue->at(i)->name == func_decl->name) {
				prog->func_queue->at(i) = func_decl;
				found = true;
			}
			
		}
		if(!found) prog->func_queue->push_back(func_decl);
	}
	else prog->func_queue->push_back(func_decl);
	
	while (block.index == blk.index && token.kind != TOKEN_EOF) {
		FuncDecl* func_decl = parse_func_decl();
		
		bool found = false;
		for (size_t i = 0; i < prog->func_queue->size(); i++) {
			if (prog->func_queue->at(i)->name == func_decl->name) {
				prog->func_queue->at(i) = func_decl;
				found = true;
			}
			else prog->func_queue->push_back(func_decl);
		}
		if (!found) prog->func_queue->push_back(func_decl);

		//prog->func_queue->push(func_decl); ---
		//prog->func_queue->push_back(func_decl); +++
	}
	while_spaces();
	if (!is_kind(TOKEN_EOF)) fatal("UNEXPECTED TOKEN INSTEAD EOF AT LINE [%d], POSITION [%d].", src_line, (size_t)((uintptr_t)stream - (uintptr_t)line_start + 1));

	return prog;
}

void parse_file() {
	// "prog" is used in "code generation" stage
	prog = parse_prog();
}