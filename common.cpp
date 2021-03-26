#define MAX(a, b) ((a) > (b) ? (a) : (b))
#define PULL_BLOCK_SIZE 1024

// fatal error with following program termination
void fatal(const char* message, ...) {
	va_list args;
	va_start(args, message);
	vprintf(message, args);
	printf("\n");
	va_end(args);
	exit(1);
}

// extended malloc
void* xmalloc(size_t bytes, const char* fail_mes) {
	void* ptr = malloc(bytes);
	if (!ptr) {
		printf("%s xmalloc failed", fail_mes);
		exit(1);
	}
	return ptr;
}

void* xrealloc(void* ptr, size_t num_bytes) {
	ptr = realloc(ptr, num_bytes);
	if (!ptr) {
		perror("xrealloc failed");
		exit(1);
	}
	return ptr;
}

typedef std::vector<char*> Blocks;		// pull of blocks

typedef struct Pull {
	char* ptr;		// start of free mem
	char* end;
}Pull;

Pull str_pull;					// string pull
Blocks str_pull_blocks;			// Blocks are related to Pull

void pull_append(Pull* pull, size_t min_size) {
	size_t size = MAX(PULL_BLOCK_SIZE, min_size);
	pull->ptr = (char*)xmalloc(size, "can't append to memory pull");
	pull->end = pull->ptr + size;
	str_pull_blocks.push_back(pull->ptr);
}

void* pull_alloc(Pull* pull, size_t min_size) {
	if (pull == nullptr || min_size >= (size_t)(pull->end - pull->ptr)) {
		pull_append(pull, min_size);
	}
	void* ptr = pull->ptr;
	pull->ptr += min_size;	// update pointer on free position
	str_pull_blocks.push_back(pull->ptr);
	return ptr;
}

// string interning mechanism
typedef struct InterStr {
	const char* str;
	size_t len;
}InterStr;

typedef std::vector<InterStr> Interns;
Interns interns;

const char* str_intern_range(const char* start, const char* end) {
	size_t len = end - start;
	for (size_t i = 0; i < interns.size(); i++) {
		if (interns[i].len == len && strncmp(interns[i].str, start, len) == 0) {
			return interns[i].str;
		}
	}
	
	char* str = (char*)pull_alloc(&str_pull, len + 1);
	memcpy(str, start, len);
	str[len] = 0;
	
	interns.push_back({ str, len });
	return str;
}

const char* str_intern(const char* str) {
	return str_intern_range(str, str + strlen(str));
}