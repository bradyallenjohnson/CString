
#include <iostream>

#include <CString.h>

// This is a very simple, basic test program for the CString class

#define TEST_CASE(testMethod) \
  try{ testMethod; } \
  catch(CStringException &cse) {std::cerr << "Exception caught in test: " << #testMethod << " " << cse.what().str() << std::endl;} \
  catch(...) {std::cerr << "Unexpected Exception caught in test: [" << __FILE__ << ":" << __LINE__ << "] " << std::endl;}

#define FAIL(errorStr) \
  std::cerr << "FAIL: [" << __FILE__ << ":" << __LINE__ << "] " errorStr << std::endl;

#define LOG(logStr) \
  std::cerr << "[" << __FILE__ << ":" << __LINE__ << "] " logStr << std::endl;

#define ASSERT_TRUE(boolValue, errorStr) \
  if( !boolValue ) { std::cerr << "ASSERT_TRUE FAILURE [" << __FILE__ << ":" << __LINE__ << "] " << errorStr << std::endl; }

#define ASSERT_FALSE(boolValue, errorStr) \
  if( boolValue ) { std::cerr << "ASSERT_FALSE FAILURE [" << __FILE__ << ":" << __LINE__ << "] " << errorStr << std::endl; }

#define ASSERT_EQUALS(val1, val2, errorStr) \
    if( val1 != val2 ) { \
      std::cerr << "ASSERT_EQUALS FAILURE [" << __FILE__ << ":" << __LINE__ \
                << "] values=[" << val1 << ", " << val2 << "] " << errorStr << std::endl; }

#define ASSERT_NOT_EQUALS(val1, val2, errorStr) \
    if( val1 == val2 ) { \
      std::cerr << "ASSERT_NOT_EQUALS FAILURE [" << __FILE__ << ":" << __LINE__ \
                << "] values=[" << val1 << ", " << val2 << "] " << errorStr << std::endl; }

#define ASSERT_THROWS(method, exception, errorStr) \
    try{ method; LOG("Exception not thrown"); FAIL(errorStr); } \
    catch(exception &e) {} \
    catch(...) { FAIL("Unhandled Exception"); }

#define ASSERT_THROWS_STR(method, exception, exceptionStr, errorStr) \
    try{ method; LOG("Exception not thrown"); FAIL(errorStr); } \
    catch(exception &e) { if(!e.what().equals(exceptionStr)) { \
           FAIL(errorStr) \
           std::cerr << "Correct exception thrown, but with incorrect exception string: [" \
                     << exceptionStr << "]" << std::endl; } } \
    catch(CStringException &cse) { FAIL(errorStr); std::cerr << "CString Exception caught: " << cse.what().str() << std::endl;} \
    catch(...) { FAIL("Unhandled Exception"); }

#define ASSERT_NOT_THROWS(method, errorStr) \
    try{ method; } \
    catch(CStringException &cse) { FAIL(errorStr); std::cerr << "CString Exception caught: " << cse.what().str() << std::endl;} \
    catch(...) { LOG("Exception caught"); FAIL(errorStr); }


// A simple CString test class that allows the references to be obtained
class TestCString : public CString
{
  public:
    TestCString(const CString &copy) : CString(copy) {;}
    inline int getReferences() const { return data_->references_; };
};

// Simple do-nothing method to test passing CStrings by value
void simpleCstringObjectMethod(CString str)
{
  const char *strPtr = str.str();
  int size = str.size();
  const char ch = strPtr[size-1];
}

// Simple do-nothing method to test passing CStrings by reference
void simpleCstringRefMethod(const CString &str)
{
  const char *strPtr = str.str();
  int size = str.size();
  const char ch = strPtr[size-1];
}

/* Tests general usage of the following methods:
 *   default ctor
 *   operator=()
 *   empty()
 *   capacity()
 *   basic use of: append(), insert(), replace(), remove(), and find()
 *   equals()
 *   size()
 *   index()
 *   substr()
 *   hash()
 */
void testGeneral()
{
    CString str;

    ASSERT_TRUE(str.empty(), "String should be considered empty");
    ASSERT_EQUALS(str.getCapacity(), CString::DEFAULT_CAPACITY, "Incorrect capacity");

    str = "Hello";
    unsigned int hash1 = str.hash();
    str.append("World");  // HelloWorld
    str.insert("XYZ", 5); // HelloXYZWorld
    str.replace(" ", 5);  // Hello YZWorld
    str.remove(6, 2);     // Hello World
    unsigned int hash2 = str.hash();

    ASSERT_TRUE(str.equals("Hello World"), str.str());
    ASSERT_FALSE(str.equals("World Hello"), str.str());

    ASSERT_NOT_EQUALS(hash1, hash2, "Hash should have been recalculated when the str changed");

    ASSERT_EQUALS(str.size(), 11, "Incorrect Hello World str size");

    ASSERT_EQUALS(str.find("Wo"), 6, "Incorrect location of \"Wo\"");

    ASSERT_EQUALS(str.index(4), 'o', "Incorrect indexing");

    ASSERT_EQUALS(str.find("XYZ"), CString::NPOS, "\"XYZ\" should NOT be found");

    CString str2 = str.substr(3, 5);
    ASSERT_TRUE(str2.equals("lo Wo"), str.str());

    str.clear();
    ASSERT_EQUALS(str.size(), 0, "cleared size should be 0");
    ASSERT_TRUE(str.equals(""), "cleared str");
}

// Tests both append and operator+=
void testAppend()
{
    CString str;

    // Append char *, left pad 2
    str.append("Hello", 7);
    ASSERT_TRUE(str.equals("  Hello"), str.str());
    ASSERT_EQUALS(str.size(), 7, "Append char* length");

    // Append char, right pad 1
    str.append('a', 6, false, 5);
    ASSERT_TRUE(strcmp("  Helloaaaaa ", str.str()) == 0, str.str());
    ASSERT_EQUALS(str.size(), 13, "Append char length");

    // Append int
    str.clear();
    str.append(18, 0, true, 4);
    ASSERT_TRUE(str.equals("18181818"), str.str());
    ASSERT_EQUALS(str.size(), 8, "Append int length");

    // Append float
    str.append((float) 22/7);
    ASSERT_TRUE(str.equals("181818183.14286"), str.str());
    ASSERT_EQUALS(str.size(), 15, "Append float length");
    str.append((float) 22/7, 2);
    ASSERT_TRUE(str.equals("181818183.142863.14"), str.str());
    ASSERT_EQUALS(str.size(), 19, "Append float length 2 decimal places");

    // Append long
    str.clear();
    str.append((long) 1234567890, 0, true, 5);
    ASSERT_TRUE(str.equals("12345678901234567890123456789012345678901234567890"), str.str());
    ASSERT_EQUALS(str.size(), 50, "Append long length");

    // Append boolean
    str.clear();
    str.append(true, false, 0, false, 2);
    ASSERT_TRUE(str.equals("11"), str.str());
    ASSERT_EQUALS(str.size(), 2, "Append bool true length");
    str.append(false, false, 0, false, 3);
    ASSERT_TRUE(str.equals("11000"), str.str());
    ASSERT_EQUALS(str.size(), 5, "Append bool false length");
    str.append(true, true);
    ASSERT_TRUE(str.equals("11000true"), str.str());
    ASSERT_EQUALS(str.size(), 9, "Append bool true text length");
    str.append(false, true);
    ASSERT_TRUE(str.equals("11000truefalse"), str.str());
    ASSERT_EQUALS(str.size(), 14, "Append bool false text length");

    // Append hex
    str.clear();
    str.appendHex(4293844428);
    ASSERT_TRUE(str.equals("FFEEDDCC"), str.str());
    ASSERT_EQUALS(str.size(), 8, "Append hex length");

    // Append CString
    str.clear();
    str.append("Hello ");
    CString str2("World");
    str.append(str2);
    ASSERT_TRUE(str.equals("Hello World"), str.str());
    ASSERT_EQUALS(str.size(), 11, "Append CString length");

    // Test the minWidth padding
    str.clear();
    str.append("Hello", 3);
    ASSERT_TRUE(str.equals("Hello"), str.str());
    ASSERT_EQUALS(str.size(), 5, "Append CString minWidth < length");

    str.append("World", 6);
    ASSERT_TRUE(str.equals("Hello World"), str.str());
    ASSERT_EQUALS(str.size(), 11, "Append CString left pad to 6 spaces");

    str.append("!", 10, false);
    ASSERT_TRUE(str.equals("Hello World!         "), str.str());
    ASSERT_EQUALS(str.size(), 21, "Append CString right pad to 10 spaces");

    str.clear();
    str.append(10, 5);
    ASSERT_TRUE(str.equals("   10"), str.str());
    ASSERT_EQUALS(str.size(), 5, "Append CString pad int");

    str.append((float) 22/7, 2, 6);
    ASSERT_TRUE(str.equals("   10  3.14"), str.str());
    ASSERT_EQUALS(str.size(), 11, "Append CString pad float");

    str.append(true, false, 2, false);
    ASSERT_TRUE(str.equals("   10  3.141 "), str.str());
    ASSERT_EQUALS(str.size(), 13, "Append CString pad bool");

    // Test the operator+=
    str.clear();
    str += "Hello";
    str += ' ';
    str += str2;
    ASSERT_TRUE(str.equals("Hello World"), str.str());
    ASSERT_EQUALS(str.size(), 11, "Append += length");

    float f = 2.2;
    long l = 3;
    str += 1;
    str += f;
    str += l;
    ASSERT_TRUE(str.equals("Hello World12.200003"), str.str());
    ASSERT_EQUALS(str.size(), 20, "Append += float, long length");
}

void testInsert()
{
  //
  // The insert methods with char, int, long, float, bool, and
  // hex are tested in append, since the append methods for these
  // types just directly call the corresponding insert method
  //

  CString str1("test string");

  str1.insert("this is a ");
  ASSERT_TRUE(str1.equals("this is a test string"), str1.str());
  ASSERT_EQUALS(str1.size(), 21, "insert size, front of string");

  str1.insert("normal ", 10);
  ASSERT_TRUE(str1.equals("this is a normal test string"), str1.str());
  ASSERT_EQUALS(str1.size(), 28, "insert size, middle of string");

  str1.insert(", nothing special", CString::NPOS);
  ASSERT_TRUE(str1.equals("this is a normal test string, nothing special"), str1.str());
  ASSERT_EQUALS(str1.size(), 45, "insert size, end of string");

  CString str2;
  str2.insert(" and nothing else");
  str1.insert(str2, CString::NPOS);

  ASSERT_TRUE(str1.equals("this is a normal test string, nothing special and nothing else"), str1.str());
  ASSERT_EQUALS(str1.size(), 62, "insert CString size, end of string");

  const char *strPtr = str1.str();
  ASSERT_EQUALS(strPtr[str1.size()], '\0', "insert end of line");

  // test minWidth padding
  CString str3("Hello");
  str3.insert("World", CString::NPOS, 6);
  ASSERT_TRUE(str3.equals("Hello World"), str3.str());
  ASSERT_EQUALS(str3.size(), 11, "insert size, left pad to 6 chars");

  str3.insert("not so friendly", 6, 16, false);
  ASSERT_TRUE(str3.equals("Hello not so friendly World"), str3.str());
  ASSERT_EQUALS(str3.size(), 27, "insert size, right pad");
}

void testFind()
{
  CString str("aabbccddeeaabbccdd");

  //
  // find - forward
  //
  ASSERT_EQUALS(str.find("aa"), 0, "find beginning");
  ASSERT_EQUALS(str.find("aa", 1), 10, "find again in middle");
  ASSERT_EQUALS(str.find("aa", 10), 10, "find index in middle");
  ASSERT_EQUALS(str.find("aa", 11), CString::NPOS, "find not found index");

  ASSERT_EQUALS(str.find("aabbccddeeaabbccdd"), 0, "find entire string");
  ASSERT_EQUALS(str.find('e'), 8, "find char");

  ASSERT_EQUALS(str.find("aabbccddf"), CString::NPOS, "find not found");
  ASSERT_EQUALS(str.find("aabbccddf", 1), CString::NPOS, "find not found");

  ASSERT_EQUALS(str.find("aabbccddee"), 0, "find");
  ASSERT_EQUALS(str.find("aabbccddee", 1), CString::NPOS, "find");
  ASSERT_THROWS(str.find("aabbccddee", 10), CStringInvalidArgException, "find should throw");

  //
  // rfind - reverse, from end of string
  //
  ASSERT_EQUALS(str.rfind("aa"), 10, "rfind beginning");
  ASSERT_EQUALS(str.rfind("aa", 9), 0, "rfind middle");
  ASSERT_EQUALS(str.rfind('e'), 9, "rfind char");
  ASSERT_THROWS(str.rfind("aabbccddee", 10), CStringInvalidArgException, "rfind should throw");

}

void testReplace()
{
  CString str("0123456789abcdefghij");

  str.replace("3210");
  ASSERT_TRUE(str.equals("3210456789abcdefghij"), str.str());
  ASSERT_EQUALS(str.size(), 20, "replace size default");

  str.replace("cba", 10);
  ASSERT_TRUE(str.equals("3210456789cbadefghij"), str.str());
  ASSERT_EQUALS(str.size(), 20, "replace size middle");

  // increase the str size
  str.replace("ooo", 10, 2);
  ASSERT_TRUE(str.equals("3210456789oooadefghij"), str.str());
  ASSERT_EQUALS(str.size(), 21, "replace size 2 with 3");

  // decrease the str size
  str.replace("ppp", 10, 9);
  ASSERT_TRUE(str.equals("3210456789pppij"), str.str());
  ASSERT_EQUALS(str.size(), 15, "replace size 9 with 3");

  // replace off the end of the string
  str.replace("qqq", 13);
  ASSERT_TRUE(str.equals("3210456789pppqqq"), str.str());
  ASSERT_EQUALS(str.size(), 16, "replace off end with 3");

  // check correct use of NPOS
  str.replace("xyz", 10, CString::NPOS);
  ASSERT_TRUE(str.equals("3210456789xyz"), str.str());
  ASSERT_EQUALS(str.size(), 13, "replace NPOS");

  // replace entire string
  str.replace("replaced", 0, CString::NPOS);
  ASSERT_TRUE(str.equals("replaced"), str.str());
  ASSERT_EQUALS(str.size(), 8, "replace NPOS");

  // replace with an empty CString
  CString emptyStr;
  str.replace(emptyStr, 6, CString::NPOS);
  ASSERT_TRUE(str.equals("replac"), str.str());
  ASSERT_EQUALS(str.size(), 6, "replace empty Cstring");
}

void testSubstr()
{
  // TODO finish this
}

void testUpperLowerCase()
{
  CString str("0123456789abcdefghijklmnopqrstuvwxyz");

  str.toupper();
  ASSERT_TRUE(str.equals("0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ"), str.str());

  str.tolower();
  ASSERT_TRUE(str.equals("0123456789abcdefghijklmnopqrstuvwxyz"), str.str());
}

void testIsNumber()
{
  CString str("123456789");
  ASSERT_TRUE(str.isNumber(), "isNumber: normal number");

  str = "   1234   ";
  ASSERT_TRUE(str.isNumber(), "isNumber: leading and trailing spaces");

  str = " -1234";
  ASSERT_TRUE(str.isNumber(), "isNumber: leading spaces with minus sign");

  str = "1,234,567";
  ASSERT_TRUE(str.isNumber(), "isNumber: thousands commas");

  str = "1234.5678";
  ASSERT_TRUE(str.isNumber(), "isNumber: decimal place");

  str = "1,234.5678";
  ASSERT_TRUE(str.isNumber(), "isNumber: decimal place and thousands comma");

  str = "   -1,234.5678 ";
  ASSERT_TRUE(str.isNumber(), "isNumber: leading spaces, negative, decimal place, and thousands comma");

  str = "1234-";
  ASSERT_FALSE(str.isNumber(), "isNumber: trailing minus sign");

  str = " 1234 5";
  ASSERT_FALSE(str.isNumber(), "isNumber: spaces in the middle");

  str = " 1234 ,5";
  ASSERT_FALSE(str.isNumber(), "isNumber: spaces in the middle after comma");

  str = " 1234 .5";
  ASSERT_FALSE(str.isNumber(), "isNumber: spaces in the middle after decimal");

  str = "1234a";
  ASSERT_FALSE(str.isNumber(), "isNumber: negative case");
}

void testRemove()
{
  CString str("1234567890");

  str.remove(5, 3);
  ASSERT_TRUE(str.equals("1234590"), str.str());
  ASSERT_EQUALS(str.size(), 7, "remove 3 from middle");

  str.remove(0, 2);
  ASSERT_TRUE(str.equals("34590"), str.str());
  ASSERT_EQUALS(str.size(), 5, "remove 2 from begining");

  str.remove(2); // remove to end of str
  ASSERT_TRUE(str.equals("34"), str.str());
  ASSERT_EQUALS(str.size(), 2, "remove NPOS");

  str.remove(0); // remove entire str
  ASSERT_TRUE(str.equals(""), str.str());
  ASSERT_EQUALS(str.size(), 0, "remove entire str");
}

void testIterators()
{
    CString str("Hello World");

    //
    // Forward Iterator
    //
    CStringIterator iter1;
    ASSERT_FALSE(iter1.hasNext(), "Iterator not initialized");
    ASSERT_THROWS_STR(iter1.next(),
                      CStringIteratorException,
                      "Invalid iterator",
                      "Iterator::next() should have thrown");

      // This line tests operator=() and CString& ctor
    iter1 = str.iterator();
    ASSERT_TRUE(iter1.hasNext(), "Iterator should be initialized");

    ASSERT_EQUALS(iter1.next(), 'H', "iter1.next() should be \'H\'");
    ASSERT_TRUE(iter1.hasNext(),     "iter1.hasNext() should still be valid");
    ASSERT_EQUALS(iter1.next(), 'e', "iter1.next() should be \'e\'");
    ASSERT_EQUALS(iter1.next(), 'l', "iter1.next() should be \'l\'");
    ASSERT_EQUALS(iter1.next(), 'l', "iter1.next() should be \'l\'");
    ASSERT_EQUALS(iter1.next(), 'o', "iter1.next() should be \'o\'");
    ASSERT_EQUALS(iter1.next(), ' ', "iter1.next() should be \' \'");
    ASSERT_TRUE(iter1.hasNext(),     "iter1.hasNext() should still be valid");

      // This line tests the copy ctor
    CStringIterator iter2(iter1);
    ASSERT_EQUALS(iter2.next(), 'W', "iter2.next() should be \'W\'");
    ASSERT_EQUALS(iter2.next(), 'o', "iter2.next() should be \'o\'");
    ASSERT_EQUALS(iter2.next(), 'r', "iter2.next() should be \'r\'");
    ASSERT_EQUALS(iter2.next(), 'l', "iter2.next() should be \'l\'");
    ASSERT_TRUE(iter2.hasNext(),     "iter2.hasNext() should still be valid");
    ASSERT_EQUALS(iter2.next(), 'd', "iter2.next() should be \'d\'");
    ASSERT_FALSE(iter2.hasNext(),    "iter2.hasNext() should no longer be valid");

    //
    // Reverse Iterator
    //
    CStringReverseIterator riter1;
    ASSERT_FALSE(riter1.hasNext(), "ReverseIterator not initialized");
    ASSERT_THROWS_STR(riter1.next(),
                      CStringIteratorException,
                      "Invalid iterator",
                      "ReverseIterator::next() should have thrown");

      // This line tests operator=() and CString& ctor
    riter1 = str.riterator();
    ASSERT_TRUE(riter1.hasNext(), "ReverseIterator should be initialized");

    ASSERT_EQUALS(riter1.next(), 'd', "riter1.next() should be \'d\'");
    ASSERT_TRUE(riter1.hasNext(),     "riter1.hasNext() should still be valid");
    ASSERT_EQUALS(riter1.next(), 'l', "riter1.next() should be \'l\'");
    ASSERT_EQUALS(riter1.next(), 'r', "riter1.next() should be \'r\'");
    ASSERT_EQUALS(riter1.next(), 'o', "riter1.next() should be \'o\'");
    ASSERT_EQUALS(riter1.next(), 'W', "riter1.next() should be \'W\'");
    ASSERT_EQUALS(riter1.next(), ' ', "riter1.next() should be \' \'");
    ASSERT_TRUE(riter1.hasNext(),     "riter1.hasNext() should still be valid");

      // This line tests the copy ctor, iter1 is currently pointing to W
    CStringReverseIterator riter2(iter1);
    ASSERT_EQUALS(riter2.next(), 'W', "riter2.next() should be \'W\'");
    ASSERT_EQUALS(riter2.next(), ' ', "riter2.next() should be \' \'");
    ASSERT_EQUALS(riter2.next(), 'o', "riter2.next() should be \'o\'");
    ASSERT_EQUALS(riter2.next(), 'l', "riter2.next() should be \'l\'");
    ASSERT_EQUALS(riter2.next(), 'l', "riter2.next() should be \'l\'");
    ASSERT_EQUALS(riter2.next(), 'e', "riter2.next() should be \'e\'");
    ASSERT_TRUE(riter2.hasNext(),     "riter2.hasNext() should still be valid");
    ASSERT_EQUALS(riter2.next(), 'H', "riter2.next() should be \'H\'");
    ASSERT_FALSE(riter2.hasNext(),    "riter2.hasNext() should no longer be valid");

    // Test reset
    iter1.reset();
    ASSERT_TRUE(iter1.hasNext(), "Iterator should be initialized after reset");
    ASSERT_EQUALS(iter1.next(), 'H', "iter1.next() should be \'H\' after reset");

    riter1.reset();
    ASSERT_TRUE(riter1.hasNext(), "ReverseIterator should be initialized after reset");
    ASSERT_EQUALS(riter1.next(), 'd', "riter1.next() should be \'d\' after reset");

    // The typical way to use an iterator, with an infinite loop protection
    iter1.reset();
    int i;
    //while(iter1.hasNext())
    for(i = 0; i < str.size() && iter1.hasNext(); i++)
    {
      const char c = iter1.next();
    }

    ASSERT_FALSE(iter1.hasNext(), "iter hasNext after loop");
    ASSERT_EQUALS(i, str.size(), "iter num iterations in loop");
}

void testTokenizer()
{
  CString str("This is a \t test    string \t");
  CStringTokenizer token(str, " \t");

  CString tokenStr = token.next();
  ASSERT_TRUE(tokenStr.equals("This"), tokenStr.str());

  tokenStr = token.next();
  ASSERT_TRUE(tokenStr.equals("is"), tokenStr.str());

  tokenStr = token.next();
  ASSERT_TRUE(tokenStr.equals("a"), tokenStr.str());

  tokenStr = token.next();
  ASSERT_TRUE(tokenStr.equals("test"), tokenStr.str());

  tokenStr = token.next();
  ASSERT_TRUE(tokenStr.equals("string"), tokenStr.str());

  tokenStr = token.next();
  ASSERT_EQUALS(tokenStr.size(), 0, "End token should be empty");

  CStringTokenizer token2(str, "x");
  token2.next();
  token2.next();
}

// Tests that the capacity gets incremented at the correct times
// Should be tested with append(same as +=), insert, and replace
void testCapacity()
{
  CString str(20);
  ASSERT_EQUALS(str.getCapacity(), 20, "Incorrect capacity, append");

  //
  // Test with append(same as +=)
  //
  str += "1234567890"; // size = 10
  ASSERT_EQUALS(str.getCapacity(), 20, "Incorrect capacity, append");

  str += "1234567890"; // size = 20
  ASSERT_EQUALS(str.getCapacity(), 20, "Incorrect capacity, append");

  // The capacity should be incremented after this call
  str.append("1"); // size = 21
  ASSERT_EQUALS(str.getCapacity(), 40, "Incorrect capacity, append");

  //
  // Test with insert
  //
  str.insert("234567890", CString::NPOS); // size = 30
  ASSERT_EQUALS(str.getCapacity(), 40, "Incorrect capacity, insert");

  str.insert("12345678901", CString::NPOS); // size = 41
  ASSERT_EQUALS(str.getCapacity(), 60, "Incorrect capacity, insert");

  //
  // Test with replace
  //
  str.replace("abcdefghij", str.size()-1); // replace 1 with 10 => size = 50
  ASSERT_EQUALS(str.getCapacity(), 60, "Incorrect capacity, replace");

  str.replace("zyxwvutsrqpo", str.size()-1); // replace 1 with 12 => size = 61
  ASSERT_EQUALS(str.getCapacity(), 80, "Incorrect capacity, replace");
}

void testReferenceCounting()
{
  CString *str1 = new CString("Test Str");
  TestCString tstr1(*str1);
  ASSERT_EQUALS(tstr1.getReferences(), 2, "invalid num refs, copy ctor");

  // Test the the equals operator decrements the previous ref and increments the new ref
  CString *str2 = new CString("another str");
  TestCString tstr2(*str2);
  ASSERT_EQUALS(tstr2.getReferences(), 2, "invalid num refs, equals operator 2 refs");
  str2->operator=(*str1);
  ASSERT_EQUALS(tstr2.getReferences(), 1, "invalid num refs, equals operator 1 ref");
  ASSERT_EQUALS(tstr1.getReferences(), 3, "invalid num refs, equals operator 3 refs");

  // Test that the destructor decrements the ref
  delete str1;
  ASSERT_EQUALS(tstr1.getReferences(), 2, "invalid num refs, dtor 2 refs");

  delete str2;
  ASSERT_EQUALS(tstr1.getReferences(), 1, "invalid num refs, dtor 1 ref");

  // Test that the Forward Iterators correctly handle the refs
  CStringIterator *iter1 = new CStringIterator(tstr1);
  ASSERT_EQUALS(tstr1.getReferences(), 2, "invalid num refs, forward iterator, CString ctor");

  CStringIterator *iter2 = new CStringIterator();
  iter2->operator=(*iter1);
  ASSERT_EQUALS(tstr1.getReferences(), 3, "invalid num refs, forward iterator, equals operator");

  CStringIterator *iter3 = new CStringIterator(tstr1.iterator());
  ASSERT_EQUALS(tstr1.getReferences(), 4, "invalid num refs, forward iterator, copy ctor");

  delete iter1;
  delete iter2;
  delete iter3;
  ASSERT_EQUALS(tstr1.getReferences(), 1, "invalid num refs, delete forward iterators");

  // Test that the Reverse Iterators correctly handle the refs
  CStringReverseIterator *riter1 = new CStringReverseIterator(tstr1);
  ASSERT_EQUALS(tstr1.getReferences(), 2, "invalid num refs, reverse iterator, CString ctor");

  CStringReverseIterator *riter2 = new CStringReverseIterator();
  riter2->operator=(*riter1);
  ASSERT_EQUALS(tstr1.getReferences(), 3, "invalid num refs, reverse iterator, equals operator");

  CStringReverseIterator *riter3 = new CStringReverseIterator(tstr1.iterator());
  ASSERT_EQUALS(tstr1.getReferences(), 4, "invalid num refs, reverse iterator, copy ctor");

  delete riter1;
  delete riter2;
  delete riter3;
  ASSERT_EQUALS(tstr1.getReferences(), 1, "invalid num refs, delete reverse iterators");

  // These may be a bit redundant, but just check calling methods with CStrings
  simpleCstringObjectMethod(tstr1);
  ASSERT_EQUALS(tstr1.getReferences(), 1, "invalid num refs, method call pass by value");

  simpleCstringRefMethod(tstr1);
  ASSERT_EQUALS(tstr1.getReferences(), 1, "invalid num refs, method call pass by reference");
}

void testExceptions()
{
  CString str("This is a normal string");

  //
  // substr
  //
  ASSERT_THROWS_STR(str.substr(str.size()+1, 1),
                    CStringOutOfBoundsException,
                    "CString::substr index > size",
                    "no substr exception thrown");

  ASSERT_THROWS_STR(str.substr(str.size()-1, 3),
                    CStringOutOfBoundsException,
                    "CString::substr index+numChars > size",
                    "no substr exception thrown");

  //
  // index and operator[]
  //
  ASSERT_THROWS_STR(str.index(str.size()+1),
                    CStringOutOfBoundsException,
                    "CString::index index > size",
                    "no index exception thrown");

  ASSERT_THROWS_STR(str[str.size()+1],
                    CStringOutOfBoundsException,
                    "CString::index index > size",
                    "no operator[] exception thrown");

  //
  // find
  //
  ASSERT_THROWS_STR(str.find('x', str.size()+10),
                    CStringOutOfBoundsException,
                    "CString::find index > size",
                    "no find char exception thrown");

  ASSERT_THROWS_STR(str.find("x", str.size()+11),
                    CStringOutOfBoundsException,
                    "CString::find index > size",
                    "no find str exception thrown");

  ASSERT_THROWS_STR(str.find("xyz", str.size()-1),
                    CStringInvalidArgException,
                    "CString::find index+length > size",
                    "no find str exception thrown");

  //
  // insert
  //
  ASSERT_THROWS_STR(str.insert("xyz", str.size()+1),
                    CStringOutOfBoundsException,
                    "CString::insert index > size",
                    "no insert exception thrown");

  ASSERT_NOT_THROWS(str.insert("xyz", CString::NPOS),
                    "insert should not throw exception with index=NPOS");

    // Test inserting off the end of the string
  ASSERT_NOT_THROWS(str.insert("xyz", str.size()-1),
                    "insert should not throw exception");

  //
  // remove
  //
  ASSERT_THROWS_STR(str.remove(str.size()+1, 1),
                    CStringOutOfBoundsException,
                    "CString::remove index > size",
                    "no remove exception thrown");

  ASSERT_THROWS_STR(str.remove(str.size()-1, 3),
                    CStringInvalidArgException,
                    "CString::remove numChars+index > size",
                    "no remove exception thrown");

  //
  // replace
  //
  ASSERT_THROWS_STR(str.replace("x", str.size()+1),
                    CStringOutOfBoundsException,
                    "CString::replace index > size",
                    "no replace exception thrown");

  //
  // iterators
  //
  CStringIterator iter;
  ASSERT_THROWS_STR(iter.next(),
                    CStringIteratorException,
                    "Invalid iterator",
                    "no forward iterator::next exception thrown");
  ASSERT_THROWS_STR(iter.reset(),
                    CStringIteratorException,
                    "Invalid iterator",
                    "no forward iterator::reset exception thrown");

  CStringReverseIterator riter;
  ASSERT_THROWS_STR(riter.next(),
                    CStringIteratorException,
                    "Invalid iterator",
                    "no reverse iterator::next exception thrown");
  ASSERT_THROWS_STR(riter.reset(),
                    CStringIteratorException,
                    "Invalid iterator",
                    "no reverse iterator::reset exception thrown");

  //
  // autoIncrement
  //
  CString str2(5, false);
  ASSERT_THROWS_STR(str2.insert("123456"),
                    CStringOutOfBoundsException,
                    "Trying to increment capacity with autoCapacity set false",
                    "incrementCapacity with autoIncrement false");

}

int main(int argc, char **argv)
{
    TEST_CASE(testGeneral());

    TEST_CASE(testAppend());

    TEST_CASE(testInsert());

    TEST_CASE(testReplace());

    TEST_CASE(testRemove());

    TEST_CASE(testFind());

    TEST_CASE(testSubstr());

    TEST_CASE(testUpperLowerCase());

    TEST_CASE(testIsNumber());

    TEST_CASE(testIterators());

    TEST_CASE(testTokenizer());

    TEST_CASE(testCapacity());

    TEST_CASE(testReferenceCounting());

    TEST_CASE(testExceptions());

    std::cout << "\nTests complete\n" << std::endl;

    return 1;
}
