//uint8_t tab_count = 0;
uint16_t space_count = 0;

const char* parse_name() {
	const char* name = token.name;
	expected_token(TOKEN_NAME, true);
	return name;
}

FuncDecl* parse_func_decl() {
	while (expected_token(TOKEN_SPACE, false)) {
		space_count++;
	}
	while (expected_token(TOKEN_TAB, false)) {
		space_count += 4;
	}
	expected_keyword(KEYWORD_DEF, true);
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
		fatal("");
	}
	return program(func_decl);
}

void parse_file() {
	Program* program = parse_prog();
}