#pragma once
/*
	Copyright (C) 1991, 1992, 1996, 1997, 1999 Free Software Foundation, Inc.
	This file is part of the GNU C Library.
	Written by Douglas C. Schmidt (schmidt@ics.uci.edu).
*/
#define _QSORT_SWAP(a, b, t) ((void)((t = *a), (*a = *b), (*b = t)))
#define _QSORT_MAX_THRESH 4
#define _QSORT_STACK_SIZE	(8 * sizeof(unsigned))
#define _QSORT_PUSH(top, low, high)	\
	(((top->_lo = (low)), (top->_hi = (high)), ++top))
#define	_QSORT_POP(low, high, top)	\
	((--top, (low = top->_lo), (high = top->_hi)))
#define	_QSORT_STACK_NOT_EMPTY	(_stack < _top)

#define B_QSORT(QSORT_TYPE,QSORT_BASE,QSORT_NELT,QSORT_LT)\
{\
  QSORT_TYPE *const _base = (QSORT_BASE);\
  const unsigned _elems = (QSORT_NELT);\
  QSORT_TYPE _hold;\
  if (_elems > _QSORT_MAX_THRESH) {\
	QSORT_TYPE *_lo = _base;\
	QSORT_TYPE *_hi = _lo + _elems - 1;\
	struct {\
	  QSORT_TYPE *_hi; QSORT_TYPE *_lo;\
	} _stack[_QSORT_STACK_SIZE], *_top = _stack + 1;\
	while (_QSORT_STACK_NOT_EMPTY) {\
	  QSORT_TYPE *_left_ptr; QSORT_TYPE *_right_ptr;\
	  QSORT_TYPE *_mid = _lo + ((_hi - _lo) >> 1);\
	  if (QSORT_LT (_mid, _lo))\
		_QSORT_SWAP (_mid, _lo, _hold);\
	  if (QSORT_LT (_hi, _mid))	{\
		_QSORT_SWAP (_mid, _hi, _hold);\
		if (QSORT_LT (_mid, _lo))\
		  _QSORT_SWAP (_mid, _lo, _hold);\
	  }\
	  _left_ptr  = _lo + 1;\
	  _right_ptr = _hi - 1;\
	  do {\
		while (QSORT_LT (_left_ptr, _mid))\
		 ++_left_ptr;\
		while (QSORT_LT (_mid, _right_ptr))\
		  --_right_ptr;\
		if (_left_ptr < _right_ptr) {\
		  _QSORT_SWAP (_left_ptr, _right_ptr, _hold);\
		  if (_mid == _left_ptr)\
			_mid = _right_ptr;\
		  else if (_mid == _right_ptr)\
			_mid = _left_ptr;\
		  ++_left_ptr;\
		  --_right_ptr;\
		}\
		else if (_left_ptr == _right_ptr) {\
		  ++_left_ptr;\
		  --_right_ptr;\
		  break;\
		}\
	  } while (_left_ptr <= _right_ptr);\
	  if (_right_ptr - _lo <= _QSORT_MAX_THRESH) {\
		if (_hi - _left_ptr <= _QSORT_MAX_THRESH)\
		  _QSORT_POP (_lo, _hi, _top);\
		else\
		  _lo = _left_ptr;\
	  }\
	  else if (_hi - _left_ptr <= _QSORT_MAX_THRESH)\
		_hi = _right_ptr;\
	  else if (_right_ptr - _lo > _hi - _left_ptr) {\
		_QSORT_PUSH (_top, _lo, _right_ptr);\
		_lo = _left_ptr;\
	  }\
	  else {\
		_QSORT_PUSH (_top, _left_ptr, _hi);\
		_hi = _right_ptr;\
	  }\
	}\
  }\
  {\
	QSORT_TYPE *const _end_ptr = _base + _elems - 1;\
	QSORT_TYPE *_tmp_ptr = _base;\
	register QSORT_TYPE *_run_ptr;\
	QSORT_TYPE *_thresh;\
	_thresh = _base + _QSORT_MAX_THRESH;\
	if (_thresh > _end_ptr)\
	  _thresh = _end_ptr;\
	for (_run_ptr = _tmp_ptr + 1; _run_ptr <= _thresh; ++_run_ptr)\
	  if (QSORT_LT (_run_ptr, _tmp_ptr))\
		_tmp_ptr = _run_ptr;\
	if (_tmp_ptr != _base)\
	  _QSORT_SWAP (_tmp_ptr, _base, _hold);\
	_run_ptr = _base + 1;\
	while (++_run_ptr <= _end_ptr) {\
	  _tmp_ptr = _run_ptr - 1;\
	  while (QSORT_LT (_run_ptr, _tmp_ptr))\
		--_tmp_ptr;\
	  ++_tmp_ptr;\
	  if (_tmp_ptr != _run_ptr) {\
		QSORT_TYPE *_trav = _run_ptr + 1;\
		while (--_trav >= _run_ptr) {\
		  QSORT_TYPE *_hi; QSORT_TYPE *_lo;\
		  _hold = *_trav;\
		  for (_hi = _lo = _trav; --_lo >= _tmp_ptr; _hi = _lo)\
			*_hi = *_lo;\
		  *_hi = _hold;\
		}\
	  }\
	}\
  }\
}
