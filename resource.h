#ifndef RESOURCES_H
#define RESOURCES_H

/* $Id$
 *
 * The information in this file was created by Michael D. Bayne. This
 * information is in the public domain. You are permitted to reuse, rewrite,
 * bend, fold and mutilate this information. When it comes down to it, all
 * this distribution information is pretty much crap anyway and I maintain
 * exclusive rights to everything you see here, but go ahead and use it
 * anyway. I'm too busy doing cool stuff to sue anyone.
 * 
 * $Log$
 */

#define RS_ZEROARG  0
#define RS_ONEARG 1

#ifndef EXEC_LISTS_H
#include <exec/lists.h>
#endif

typedef void __regargs (*FREEFUNC)( VOID * );

typedef struct _PvtResource
{
	struct Node rs_Node;
	VOID *rs_Resource;
	FREEFUNC rs_Destructor;
}
PvtResource;

#define resourceHead( l )\
	( IsListEmpty( l ) ? 0L : ( PvtResource * )(l)->lh_Head )
#define resourceTail( l )\
	( IsListEmpty( l ) ? 0L : ( PvtResource * )(l)->lh_TailPred )
#define resourceSucc( n ) (( PvtResource * )(n)->rs_Node.ln_Succ )
#define resourcePred( n ) (( PvtResource * )(n)->rs_Node.ln_Pred )

#include "protos/resource_protos.h"

#endif /* RESOURCES_H */
