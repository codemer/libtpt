/*
 * parse_impl_using.cxx
 *
 * Handle the @using instruction, loading the specified shared library.
 *
 * Copyright (C) 2002-2009 Isaac W. Foraker (isaac at noscience dot net)
 * All Rights Reserved
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in
 *    the documentation and/or other materials provided with the
 *    distribution.
 * 3. Neither the name of the Author nor the names of its contributors
 *    may be used to endorse or promote products derived from this
 *    software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR AND CONTRIBUTORS ``AS IS''
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A
 * PARTICULAR PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
 * PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY
 * OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "conf.h"
#include "parse_impl.h"
#include <algorithm>
#include <sstream>
#include <iostream>
#include <cstdio>

namespace {
#ifdef WIN32
	const char* shlib_ext = ".dll";
#else
	const char* shlib_ext = ".so";
#endif
} // end anonymous namespace

namespace TPT {


/*
 * Handle inclusion of shared library code.  File will be a DLL on Windows or
 * SO on Unix.
 */
void Parser_Impl::parse_using()
{
	Object params;
	if (getparamlist(params))
		return;
	Object::ArrayType& pl = params.array();

	if (pl.size() != 1)
	{
		recorderror("Error: @using takes exactly 1 parameter");
		return;
	}

	Object& obj = *pl[0].get();		// obj now contains parameter
	if (obj.gettype() != Object::type_scalar)
	{
		recorderror("Error: @using excpects string");
		return;
	}

	// Construct the filename
	std::string filename(obj.scalar());
	filename+= shlib_ext;

	// Search for the file
}

} // end namespace TPT
