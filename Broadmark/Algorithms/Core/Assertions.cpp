

#include <iostream>


//#define DOCTEST_CONFIG_IMPLEMENT
#include "Dependencies\doctest.h"


void assertionHandler(const doctest::AssertData& ad) {
	std::cout << "Assertion failure: " << ad.m_expr << " expanded as: " << ad.m_decomp << ". At " << ad.m_file << "(" << ad.m_line << ")\n";
	std::cout.flush();
	exit(-1);
}

doctest::Context __assertionsOutOfTestCasesContext;
void initializeDocTestForAssertionsOutOfTestCases() {
	__assertionsOutOfTestCasesContext.setAsDefaultForAssertsOutOfTestCases();
	__assertionsOutOfTestCasesContext.setAssertHandler(assertionHandler);
}