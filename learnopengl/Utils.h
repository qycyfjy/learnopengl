#pragma once

#include <cstdio>
#include <cstdlib>
#include <string>

inline std::string readfile(const char* fname) {
	FILE* f;
	fopen_s(&f, fname, "r");
	if (f == nullptr) {
		return "";
	}
	fseek(f, 0, SEEK_END);
	long fsize = ftell(f);
	fseek(f, 0, SEEK_SET);

	std::string s(fsize, 0);
	fread(&s[0], fsize, 1, f);
	fclose(f);

	return s;
}