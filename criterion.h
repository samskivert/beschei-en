#ifndef CRITERION_H
#define CRITERION_H

/* $Id: criterion.h,v 39.1 1995/04/25 01:47:04 mbayne Exp $
 *
 * The information in this file was created by Michael D. Bayne. This
 * information is in the public domain. You are permitted to reuse, rewrite,
 * bend, fold and mutilate this information. When it comes down to it, all
 * this distribution information is pretty much crap anyway and I maintain
 * exclusive rights to everything you see here, but go ahead and use it
 * anyway. I'm too busy doing cool stuff to sue anyone.
 * 
 * $Log: criterion.h,v $
 * Revision 39.1  1995/04/25  01:47:04  mbayne
 * Initial revision.
 *
 */

#ifndef STACK_H
#include "stack.h"
#endif

typedef int (*SourceCriterionFunc)( Stack * );
typedef int (*CriterionFunc)( Stack *, Stack * );
	 
#include "protos/criterion_protos.h"

#endif /* CRITERION_H */
