/*
 * test1.cxx
 *
 * $Id$
 *
 */

#ifdef _MSC_VER
// Disable long symbol name warning on MSVC++
#pragma warning(disable:4786)
#endif

#include <tptlib/buffer.h>
#include <tptlib/parse.h>

#include <iostream>
#include <stdexcept>
#include <sstream>
#include <cstring>
#include <cstdio>
#include <cstdlib>

bool test1(unsigned testcount);

int main(int argc, char* argv[])
{
	bool result=false, r;

	if (argc != 2)
	{
		std::cout << "Usage: test1 <testcount>" << std::endl;
		return 0;
	}

	try {
		r = test1(std::atoi(argv[1]));
//		if (r) std::cout << "failed" << std::endl;
//		else std::cout << "passed" << std::endl;
		result|= r;
	} catch(const std::exception& e) {
		result = true;
		std::cout << "Exception " << e.what() << std::endl;
	} catch(...) {
		result = true;
		std::cout << "Unknown exception" << std::endl;
	}
	if (result)
		std::cout << "FAILED" << std::endl;
	else
		std::cout << "PASSED" << std::endl;

	return result;
}

bool test1(unsigned testcount)
{
	TPTLib::SymbolTable sym;
	bool result = false;

	sym["var"] = "this is the value of var";
	sym["var1"] = "Supercalifragilisticexpialidocious";
	sym["var2"] = "The red fox runs through the plain and jumps over the fence.";
	sym["title"] = "TEST TITLE";

	char tptfile[256], outfile[256];
	unsigned i;

	for (i = 0; i < testcount; ++i)
	{
		std::cout << "test" << (i+1) << ".tpt: ";

		// generate test file names by rule
		sprintf(tptfile, "test%u.tpt", i+1);
		sprintf(outfile, "test%u.out", i+1);

		// Process the tpt file and store the result in a string
		TPTLib::Buffer tptbuf(tptfile);
		TPTLib::Parser p(tptbuf, &sym);
		std::string tptstr;
		std::stringstream strs(tptstr);
		p.run(strs);

		// Load the out file
		TPTLib::Buffer outbuf(outfile);
		std::string outstr;
		while (outbuf)
			outstr+= outbuf.getnextchar();

		// Compare tptstr to outstr
		if (strs.str() == outstr)
		{
			std::cout << "passed" << std::endl;
		}
		else
		{
			result|= true;
			std::cout << "failed" << std::endl;
		}
		std::cout << "\ntptstr = (" << strs.str().size() << ")\n<quote>" << strs.str() << "</quote>" <<std::endl;
		std::cout << "\noutstr = (" << outstr.size() << ")\n<quote>" << outstr << "</quote>" <<std::endl;
	}

	return result;
}