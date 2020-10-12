/*
 * shell.h
 *
 *  Created on: 2020-10-12 Mon 11:35
 *      Author: Kim Junsu
 */

#ifndef __SHELL_H__
#define __SHELL_H__

#define RED_FONT   (char*)"\033[31m"
#define WHITE_FONT (char*)"\033[0m"

// print to stream callback
void print(const char* __restrict str, ...);

// execute callback
int execute(int argc, const char* const* argv);

// completion callback
char** complet(int argc, const char* const* argv);

void initShell(void);

#endif /* SHELL_SHELL_H_ */

