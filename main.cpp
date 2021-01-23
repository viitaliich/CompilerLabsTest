#pragma once
#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <fstream>
#include <vector>
#include <cstdlib>
#include <cassert>
#include <cstdarg>

#include "common.cpp"
#include "ast.h"
#include "ast.cpp"
#include "lexer.cpp"
#include "parser.cpp"
#include "gen.cpp"


char* read_file(const char* path) {
	size_t path_len = strlen(path);
	if (path[path_len - 1] != 'y') {		// TO DO: new extension checking mechanism
		fatal("Source file extension error.");
	}
	
	FILE* file = fopen(path, "rb");
	if (!file) return NULL;
	
	// file size detection
	fseek(file, 0, SEEK_END);	// set cursor at the end of the file.
	size_t size = ftell(file);	// number of bytes from beginning to current cursor position
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

bool write_file(const char* path, const char* buf, size_t len) {
	FILE* file = fopen(path, "w");		// not "wb"
	if (!file) {
		return false;
	}
	size_t n = fwrite(buf, len, 1, file);
	fclose(file);
	return n == 1;
}

const char* get_ext(const char* path) {
	const char* ext = NULL;
	for (; *path; path++) {
		if (*path == '.') {
			ext = path + 1;
		}
	}
	return ext;
}

char* replace_ext(const char* path, const char* new_ext) {
	const char* ext = get_ext(path);
	if (!ext) {
		return NULL;
	}
	size_t base_len = ext - path;
	size_t new_ext_len = strlen(new_ext);
	size_t new_path_len = base_len + new_ext_len;
	char* new_path = (char*)xmalloc(new_path_len + 1, "ERROR");		// +1 for the string terminator
	memcpy(new_path, path, base_len);
	memcpy(new_path + base_len, new_ext, new_ext_len);
	new_path[new_path_len] = 0;
	return new_path;
}

bool compile_py_file(const char* path){
	const char* str = read_file(path);
	if (!str) return false;
	
	init_stream(str);
	parse_file();
	code_gen();

	const char* asm_code = buf;
	const char* asm_path = replace_ext(path, "asm");
	if (!asm_path) {
		return false;
	}
	if (!write_file(asm_path, asm_code, strlen(asm_code))) {
		return false;
	}
	return true;
}

void clear_buf() {
	free(buf);
	buf_cap = 0;
	buf_len = 0;
}

bool gen_bat_file(const char* path) {
	clear_buf();
	buf_cap = 2048;		// TO DO: #define 2048	???
	buf = (char*)xmalloc(buf_cap, "Can't allocate space for char buffer");
	buf = buf_printf(buf, \
"c:\\masm32\\bin\\ml /c /Zd /coff source.asm\n\
c:\\masm32\\bin\\Link /SUBSYSTEM:CONSOLE source.obj\n\
echo RESULT BEGIN...\n\
source.exe\n\
echo ...RESULT END\n\
PAUSE\n");

	const char* bat_code = buf;
	const char* bat_path = path;
	if (!bat_path) {
		return false;
	}
	if (!write_file(bat_path, bat_code, strlen(bat_code))) {
		return false;
	}
	return true;
}

void test_asm_code() {
	int a;

	__asm {
		//main:
		mov eax, 55
		//ret

		mov a, eax
	}
	printf("%d\n", a);
	//cout << a << endl;

	system("PAUSE");
}

int main(int argc, char* argv) {

	// TO DO: manual path input
	const char* path = "source.py";			// path to source code
	if (!compile_py_file(path)) {
		printf("Compilation failed.\n");
		return 1;
	}
	printf("Compilation succeeded.\n");
	// TO DO: auto generation pathes
	if (!gen_bat_file("source.bat")) {
		printf("BAT file generation failed.\n");
		return 1;
	}
	system("source.bat");

	// TEST ASM COSE
	//test_asm_code();
	// TEST ASM CODE

	return 0;
}