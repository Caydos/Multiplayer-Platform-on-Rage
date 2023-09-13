#ifndef PROCESS_H
#define PROCESS_H

extern char* scan_base;
extern size_t scan_length;

char* ScanPattern(char* begin, intptr_t size, const char* pattern, const char* mask);

#endif // !PROCESS_H