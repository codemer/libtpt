/*
 * parse_impl.cxx
 *
 * Parser Implementation
 *
 * $Id$
 *
 */

#include "parse_impl.h"

#include <algorithm>
#include <sstream>
#include <iostream>

namespace TPTLib {


void Parser::Impl::parse_macro()
{
	if (level > 0)
	{
		recorderror("Macro may not be defined in sub-block");
		return;
	}
	if (getnextstrict(tok))
		return;
	if (tok.type != token_openparen)
	{
		recorderror("Expected macro declaration");
		return;
	}
	if (getnextstrict(tok))
		return;
	if (tok.type != token_id)
	{
		recorderror("Macro requires name parameter");
		return;
	}
	Macro newmacro;
	std::string name(tok.value);

	tok = lex.getstricttoken();
	while ((tok.type != token_eof) && (tok.type != token_closeparen))
	{
		if (tok.type != token_id)
		{
			recorderror("Syntax error, expected identifier", &tok);
			return;
		}
		newmacro.params.push_back(tok.value);
		tok = lex.getstricttoken();

		// The next token should be a comma or a close paren
		if ( (tok.type != token_closeparen) && (tok.type != token_comma) )
		{
			recorderror("Syntax error, expected comma or close parenthesis", &tok);
			return;
		}
		tok = lex.getstricttoken();
	}

	if (tok.type == token_eof)
	{
		recorderror("Unexpected end of file");
		return;
	}
	newmacro.body = lex.getblock();
	if (newmacro.body.empty())
	{
		recorderror("Expected macro body {}");
		return;
	}
	macros[name] = newmacro;
}


void Parser::Impl::user_macro(const std::string& name, std::ostream* os)
{
	std::string id(name.substr(1));
	ParamList pl;
	if (getparamlist(pl))
		return;

	// The id does not include the prefix @.
	MacroList::const_iterator it(macros.find(id));
	if (it == macros.end())
	{
		recorderror("Undefined macro: " + name);
		return;
	}

	const Macro& mac = (*it).second;
	// While processing parameters, preserve existing symbols
	// if there is a name collision.
	SymbolStack_t symstack;	// preserve scope on global symbols.
	Symbol_t tempsym;
	ParamList::const_iterator pit(mac.params.begin()),
		pend(mac.params.end());
	unsigned i = 0;
	for (; pit != pend; ++pit)
	{
		if (symbols.exists((*pit)))
		{
			tempsym.id = (*pit);
			tempsym.value = symbols.get((*pit));
			symstack.push(tempsym);
		}
		if (i >= pl.size())
			symbols.set((*pit), "");
		else
			symbols.set((*pit), pl[i++]);
	}

	Buffer newbuf(mac.body.c_str(), mac.body.size()+1);
	Impl imp(newbuf, symbols, macros);
	imp.parse_block(os);

}


} // end namespace TPTLib
