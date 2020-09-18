const char* first_keyword;
const char* last_keyword;

typedef enum KeywordMod {
	KEYWORD_DEF,
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
	TOKEN_CHAR,
	TOKEN_STR,
	TOKEN_EOF,
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

Token token;			// global token, corresponds to the current token.
const char* stream;		// global variable responsible for source string
const char* line_start;
int src_line;

void scan_str() {
	const char close_quote = *stream;
	stream++;
	char* str = NULL;
	while (*stream && *stream != close_quote) {
		char val = *stream;
		if (val == '\n') {
			fatal("STR TOKEN [%s] AT LINE [%d], POSITION [%d] CAN'T CONTAIN NEWLINE SYM.", token.start, src_line, (size_t)(stream - line_start));
		}
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
				fatal("INVALID ESCAPE SYMBOL [\\%c] AT LINE [%d], POSITION [%d]", *stream, src_line, (size_t)(stream - line_start));
			}
		}
		strncat(str, (const char*) val, 1);
		stream++;
		}
	if(*stream == close_quote){
		stream++;
	}
	else {
		fatal("UNEXPECTED END OF STRING [%c] AT LINE [%d], POSITION [%d]", *stream, src_line, (size_t)(stream - line_start));
	}
	strncat(str, (const char*)0, 1);
	token.kind = TOKEN_STR;
	token.str_val = str;
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
		}
		else if (tolower(*stream) == 'o') {
			base = 8;
			token.kind = TOKEN_OCT;
			stream++;
		}
		else if (tolower(*stream) == 'x') {
			base = 16;
			token.kind = TOKEN_HEX;
			stream++;
		}
		else {
			fatal("UNEXPECTED SYMBOL [%c] AT LINE [%d], POSITION [%d]", *stream, src_line, (size_t)(stream - line_start));
		}
	}
	int value = 0;
	while (isdigit(*stream)) {
		if (*stream - '0' > base) {
			fatal("DIGIT [%c] AT LINE [%d], POSITION [%d] OUT OF RANGE FOR BASE [%d].", *stream, src_line, (size_t)(stream - line_start), base);
		}
		value = value * base + *stream - '0';
		stream++;
	}
	token.int_val = value;
}

bool is_keyword(const char* name) {
	if (first_keyword <= name && name <= last_keyword){
		switch (uintptr_t(name) - uintptr_t(first_keyword)) {
		case KEYWORD_DEF:
			token.mod = KEYWORD_DEF;
			break;
		case KEYWORD_RET:
			token.mod = KEYWORD_RET;
			break;
		}
	}
}

void consume_token() {
repeat:
	token.start = stream;
	switch (*stream) {
	case '\n': {
		stream++;
		line_start = stream;
		src_line++;
		goto repeat;
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
		token.kind = is_keyword(token.name) ? TOKEN_KEYWORD : TOKEN_NAME;
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

	case '.': {	// float numbers can start with point
		if (isdigit(stream[1])) {
			scan_float();
			stream++;
		}
		else {
			// ...
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

	case '\0': {
		token.kind = TOKEN_EOF;
		stream++;
		break;
	}
	default: {
		fatal("INVALID TOKEN [%c] AT LINE [%d], POSITION [%d].", *stream, src_line, (size_t)(stream - line_start));
	}
	}
	token.end = stream;
}

typedef std::vector<const char*> Keywords;
Keywords keywords;

const char* keyword(const char* str) {
	const char* keyword = str_intern(str);
	keywords.push_back(keyword);
	return keyword;
}

void init_keywords() {
	static bool inited;
	if (inited) return;
	
	first_keyword = keyword("def");
	char* pull_end = str_pull.end;	// for future checks. All keywords must be in the same block.
	last_keyword = keyword("return");
	assert(str_pull.end == pull_end);
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

bool expected_keyword(KeywordMod mod, bool error) {
	if (is_kind(TOKEN_KEYWORD) && is_mod(mod)) {
		consume_token();
		return true;
	}
	else {
		if (error) fatal("***");
		else return false;
	}
}

bool expected_token(TokenKind kind, bool error) {
	if (is_kind(kind)) {
		consume_token();
		return true;
	}
	else {
		if (error) fatal("***");
		else return false;
	}
}

