/*
perf.h -- perfomance monitor
Copyright (C) 2018 a1batross

*/

#pragma once
#ifndef PERF_H
#define PERF_H

qboolean Perf_IsActive( void );

#ifdef XASH_PERFMON
#define PERF_FRAMES 2048 // remember perfomance of N frames
#define PERF_FRAMES_MASK (PERF_FRAMES-1)

typedef struct perf_func_s
{
	const char *name;
	double start;
	double end;
} perf_func_t;

typedef struct perf_list_s
{
	perf_func_t f;

	int stackidx;
	struct perf_list_s *stacknext; // temporary pointer for keeping call stack
	struct perf_list_s *graphnext;
} perf_list_t;

typedef struct perf_frame_s
{
	perf_list_t *head;
	perf_list_t *cur; // points to the end of call graph

	perf_list_t *callstack; // pointer to call stack

	int stacksize;
} perf_frame_t;

perf_frame_t *Perf_GetOnce( void );
perf_frame_t *Perf_GetFrame( int framenum );

void Perf_Init( void );
void Perf_Shutdown( void );
void Perf_StartFrame( void );
void Perf_FunctionStart( const char *function );
void Perf_FunctionEnd( const char *function );
void Perf_FunctionStartOnce( const char *function );
void Perf_FunctionEndOnce( const char *function );
#ifndef XASH_DEDICATED
void CL_InitPerf( void );
void CL_DrawPerf( void );
#endif // XASH_DEDICATED

#define PERF_START Perf_FunctionStart( __FUNCTION__ )
#define PERF_END Perf_FunctionEnd( __FUNCTION__ )
#define PERF_RETURN { PERF_END; return; }
#define PERF_RETVAL(x) { PERF_END; return (x); }
#define PERF_ONCE_START Perf_FunctionStartOnce( __FUNCTION__ )
#define PERF_ONCE_END Perf_FunctionEndOnce( __FUNCTION__ )
#define PERF_ONCE_RETURN { PERF_ONCE_END; return; }
#define PERF_ONCE_RETVAL(x) { PERF_ONCE_END; return (x); }
#else // Remove unneeded calls
#define PERF_START
#define PERF_END
#define PERF_RETURN return
#define PERF_RETVAL(x) return (x)
#define PERF_ONCE_START
#define PERF_ONCE_END
#define PERF_ONCE_RETURN PERF_RETURN
#define PERF_ONCE_RETVAL(x) PERF_RETVAL(x)
#endif // XASH_PERFMON
#endif // PERF_H
