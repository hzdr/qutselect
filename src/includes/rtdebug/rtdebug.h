/* vim:set ts=2 nowrap: ****************************************************

 librtdebug - A C++ based thread-safe Runtime Debugging Library
 Copyright (C) 2003-2006 by Jens Langner <Jens.Langner@light-speed.de>

 This library is free software; you can redistribute it and/or
 modify it under the terms of the GNU Lesser General Public
 License as published by the Free Software Foundation; either
 version 2.1 of the License, or (at your option) any later version.

 This library is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 Lesser General Public License for more details.

 You should have received a copy of the GNU Lesser General Public
 License along with this library; if not, write to the Free Software
 Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

 $Id: rtdebug.h 34 2011-12-08 14:45:50Z damato $

***************************************************************************/

#ifndef RTDEBUG_H
#define RTDEBUG_H

// This header is just a plain dummy to please libmedio in case there
// is no librtdebug installation found at all. It will be put in the
// include path and if loaded by the compiler takes precedence over the
// global installed rtdebug.h of the librtdebug package. It will simply
// define all debug statements as void() and nothing more.

// redefine the Qt's own debug system
#ifdef qDebug
#undef qDebug
#define qDebug	D
#endif
#ifdef qWarning
#undef qWarning
#define qWarning W
#endif
#ifdef qFatal
#undef qFatal
#define qFatal E
#endif

// first we make sure all previously defined symbols are undefined now so
// that no other debug system interferes with ours.
#if defined(ENTER)
#undef ENTER
#endif
#if defined(LEAVE)
#undef LEAVE
#endif
#if defined(RETURN)
#undef RETURN
#endif
#if defined(SHOWVALUE)
#undef SHOWVALUE
#endif
#if defined(SHOWPOINTER)
#undef SHOWPOINTER
#endif
#if defined(SHOWSTRING)
#undef SHOWSTRING
#endif
#if defined(SHOWMSG)
#undef SHOWMSG
#endif
#if defined(STARTCLOCK)
#undef STARTCLOCK
#endif
#if defined(STOPCLOCK)
#undef STOPCLOCK
#endif
#if defined(D)
#undef D
#endif
#if defined(E)
#undef E
#endif
#if defined(W)
#undef W
#endif
#if defined(ASSERT)
#undef ASSERT
#endif
#if defined(Debug)
#undef Debug
#endif
#if defined(Verbose)
#undef Verbose
#endif
#if defined(Warning)
#undef Warning
#endif
#if defined(Error)
#undef Error
#endif
#if defined(Info)
#undef Info
#endif

#define ENTER()							(void(0))
#define LEAVE()							(void(0))
#define RETURN(r)						(void(0))
#define SHOWVALUE(v)				(void(0))
#define SHOWPOINTER(p)			(void(0))
#define SHOWSTRING(s)				(void(0))
#define SHOWMSG(m)					(void(0))
#define STARTCLOCK(s)				(void(0))
#define STOPCLOCK(s)				(void(0))
#define D(s, vargs...)			(void(0))
#define E(s, vargs...)			(void(0))
#define W(s, vargs...)			(void(0))
#define ASSERT(expression)	(void(0))

#endif // RTDEBUG_H
