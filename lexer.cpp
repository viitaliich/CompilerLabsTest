

typedef enum TokenKind {
	TOKEN_SPACE,
	TOKEN_TAB,
	TOKEN_KEYWORD,
	TOKEN_NAME,
	TOKEN_LPAREN,
	TOKEN_RPAREN,
	TOKEN_COLON,
	TOKEN_INT,
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
	const char* start;
	const char* end;
	union {
		int int_val;
		double float_val;
		char ch_val;
		const char* str_val;
		const char* name;
	};
} Token;

Token token;			// global token, corresponds to the current token.
const char* stream;		// global variable responsible for source string
const char* line_start;
int src_line;

void scan_tab() {

}

void scan_space() {

}

bool is_keyword(const char* name) {
	return first_keyword <= name && name <= last_keyword;
}

void consume_token() {
repeat:
	token.start = stream;
	switch (*stream) {
	case '\n':
		stream++;
		line_start = stream;
		src_line++;
		goto repeat;
		break;

	case ' ':
		scan_space();
		break;
	case '\t':
		scan_tab();
		break;

	case 'a': case 'b': case 'c': case 'd': case 'e': case 'f': case 'g': case 'h': case 'i': case 'j':
	case 'k': case 'l': case 'm': case 'n': case 'o': case 'p': case 'q': case 'r': case 's': case 't':
	case 'u': case 'v': case 'w': case 'x': case 'y': case 'z':
	case 'A': case 'B': case 'C': case 'D': case 'E': case 'F': case 'G': case 'H': case 'I': case 'J':
	case 'K': case 'L': case 'M': case 'N': case 'O': case 'P': case 'Q': case 'R': case 'S': case 'T':
	case 'U': case 'V': case 'W': case 'X': case 'Y': case 'Z':
	case '_':
		while (isalnum(*stream) || *stream == '_') {
			stream++;
		}
		token.name = str_intern_range(token.start, stream);
		token.kind = is_keyword(token.name) ? TOKEN_KEYWORD : TOKEN_NAME;
		break;

	}
}

typedef std::vector<const char*> Keywords;
Keywords keywords;
const char* first_keyword;
const char* last_keyword;


const char* keyword(const char* str) {
	const char* keyword = str_intern(str);
	keywords.push_back(keyword);
}

void init_keywords() {

	static bool inited;
	if (inited) return;
	
	first_keyword = keyword("def");
	char* pull_end = str_pull->end;	// for future checks. All keywords must be in the same block.
	last_keyword = keyword("return");
	assert(str_pull->end == pull_end);
	inited = true;
}

void init_stream(const char* str) {
	init_keywords();

	stream = str;
	src_line = 1;
	line_start = stream;
	consume_token();

}