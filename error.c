/***********************************************
*	IOS Project 2 - 8.4.2014
*	File: error.c
*	Author: Lukáš Rendvanský, FIT
*	E-mail: xrendv00@stud.fit.vutbr.cz
*	Compiled with: gcc 4.7.2-1
*	Library for printing warning and error messages.
************************************************/

/*
 *  Including used libraries and files.
 */

#include <stdlib.h> // library for standard functions
#include <stdio.h> // library for input and output
#include <stdarg.h> // library for indefinite number of arguments

// headers with functions description
#include "error.h"

void Warning(FILE *stream, const char *fmt, ...){
    va_list arguments;
    va_start(arguments, fmt);
    fprintf(stream, "Warning: ");
    vfprintf(stream, fmt, arguments);
    fputc('\n',stream);
    va_end(arguments);
    return;
}

void ExitMessage(FILE *stream, const char *fmt, ...){
    va_list arguments;
    va_start(arguments, fmt);
    fprintf(stream,"Closing: ");
    vfprintf(stream, fmt, arguments);
    fputc('\n',stream);
    va_end(arguments);
    exit(0);
}

void InputError(FILE *stream, const char *fmt, ...){
    va_list arguments;
    va_start(arguments, fmt);
    fprintf(stream, "Error: ");
    vfprintf(stream, fmt, arguments);
    fputc('\n',stream);
    va_end(arguments);
    exit(1);
}

void SyscallError(FILE *stream, const char *fmt, ...){
    va_list arguments;
    va_start(arguments, fmt);
    fprintf(stream, "Error: ");
    vfprintf(stream, fmt, arguments);
    fputc('\n',stream);
    va_end(arguments);
    exit(2);
}

/*  End of file error.c */
