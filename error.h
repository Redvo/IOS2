/***********************************************
*	IOS Project 2 - 8.4.2014
*	File: error.h
*	Author: Lukáš Rendvanský, FIT
*	E-mail: xrendv00@stud.fit.vutbr.cz
*	Compiled with: gcc 4.7.2-1
*	Declaration of functions used in file error.c
************************************************/

/**
 *	Function Warning 
 *
 *	void Warning(const char *fmt, ...);
 *
 *	@brief	Function printf formated warning message.
 *	@param	stream	pointer to output stream
 *	@param	ftm	Pointer to char array containing
 *				format of message. 
 *	@post	Prints error message and program continues.
 */

#ifndef ERROR_H_Warning
#define ERROR_H_Warning

	void Warning(FILE *stream, const char *fmt, ...);

#endif /* ERROR_H_Warning */

/**
 *	Function ExitMessage 
 *
 *	void ExitMessage(const char *fmt, ...);
 *
 *	@brief	Function printf formated exit message.
 *	@param	stream	pointer to output stream
 *	@param	ftm	Pointer to char array containing
 *				format of message. 
 *	@post	Prints message and program closes
 *				with exit code exit(0).
 */

#ifndef ERROR_H_ExitMessage
#define ERROR_H_ExitMessage

	void ExitMessage(FILE *stream, const char *fmt, ...);

#endif /* ERROR_H_ExitMessage */

/**
 *	Function InputError 
 *
 *	void InputError(const char *fmt, ...);
 *
 *	@brief	Function print formated error message.
 *	@param	stream	pointer to output stream
 *	@param	ftm	Pointer to char array containing
 *				format of message. 
 *	@post	Prints error message and program closes
 *				with exit code exit(1).
 */

#ifndef ERROR_H_InputError
#define ERROR_H_InputError

	void InputError(FILE *stream, const char *fmt, ...);

#endif /* ERROR_H_InputError */

/**
 *	Function SyscallError 
 *
 *	void SyscallError(const char *fmt, ...);
 *
 *	@brief	Function printf formated error message.
 *	@param	stream	pointer to output stream
 *	@param	ftm	Pointer to char arry containing
 *				format of message.
 *	@post	Prints error message and program closes
 *				with exit code exit(2).
 */

#ifndef ERROR_H_SyscallError
#define ERROR_H_SyscallError

	void SyscallError(FILE *stream, const char *fmt, ...);
	
#endif /* ERROR_H_SyscallError */

/*  End of file error.h */
