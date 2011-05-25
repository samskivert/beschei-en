#ifndef MAIN_H
#define MAIN_H

/* $Id: main.h,v 39.1 1995/04/25 01:43:07 mbayne Exp $
 *
 * The information in this file was created by Michael D. Bayne. This
 * information is in the public domain. You are permitted to reuse, rewrite,
 * bend, fold and mutilate this information. When it comes down to it, all
 * this distribution information is pretty much crap anyway and I maintain
 * exclusive rights to everything you see here, but go ahead and use it
 * anyway. I'm too busy doing cool stuff to sue anyone.
 *
 * $Log: main.h,v $
 * Revision 39.1  1995/04/25  01:43:07  mbayne
 * Initial revision.
 *
 */

extern struct Screen *Scr;
extern struct Window *Win;
extern struct Menu *Menus;
extern struct VisualInfo *VisInf;
extern LONG Width, Height;

#include "protos/main_protos.h"

#endif /* MAIN_H */
