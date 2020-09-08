#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <fstream>
#include <vector>
#include <cstdlib>
#include <cassert>

#include "lexer.cpp"
#include "common.cpp"





char* read_file(const char* path) {
	size_t path_len = strlen(path);
	if (path[path_len - 1] != 'y') {
		printf("extension error");
		exit(1);
	}
	FILE* file = fopen(path, "rb");
	if (!file) return NULL;
	fseek(file, 0, SEEK_END);	// set cursor at the end of the file.
	long size = ftell(file);	// number of bytes from beginning tj current cursor position
	fseek(file, 0, SEEK_SET);
	char* str = (char*)xmalloc(size + 1, "read_file");
	if (size != 0) {
		if (fread(str, size, 1, file) != 1) {
			fclose(file);
			free(str);
			return NULL;
		}
	}
	fclose(file);
	str[size] = 0;
	return str;
}

bool compile_py_file(const char* path){
	const char* str = read_file(path);
	if (!str) return false;
	
	init_stream(str);
}

int main(int argc, char* argv) {

	const char* path = "source.py";
	if (!compile_py_file(path)) {
		printf("Compilation failed.\n");
		return 1;
	}
	printf("Compilation succeeded.\n");
	return 0;
}