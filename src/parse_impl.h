/*
 * parse_impl.h
 *
 * Parser Implementation
 *
 * $Id$
 *
 */

#ifndef __tptlib_parse_impl_h
#define __tptlib_parse_impl_h

#ifdef _MSC_VER
// Disable long symbol name warning on MSVC++
#pragma warning(disable:4786)
#endif

#include <tptlib/parse.h>
#include "lexical.h"
#include "macro.h"

#ifdef _MSC_VER
#include <native.h>
#else
#include <sys/types.h>
#endif

namespace TPTLib {

struct Parser::Impl {
	Lex lex;
	unsigned level;	// if/loop level
	SymbolMap localsymmap;
	SymbolMap& symbols;	// reference to whatever symbol map is in use
	MacroList localmacros;
	MacroList& macros;	// reference to whatever macro list is in use
	ErrorList errlist;
	Token<> tok;
	bool isseeded;

	// kiss_vars are used for pseudo-random number generation
	unsigned long kiss_x;
	unsigned long kiss_y;
	unsigned long kiss_z;
	unsigned long kiss_w;
	unsigned long kiss_carry;
	unsigned long kiss_k;
	unsigned long kiss_m;
	unsigned long kiss_seed;

	Impl(Buffer& buf, const SymbolTable* st) : lex(buf), level(0),
		symbols(localsymmap), macros(localmacros), isseeded(false)
	{ if (st) localsymmap = *st; }
	Impl(Buffer& buf, SymbolMap& sm, MacroList& ml) : lex(buf), level(0),
		symbols(sm), macros(ml), isseeded(false) { }
	~Impl();

	void recorderror(const std::string& desc, const Token<>* neartoken=0);
	bool getnextparam(std::string& value);
	bool getnextstrict(Token<>& target);
	bool getparamlist(ParamList& pl);

	bool pass1(std::ostream* os);

	int64_t str2num(const char* str);
	inline int64_t str2num(const std::string& str)
	{ return str2num(str.c_str()); }
	void num2str(int64_t value, std::string& str);


	Token<> parse_level0(Token<>& left);
	Token<> parse_level1(Token<>& left);
	Token<> parse_level2(Token<>& left);
	Token<> parse_level3(Token<>& left);
	Token<> parse_level4(Token<>& left);
	Token<> parse_level5(Token<>& left);
	Token<> parse_level6(Token<>& left);
	Token<> parse_level7(Token<>& left);

	void srandom32();
	unsigned int random32();
	Token<> parse_rand();

	Token<> parse_empty();
	Token<> parse_concat();
	Token<> parse_length();
	Token<> parse_count();

	void parse_block(std::ostream* os, bool istop=false, bool isloop=false);
	void parse_include(std::ostream* os);
	void parse_if(std::ostream* os);
	void parse_set();

	void parse_macro();
	void user_macro(const std::string& name, std::ostream* os);
};


} // end namespace TPTLib

#endif // __tptlib_parse_impl_h
