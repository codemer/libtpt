/*
 * symbols_impl.cxx
 *
 * $Id$
 *
 */

/*
 * Copyright (C) 2002-2003 Isaac W. Foraker (isaac@tazthecat.net)
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
 *
 */

#include "conf.h"
#include "symbols_impl.h"
#include "eval.h"
#include <libtpt/parse.h>
#include <cctype>
#include <iostream>
#include <cassert>

namespace TPT {

/*
 * Non-destructive copy of objects
 */
void Symbols_Impl::copy(Object& object)
{
	if (object.gettype() != Object::type_hash)
		throw tptexception("Tried to copy non-hash symbols table");

	// Create references to object hashes
	Object::HashType& rhash = object.hash();
	Object::HashType& lhash = symbols.hash();
	// Create iterators for the source hash
	Object::HashType::iterator it(rhash.begin()), end(rhash.end());
	for (; it != end; ++it) {
		lhash[it->first] = it->second;
	}
}
	
/*
 * Recursively get an object based on the specified id.
 */
Object& Symbols_Impl::getobject(const SymbolKeyType& id, Object& table)
{
	Object::PtrType ptr;
	if (getobjectforget(id, table, ptr))
		return emptyobject;
	assert(ptr.get());
	return *(ptr.get());
}

/*
 * Set scalar object
 *
 * @return	false on success;
 * @return	true if id is invalid.
 */
bool Symbols_Impl::setobject(const SymbolKeyType& id,
							  const std::string& value,
							  Object& table)
{
	Object::PtrType ptr;
	if (getobjectforset(id, table, ptr))
		return true;

	// To make code more readable, assign new object pointer to a
	// reference variable.
	Object& obj = *(ptr.get());
	obj.settype(Object::type_scalar);
	obj.scalar() = value;
	return false;
}


/*
 * Set array object
 *
 * @return	false on success;
 * @return	true if id is invalid.
 */
bool Symbols_Impl::setobject(const SymbolKeyType& id,
							  const SymbolArrayType& value,
							  Object& table)
{
	Object::PtrType ptr;
	if (getobjectforset(id, table, ptr))
		return true;

	// To make code more readable, assign new object pointer to a
	// reference variable.
	Object& obj = *(ptr.get());
	// Reset this object to type array
	obj = Object::type_array;
	Object::ArrayType& array = obj.array();
	array.clear();
	array.resize(value.size());

	size_t i = 0;
	SymbolArrayType::const_iterator it(value.begin()),
		end(value.end());
	for (; it != end; ++it)
		array[i++] = new Object((*it));

	return false;
}

/*
 * Set hash object
 *
 * @return	false on success;
 * @return	true if id is invalid.
 */
bool Symbols_Impl::setobject(const SymbolKeyType& id,
	const SymbolHashType& value, Object& table)
{
	Object::PtrType ptr;
	if (getobjectforset(id, table, ptr))
		return true;

	// To make code more readable, assign new object pointer to a
	// reference variable.
	Object& obj = *(ptr.get());
	// Reset this object to type hash
	obj = Object::type_hash;
	Object::HashType& hash = obj.hash();

	SymbolHashType::const_iterator it(value.begin()),
		end(value.end());
	for (; it != end; ++it)
		hash[it->first] = new Object(it->second);
	return false;
}


/*
 * Set object object
 *
 * @return	false on success;
 * @return	true if id is invalid.
 */
bool Symbols_Impl::setobject(const SymbolKeyType& id,
	Object& value, Object& table)
{
	Object::PtrType ptr;
	if (getobjectforset(id, table, ptr))
		return true;

	// To make code more readable, assign new object pointer to a
	// reference variable.
	Object& obj = *(ptr.get());
	obj = value;

	return false;
}


/*
 * Push a scalar object onto an array
 *
 * @return	false on success;
 * @return	true if id was invalid
 */
bool Symbols_Impl::pushobject(const SymbolKeyType& id,
							  const std::string& value,
							  Object& table)
{
	Object::PtrType ptr;
	if (getobjectforset(id, table, ptr))
		return true;

	Object& obj = *(ptr.get());

	if (obj.gettype() != Object::type_array)
		obj = Object::type_array;
	obj.array().push_back(new Object(value));

	return false;
}


/*
 * Push an array object onto an array
 *
 * @return	false on success;
 * @return	true if id was invalid
 */
bool Symbols_Impl::pushobject(const SymbolKeyType& id,
							  const SymbolArrayType& value,
							  Object& table)
{
	Object::PtrType ptr;
	if (getobjectforset(id, table, ptr))
		return true;

	Object& obj = *(ptr.get());

	if (obj.gettype() != Object::type_array)
		obj = Object::type_array;
	// Create temporary array object
	Object temp(Object::type_array);
	temp.array().resize(value.size());
	size_t i = 0;
	SymbolArrayType::const_iterator it(value.begin()),
		end(value.end());
	for (; it != end; ++it)
		temp.array()[i++] = new Object((*it));

	obj.array().push_back(new Object(temp));

	return false;
}


/*
 * Push a hash object onto an array
 *
 * @return	false on success;
 * @return	true if id was invalid
 */
bool Symbols_Impl::pushobject(const SymbolKeyType& id,
							  const SymbolHashType& value,
							  Object& table)
{
	Object::PtrType ptr;
	if (getobjectforset(id, table, ptr))
		return true;

	Object& obj = *(ptr.get());

	if (obj.gettype() != Object::type_array)
		obj = Object::type_array;
	// Create temporary hash object
	Object temp(Object::type_hash);
	SymbolHashType::const_iterator it(value.begin()),
		end(value.end());
	for (; it != end; ++it)
		temp.hash()[it->first] = new Object(it->second);

	obj.array().push_back(new Object(temp));

	return false;
}


/*
 * Push an object onto an array
 *
 * @return	false on success;
 * @return	true if id was invalid
 */
bool Symbols_Impl::pushobject(const SymbolKeyType& id,
							  Object& value,
							  Object& table)
{
	Object::PtrType ptr;
	if (getobjectforset(id, table, ptr))
		return true;

	Object& obj = *(ptr.get());
	if (obj.gettype() != Object::type_array)
		obj = Object::type_array;
	obj.array().push_back(new Object(value));

	return false;
}



bool Symbols_Impl::getobjectforget(const SymbolKeyType& id, Object& table,
									Object::PtrType& rptr)
{
	// if table is not a hash, then can't process symbols
//	if (table.gettype() != Object::type_hash)
//		return true;

	if (id[0] == '$')
		return getobjectforget(id.substr(2, id.length()-3), symbols, rptr);
	else if (id.find('$') != SymbolKeyType::npos)
	{
		// When id contains embedded ${id}, recurse to build new id.
		// Note: This is really inefficient, and is only here for
		// compatibility.
		Buffer buf(id.c_str(), id.size());
//		Symbols copy(parent);
		Parser p(buf, parent);
		SymbolKeyType newid(p.run());
		if (p.geterrorcount())
			return true;	// couldn't parse
		return getobjectforget(newid, symbols, rptr);
	}

	// Read ID character by character to determine if there is a
	// hash or array part
	std::string newid;
	size_t index, length(id.length());
	if (id == ".")	// ${.} is special foreach variable
		newid = id;
	else
	{
		for (index = 0; index < length; ++index)
		{
			if (id[index] == '.')
			{
				++index;
				if (newid.empty())
					return true;
				if (table.gettype() != Object::type_hash)
					return true;
				// This is a hash
				if (!table.exists(newid))
					return true;
				Object& tempobj = (*table.hash()[newid].get());
				if (tempobj.gettype() != Object::type_hash)
					return true;
				Object::HashType& hash = table.hash();
				return getobjectforget(id.substr(index),
					*(hash[newid].get()), rptr);
			}
			else if (id[index] == '[')
			{
				// Find closing brace, then determine the array index in the
				// braces.
				size_t cbracket = findclosebracket(id, index),
					arrayindex = getarrayindex(id.substr(index+1,
						cbracket-index-1));

				// If thie newid is not empty, table must be a hash, otherwise
				// table should be an array.
				if (!newid.empty())
				{
					// Make sure table is a hash
					if (table.gettype() != Object::type_hash)
						return true;
					// If this object isn't an array, return empty
					if (!table.exists(newid))
						return true;
					if (table.hash()[newid].get()->gettype() != Object::type_array)
						return true;
				}
				else if (table.gettype() != Object::type_array)
					return true;

				// Get a reference to the new array based on whether this
				// object is part of a multidimensional array or a hash.
				Object::ArrayType& array = !newid.empty() ?
					table.hash()[newid].get()->array() : table.array();

				// If index > arraysize, return empty
				if (arrayindex >= array.size())
					return true;
				Object::PtrType ptr(array[arrayindex]);
				// Check if there is more stuff to process
				index = cbracket + 1;
				if (index < id.length())
				{
					// The next character better be a hash . or array []
					if ((id[index] != '[') && (id[index++] != '.'))
						return true;
					return getobjectforget(id.substr(index), *(array[arrayindex].get()), rptr);
				}
				else
				{
					rptr = ptr;
					return false;
				}
			}
			else
				// Add character to id
				newid+= id[index];
		}
	}

	if (table.gettype() != Object::type_hash)
		return true;
	if (newid.empty())
		return true;
	Object::HashType::iterator it = table.hash().find(newid);
	if (it == table.hash().end())
		return true;
	rptr = table.hash()[newid];
	if (!rptr.get())	// value did not exist
		return true;
	return false;
}

bool Symbols_Impl::getobjectforset(const SymbolKeyType& id, Object& table,
									Object::PtrType& rptr)
{
	if (id[0] == '$')
		return getobjectforset(id.substr(2, id.size()-3), symbols, rptr);
	else if (id.find('$') != SymbolKeyType::npos)
	{
		// When id contains embedded ${id}, recurse to build new id.
		// Note: This is really inefficient, and is only here for
		// compatibility.
		Buffer buf(id.c_str(), id.size());
		Parser p(buf, parent);
		SymbolKeyType newid(p.run());
		if (p.geterrorcount())
			return true;	// couldn't parse
		return getobjectforset(newid, symbols, rptr);
	}

	// Read ID character by character to determine if there is a hash
	// or array part
	std::string newid;
	size_t index, length(id.length());
	if (id == ".")
		newid = id;
	else
	{
		for (index = 0; index < length; ++index)
		{
			if (id[index] == '.')
			{
				++index;
				// Make sure the symbol name is not empty.
				if (newid.empty())
					return true;
				// If this object is not a hash, convert a hash.
				if (table.gettype() != Object::type_hash)
					table.settype(Object::type_hash);
				// Get reference to the hash
				Object::HashType& hash = table.hash();
				// Make sure the new symbol exists
				if (hash.find(newid) == hash.end())
					hash[newid] = new Object(Object::type_hash);
				// Recurse on key part of hash
				return getobjectforset(id.substr(index),
					*(hash[newid].get()), rptr);
			}
			else if (id[index] == '[')
			{
				// Find closing brace, then determine the array index in the
				// braces.
				size_t cbracket = findclosebracket(id, index),
					arrayindex = getarrayindex(id.substr(index+1,
						cbracket-index-1));
				// Make sure the array index is not beyond the allowed maximum.
				if (arrayindex >= maxarraysize)
					return true;

				// If newid is empty, the parent must be an array.  Otherwise,
				// the parent must be a hash.
				if (!newid.empty())
				{
					// This table must be a hash, or there is something wrong
					// with LibTPT.
					if (table.gettype() != Object::type_hash)
						return true;
					// Get reference to the parent hash.
					Object::HashType& hash = table.hash();
					// Make sure this symbol exists as an array object.
					if (hash.find(newid) == hash.end())
						hash[newid] = new Object(Object::type_array);
					else if (hash[newid].get()->gettype() != Object::type_array)
						hash[newid] = new Object(Object::type_array);
				}
				else if (table.gettype() != Object::type_array)
					return true;
				// Get a reference to the new array based on whether this
				// object is part of a multidimensional array or a hash.
				Object::ArrayType& array = !newid.empty() ?
					table.hash()[newid].get()->array() : table.array();
				// Make sure target array is large enough for array index.
				if (arrayindex >= array.size())
					array.resize(arrayindex+1);

				// Check if this is the end of the symbol or if there is a hash
				// or array component still to process.
				index = cbracket + 1;
				if (index < id.length())
				{
					// The next character must be a hash . or array [index],
					// otherwise this symbol is invalid.
					if (id[index] == '[')
						array[arrayindex] = new Object(Object::type_array);
					else if (id[index++] == '.')
						array[arrayindex] = new Object(Object::type_hash);
					else
						return true;
					return getobjectforset(id.substr(index),
							*(array[arrayindex].get()), rptr);
				}
				else
				{
					// If the array index is the last part of the identifier,
					// then this call must be setting a scalar object.
					array[arrayindex] = new Object(Object::type_scalar);
					rptr = array[arrayindex];
					return false;
				}
			}
			else
				// Add character to id
				newid+= id[index];
		}
	}
	// Make sure the symbol name is not empty.
	if (newid.empty())
		return true;
	// Make sure the current object is a hash of symbols.
	if (table.gettype() != Object::type_hash)
		return true;
	// create new object
	Object::HashType::iterator it(table.hash().find(newid));
	if (it != table.hash().end())
	{
		rptr = it->second;
	}
	else
	{
		rptr = new Object(Object::type_scalar);
		table.hash()[newid] = rptr;
	}
	return false;
}


size_t Symbols_Impl::getarrayindex(const std::string& expr)
{
	size_t arrayindex;
	if (istextnumber(expr.c_str()))
	{
		// if temp is just a number, get the number
		arrayindex = std::atoi(expr.c_str());
	}
	else
	{
		// otherwise instantiate a parser to handle expression
		std::string temp("@eval(");
		temp+= expr;
		temp+= ")";
		arrayindex = std::atoi(eval(temp, &parent).c_str());
	}
	return arrayindex;
}


size_t Symbols_Impl::findclosebracket(const SymbolKeyType& id, size_t obracket)
{
	// There is an array index here, so count brackets until close
	unsigned level = 1;
	size_t cbracket = obracket + 1;
	while (level)
	{
		if (!id[cbracket])
			return 0;
		if (id[cbracket] == '[')
			++level;
		else if (id[cbracket] == ']')
			--level;
		++cbracket;
	}
	return --cbracket;
}


bool Symbols_Impl::istextnumber(const char* str)
{
	while (*str)
	{
		if (!std::isdigit(*str))
			return false;
		++str;
	}
	return true;
}

} // end namespace TPT
