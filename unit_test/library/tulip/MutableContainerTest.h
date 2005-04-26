#ifndef TLPMUTABLECONTAINERTEST
#define TLPMUTABLECONTAINERTEST

#include <string>
#include <tulip/MutableContainer.h>
#include <cppunit/TestFixture.h>
#include <cppunit/TestSuite.h>

class MutableContainerTest : public CppUnit::TestFixture {
private:
  MutableContainer<bool> *mutBool;
  MutableContainer<double> *mutDouble;
  MutableContainer<std::string> *mutString;
  static const unsigned int NBTEST=1000;

public:
  void setUp();
  void tearDown();
  void testSetAll();
  void testSetGet();
  void testFindAll();
  void testCompression();
  static CppUnit::Test *suite();
};

#endif
