/***********************************************
*	IOS Project 2 - 8.4.2014
*	File: strtonum.h
*	Author: Lukáš Rendvanský, FIT
*	E-mail: xrendv00@stud.fit.vutbr.cz
*	Compiled with: gcc 4.7.2-1
*	Header file for strtonum.c.
************************************************/

/**
 *	Function strtonum 
 *
 *	long long
 *	strtonum(const char *numstr, long long minval, long long maxval,
 *		const char **errstrp);
 *
 *	@brief	Function convert string to number.
 *	@param	numstr	Pointer to string representation of number.
 *	@param	minval	Minimal value for successful conversion
 *	@param	maxval	Maximum value for successful conversion 
 *	@param	errstrp	Error message if error occured or NULL
 *	@return	Returns converted number if successful and set
 * 			errstrp to NULL, or returns 0 if error occured
 *			and errstrp contains error message.
 */

#ifndef STRTONUM_H_strtonum
#define STRTONUM_H_strtonum

long long
strtonum(const char *numstr, long long minval, long long maxval,
    const char **errstrp);

#endif /* STRTONUM_H_strtonum */

/* End of file strtonum.h */
