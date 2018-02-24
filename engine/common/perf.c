/*
perf.c -- perfomance monitor
Copyright (C) 2018 a1batross

*/

#include "common.h"
#include "perf.h"

qboolean Perf_IsActive()
{
#ifdef XASH_PERFMON
	return true;
#else // XASH_PERFMON
	return false;
#endif // XASH_PERFMON
}

#ifdef XASH_PERFMON
struct
{
	void *mempool;

	// for function called once, like init funcs
	perf_frame_t once;

	// for functions called every frame
	perf_frame_t every[PERF_FRAMES];

} perf;

perf_frame_t *Perf_GetOnce( void )
{
	return &perf.once;
}

perf_frame_t *Perf_GetFrame( int framenum )
{
	return perf.every + (framenum & PERF_FRAMES_MASK);
}

/*
 * example frame performance structure:
 *
 * Host_Frame
 * +-Host_ClientFrame
 * |	+-CL_PredictMovement
 * |	|	+-SCR_UpdateScreen
 * |	|	|	+-V_RenderView
 * |	|	|	+-V_PostRender
 * |	+-CL_RedoPrediction
 * +-Host_ServerFrame
 *  ...
 *
 * Such tree is being built only by handling current function nesting level.
 * When function is called inside previous tracked it creates a new nest level
 */

_inline void Perf_FunctionStartGeneric( perf_frame_t *frame, const char *function )
{
	perf_list_t *target;

	if( !frame->head ) // engine is started, no frames yet
	{
		frame->head = target = Mem_Alloc( perf.mempool, sizeof( perf_list_t ));
		target->graphnext = NULL;
	}
	else if( !frame->cur ) // new frame
	{
		target = frame->head;
	}
	else if( !frame->cur->graphnext ) // append to the list, if not enough
	{
		frame->cur->graphnext = target = Mem_Alloc( perf.mempool, sizeof( perf_list_t ));
		target->graphnext = NULL;
	}
	else // use
	{
		target = frame->cur->graphnext;
	}

	if( target->graphnext )
		target->graphnext->f.name = NULL;

	frame->cur = target;

	target->stacknext = frame->callstack;
	target->stackidx = frame->stacksize;
	target->f.name = function;

	frame->callstack = target;
	frame->stacksize++;

#if 0
	if( host.framecount <= 2 )
	{
		if( target->stacknext == 0 )
		{
			MsgDev( D_NOTE, "Entering %s\n", target->f.name );
		}
		else
		{
			MsgDev( D_NOTE, "Entering %s from %s\n", target->f.name, target->stacknext->f.name );
		}
	}
#endif
	target->f.start = Sys_DoubleTime(); //WARN: timer may be isn't precise enough!
}

_inline void Perf_FunctionEndGeneric( perf_frame_t *frame, const char *function )
{
	perf_list_t *target;

	ASSERT( frame->stacksize );
	ASSERT( frame->callstack );
	ASSERT( !Q_strcmp( frame->callstack->f.name, function ) );

	target = frame->callstack;
	target->f.end = Sys_DoubleTime(); // WARN: timer may be isn't precise enough!
	frame->callstack = target->stacknext;
	frame->stacksize--;

#if 0
	if( host.framecount <= 2 )
	{
		if( target->stacknext == 0 )
		{
			MsgDev( D_NOTE, "Leaving %s: %f\n", target->f.name, target->f.end - target->f.start );
		}
		else
		{
			MsgDev( D_NOTE, "Leaving %s to %s:%f\n", target->f.name, target->stacknext->f.name, target->f.end - target->f.start );
		}
	}
#endif
}

void Perf_FunctionStartOnce( const char *function )
{
	Perf_FunctionStartGeneric( &perf.once, function );
}

void Perf_FunctionEndOnce( const char *function )
{
	Perf_FunctionEndGeneric( &perf.once, function );
}

void Perf_FunctionStart( const char *function )
{
	int frame = host.framecount & PERF_FRAMES_MASK;

	Perf_FunctionStartGeneric( perf.every + frame, function );
}

void Perf_FunctionEnd( const char *function )
{
	int frame = host.framecount & PERF_FRAMES_MASK;

	Perf_FunctionEndGeneric( perf.every + frame, function );
}

void Perf_StartFrame( void )
{
	int frame = host.framecount & PERF_FRAMES_MASK;

	perf.every[frame].stacksize = 0;
	perf.every[frame].cur = NULL;
	perf.every[frame].callstack = NULL;
}

void Perf_PrintTimes( perf_frame_t *f )
{
	static char empty[256];
	perf_list_t *l;
	int prevNestLevel = -1, i;

	for( l = f->head; l && l->f.name; l = l->graphnext )
	{
		if( l->stackidx != prevNestLevel )
		{
			empty[0] = 0; // hack?
			for( i = 0; i < l->stackidx; i++ )
			{
				Q_strcat( empty, "|   " );
			}
		}

		MsgDev( D_NOTE, "%s+---%s: %f\n", empty, l->f.name, l->f.end - l->f.start );

		prevNestLevel = l->stackidx;
	}
}

void Perf_PrintTimes_f( void )
{
	Perf_PrintTimes( &perf.every[( host.framecount & PERF_FRAMES_MASK )-1]);
}

void Perf_Init( void )
{
	Q_memset( &perf, 0, sizeof( perf ));

	perf.mempool = Mem_AllocPool( "Perf Mon Zone" );

	Cmd_AddCommand( "perf_showtimes", Perf_PrintTimes_f, "show times for functions called only once" );
}

void Perf_Shutdown( void )
{
	Cmd_RemoveCommand( "perf_showtimes" );

//	Mem_FreePool( perf.mempool ); ??
	perf.mempool = 0;
}

#endif // XASH_PERFMON
