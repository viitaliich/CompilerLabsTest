const char* first_keyword;
const char* last_keyword;

typedef enum KeywordMod {
	KEYWORD_DEF,
	KEYWORD_NOT,	// logical negation (not)
	KEYWORD_AND,
	KEYWORD_OR,
	KEYWORD_RET,
}KeywordMod;

typedef enum TokenKind {
	TOKEN_SPACE,
	TOKEN_TAB,
	TOKEN_KEYWORD,
	TOKEN_NAME,
	TOKEN_LPAREN,
	TOKEN_RPAREN,
	TOKEN_COLON,
	TOKEN_INT,	// decimal
	TOKEN_BIN,
	TOKEN_OCT,
	TOKEN_HEX,
	TOKEN_FLOAT,
	TOKEN_STR,
	TOKEN_NEW_LINE,
	TOKEN_EOF,

	TOKEN_COMP,		// bitwise complement
	TOKEN_NEG,
	//TOKEN_LOGNEG,		// logical negation (not) is keyword
	TOKEN_ADD,
	TOKEN_MUL,
	TOKEN_DIV,

	TOKEN_EQL,
	TOKEN_NEQL,
	TOKEN_LESS_EQL,
	TOKEN_LESS,
	TOKEN_GREATER_EQL,
	TOKEN_GREATER,
	TOKEN_MOD,
	TOKEN_AND,		// bitwise and, AND is a keyword
	TOKEN_OR,		// bitwise or, OR is a keyword
	TOKEN_XOR,		// bitwise xor
	TOKEN_SH_LEFT,	
	TOKEN_SH_RIGHT,	

	TOKEN_ASSIGN,
}TokenKind;

typedef struct Token {
	TokenKind kind;
	KeywordMod mod;
	const char* start;
	const char* end;
	union {
		int int_val;
		double float_val;
		const char* str_val;
		const char* name;
	};
} Token;

typedef struct TempToken {
	const char* stream;
	Token token;
};

TempToken* temp_token_crt(Token token, const char* stream) {
	TempToken* temp_token = (TempToken*)xmalloc(sizeof(TempToken), "Can't allocate memory for TempToken");
	temp_token->stream = stream;
	temp_token->token.start = token.start;
	temp_token->token.end = token.end;
	temp_token->token.kind = token.kind;
	temp_token->token.mod = token.mod;
	temp_token->token.int_val = token.int_val;
	temp_token->token.float_val = token.float_val;
	temp_token->token.str_val = token.str_val;
	temp_token->token.name = token.name;
	return temp_token;
}

Token token;			// global token, corresponds to the current token.
const char* stream;		// global variable responsible for source string
const char* line_start;
int src_line;

void scan_str() {
	const char close_quote = *stream;
	stream++;
	std::string str;
	while (*stream && *stream != close_quote) {
		char val = *stream;
		// sting can't contain new line symbol
		if (val == '\n') {
			fatal("STR TOKEN [%s] AT LINE [%d], POSITION [%d] CAN'T CONTAIN NEWLINE SYM.", token.start, src_line, (size_t)((uintptr_t)stream - (uintptr_t)line_start + 1));
		}
		// TO DO: fot what is this if previous comment?
		else if (val == '\\'){
			stream++;
			switch (*stream){
			case 'n':
				val = '\n';
				break;
			case 'r':
				val = '\r';
				break;
			case 't':
				val = '\t';
				break;
			case 'v':
				val = '\v';
				break;
			case 'b':
				val = '\b';
				break;
			case 'a':
				val = '\a';
				break;
			case '0':
				val = 0;
				break;
			default:
				fatal("INVALID ESCAPE SYMBOL [\\%c] AT LINE [%d], POSITION [%d]", *stream, src_line, (size_t)((uintptr_t)stream - (uintptr_t)line_start + 1));
			}
		}
		str.push_back(val);
		stream++;
		}
	if(*stream == close_quote){
		stream++;
	}
	else {
		fatal("UNEXPECTED END OF STRING [%c] AT LINE [%d], POSITION [%d]", *stream, src_line, (size_t)((uintptr_t)stream - (uintptr_t)line_start + 1));
	}
	str.push_back(0);
	token.kind = TOKEN_STR;
	token.str_val = str.c_str();
}

void scan_float() {
	const char* start = stream;
	while (isdigit(*stream)) {
		stream++;
	}
	if (*stream == '.') {
		stream++;
	}
	while (isdigit(*stream)) {
		stream++;
	}
	token.float_val = strtod(start, NULL);
	token.kind = TOKEN_FLOAT;
}

void scan_int() {
	uint8_t base = 10;
	token.kind = TOKEN_INT;
	if (*stream == '0') {
		stream++;
		if (tolower(*stream) == 'b') {
			base = 2;
			token.kind = TOKEN_BIN;
			stream++;
			if (*stream == NULL) fatal("[0b] is not allowed");
		}
		else if (tolower(*stream) == 'o') {
			base = 8;
			token.kind = TOKEN_OCT;
			stream++;
			if (*stream == NULL) fatal("[0o] is not allowed");

		}
		else if (tolower(*stream) == 'x') {
			base = 16;
			token.kind = TOKEN_HEX;
			stream++;
			if (*stream == NULL) fatal("[0x] is not allowed");

		}
		/*else {
			fatal("UNEXPECTED SYMBOL [%c] AT LINE [%d], POSITION [%d]. MUST BE [b], [o] or [x]", *stream, src_line, (size_t)((uintptr_t)stream - (uintptr_t)line_start + 1));
		}*/
	}
	int value = 0;
	for (;;) {
		bool flag = false;
		int digit = 0;
		switch (*stream) {
		case '0':
			digit = 0;
			break;
		case '1':
			digit = 1;
			break;
		case '2':
			digit = 2;
			break;
		case '3':
			digit = 3;
			break;
		case '4':
			digit = 4;
			break;
		case '5':
			digit = 5;
			break;
		case '6':
			digit = 6;
			break;
		case '7':
			digit = 7;
			break;
		case '8':
			digit = 8;
			break;
		case '9':
			digit = 9;
			break;
		case 'A':
			digit = 10;
			break;
		case 'B':
			digit = 11;
			break;
		case 'C':
			digit = 12;
			break;
		case 'D':
			digit = 13;
			break;
		case 'E':
			digit = 14;
			break;
		case 'F':
			digit = 15;
			break;
		case 'G': case 'H': case 'I': case 'J': case 'K': case 'L': 
		case 'M': case 'N': case 'O': case 'P': case 'Q': case 'R': 
		case 'S': case 'T': case 'U': case 'V': case 'W': case 'X': case 'Y': case 'Z':
			digit = 16;
			break;
		default:
			flag = true;
			break;
		}
		if (flag) break;
		if (digit >= base) {
			fatal("DIGIT [%c] AT LINE [%d], POSITION [%d] OUT OF RANGE FOR BASE [%d].", *stream, src_line, (size_t)((uintptr_t)stream - (uintptr_t)line_start + 1), base);
		}
		value = value * base + digit;
		stream++;
	}
	token.int_val = value;
}

bool is_keyword(const char* name) {
	if (first_keyword <= name && name <= last_keyword){
		
		if (name == interns[KEYWORD_DEF].str) {
			token.mod = KEYWORD_DEF;
		}
		else if (name == interns[KEYWORD_RET].str) {
			token.mod = KEYWORD_RET;
		}
		else if (name == interns[KEYWORD_NOT].str) {
			token.mod = KEYWORD_NOT;
		}
		else if (name == interns[KEYWORD_AND].str) {
			token.mod = KEYWORD_AND;
		}
		else if (name == interns[KEYWORD_OR].str) {
			token.mod = KEYWORD_OR;
		}
		else fatal("Error in keyword [%s] mod detection", name);

		//switch ((uintptr_t(name) - uintptr_t(first_keyword)) / sizeof(name)) {		// STR ???
		//case KEYWORD_DEF:
		//	token.mod = KEYWORD_DEF;
		//	break;
		//case KEYWORD_RET:
		//	token.mod = KEYWORD_RET;
		//	break;
		//case KEYWORD_NOT:
		//	token.mod = KEYWORD_NOT;
		//	break;
		//case KEYWORD_AND:
		//	token.mod = KEYWORD_AND;
		//	break;
		//case KEYWORD_OR:
		//	token.mod = KEYWORD_OR;
		//	break;
		//}
		return true;
	}
	return false;
}

void consume_token() {
repeat:
	token.start = stream;
	switch (*stream) {
	case '\r': {
		stream++;
		goto repeat;
		break;
	}
	case '\n': {
		token.kind = TOKEN_NEW_LINE;
		stream++;
		line_start = stream;
		src_line++;
		break;
	}
	case ' ': {
		token.kind = TOKEN_SPACE;
		stream++;
		break;
	}
	case '\t': {
		token.kind = TOKEN_TAB;
		stream++;
		break;
	}

	case 'a': case 'b': case 'c': case 'd': case 'e': case 'f': case 'g': case 'h': case 'i': case 'j':
	case 'k': case 'l': case 'm': case 'n': case 'o': case 'p': case 'q': case 'r': case 's': case 't':
	case 'u': case 'v': case 'w': case 'x': case 'y': case 'z':
	case 'A': case 'B': case 'C': case 'D': case 'E': case 'F': case 'G': case 'H': case 'I': case 'J':
	case 'K': case 'L': case 'M': case 'N': case 'O': case 'P': case 'Q': case 'R': case 'S': case 'T':
	case 'U': case 'V': case 'W': case 'X': case 'Y': case 'Z':
	case '_': {
		while (isalnum(*stream) || *stream == '_') {
			stream++;
		}
		token.name = str_intern_range(token.start, stream);
		token.kind = is_keyword(token.name) ? TOKEN_KEYWORD : TOKEN_NAME;		// ??? change is_keyword checking
		break;
	}

	case '(': {
		token.kind = TOKEN_LPAREN;
		stream++;
		break;
	}

	case ')': {
		token.kind = TOKEN_RPAREN;
		stream++;
		break;
	}

	case ':': {
		token.kind = TOKEN_COLON;
		stream++;
		break;
	}

	case '\'': case '"': {
		scan_str();
		break;
	}

	case '.': {						// float numbers can start with point
		if (isdigit(stream[1])) {
			scan_float();
		}
		else {
			fatal("INVALID TOKEN AT LINE [%d], POSITION [%d].", src_line, (size_t)((uintptr_t)stream - (uintptr_t)line_start + 1));
		}
		break;
	}

	case '0': case '1': case '2': case '3': case '4': case '5': case '6': case '7': case '8': case '9': {
		while (isdigit(*stream)) {
			stream++;
		}
		char sym = *stream;
		stream = token.start;
		if (sym == '.') {
			scan_float();
		}
		else {
			scan_int();
		}
		break;
	}

	case '~': {
		token.kind = TOKEN_COMP;
		stream++;
		break;
	}
		
	case '-': {
		token.kind = TOKEN_NEG;
		stream++;
		break;
	}

	case '+': {
		token.kind = TOKEN_ADD;
		stream++;
		break;
	}

	case '*': {
		token.kind = TOKEN_MUL;
		stream++;
		break;
	}

	case '/': {
		token.kind = TOKEN_DIV;
		stream++;
		break;
	}

	case '=': {
		stream++;
		if (*stream == '=') {
			token.kind = TOKEN_EQL;
			stream++;
		}
		else {
			token.kind = TOKEN_ASSIGN;
		}
		break;
	}

	case '!': {
		stream++;
		if (*stream == '=') {
			token.kind = TOKEN_NEQL;
			stream++;
		} else fatal("INVALID TOKEN AT LINE [%d], POSITION [%d].", src_line, (size_t)((uintptr_t)stream - (uintptr_t)line_start + 1));
		break;
	}

	case '<': {
		stream++;
		if (*stream == '=') {
			token.kind = TOKEN_LESS_EQL;
			stream++;
		}
		else if (*stream == '<') {
			token.kind = TOKEN_SH_LEFT;
			stream++;
		}
		else token.kind = TOKEN_LESS;
		break;
	}

	case '>': {
		stream++;
		if (*stream == '=') {
			token.kind = TOKEN_GREATER_EQL;
			stream++;
		}
		else if (*stream == '>') {
			token.kind = TOKEN_SH_RIGHT;
			stream++;
		}
		else token.kind = TOKEN_GREATER;
		break;
	}

	case '%': {
		token.kind = TOKEN_MOD;
		stream++;
		break;
	}

	case '&': {
		token.kind = TOKEN_AND;
		stream++;
		break;
	}

	case '|': {
		token.kind = TOKEN_OR;
		stream++;
		break;
	}

	case '^': {
		token.kind = TOKEN_XOR;
		stream++;
		break;
	}

	case '\0': {
		token.kind = TOKEN_EOF;
		stream++;
		break;
	}
	default: {
		if (sizeof(*stream) == 1) {
			fatal("INVALID TOKEN [%c] AT LINE [%d], POSITION [%d].", *stream, src_line, (size_t)((uintptr_t)stream - (uintptr_t)line_start + 1));
		}
		else {
			fatal("INVALID TOKEN [%s] AT LINE [%d], POSITION [%d].", *stream, src_line, (size_t)((uintptr_t)stream - (uintptr_t)line_start + 1));
		}
	}
	}
	token.end = stream;
}

typedef std::vector<const char*> Keywords;
Keywords keywords;

const char* keyword(const char* str) {
	const char* keyword = str_intern(str);
	keywords.push_back(keyword);		// ??? don't need this
	return keyword;
}

void init_keywords() {
	static bool inited;
	if (inited) return;
	
	// ??? swap lines ???
	
	// SEQUENCE MUST NOT BE COMPROMISED
	first_keyword = keyword("def");

	//char* pull_end = str_pull.end;	// for future checks. All keywords must be in the same block.
	keyword("not");
	keyword("and");
	keyword("or");
	//char* pull_end = str_pull.end;	// for future checks. All keywords must be in the same block.
	last_keyword = keyword("return");
	//assert(str_pull.end == pull_end);
	inited = true;
}

void init_stream(const char* str) {
	init_keywords();

	stream = str;
	src_line = 1;
	line_start = stream;
	consume_token();
}

bool is_kind(TokenKind kind) {
	return token.kind == kind;
}

bool is_mod(KeywordMod mod) {
	return token.mod == mod;
}

bool expected_keyword(KeywordMod mod) {
	if (is_kind(TOKEN_KEYWORD) && is_mod(mod)) {
		consume_token();
		return true;
	}
	else fatal("UNEXPECTED KEYWORD AT LINE [%d], POSITION [%d]", src_line, (size_t)((uintptr_t)stream - (uintptr_t)line_start + 1));
}

bool expected_token(TokenKind kind) {
	if (is_kind(kind)) {
		consume_token();
		return true;
	}
	else {
		switch (token.kind) {
		case TOKEN_KEYWORD:
		case TOKEN_NAME:
			fatal("UNEXPECTED TOKEN AT LINE [%d], POSITION [%d].", *stream, src_line, (size_t)((uintptr_t)stream - (uintptr_t)line_start + 1));
			break;
		default:
			fatal("UNEXPECTED TOKEN AT LINE [%d], POSITION [%d].", src_line, (size_t)((uintptr_t)stream - (uintptr_t)line_start + 1));
		}
	}
}

