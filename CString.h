#ifndef CSTRING_H
#define CSTRING_H

#include <string.h>

class CStringData
{
	public:
    typedef unsigned int size_type;

		CStringData(CStringData::size_type initialCapacity, bool autoCapacity);
		CStringData(const char *str, CStringData::size_type initialCapacity, bool autoCapacity);
		CStringData(const char *str, CStringData::size_type length, CStringData::size_type initialCapacity, bool autoCapacity);

		~CStringData();

    unsigned int references_;
		CStringData::size_type initialCapacity_;
		CStringData::size_type capacity_;
    bool autoCapacity_;
    CStringData::size_type size_;
    char *str_;
};

class CStringIterator;
class CStringReverseIterator;

class CString
{
  public:
    typedef unsigned int size_type;
    static const size_type DEFAULT_CAPACITY;
    static const char DEFAULT_PAD_CHAR;
    static const size_type NPOS;

    CString(size_type initialCapacity = CString::DEFAULT_CAPACITY, bool autoCapacity = true);
    CString(const char *str, size_type initialCapacity = CString::DEFAULT_CAPACITY, bool autoCapacity = true);
    CString(const char *str, size_type length, size_type initialCapacity, bool autoCapacity = true);
    CString(const CString &copy);
    virtual ~CString();

    inline bool empty()            const { return data_->size_ < 1; };
    inline size_type size()        const { return data_->size_; };
    inline const char *str()       const { return data_->str_; };
    inline size_type getCapacity() const { return data_->capacity_; };
    inline bool getAutoCapacity()  const { return data_->autoCapacity_; };
    inline void clear() { data_->size_ = 0; data_->str_[0] = '\0'; };

    // TODO CString clone() const; copy the string without reference counting
    // TODO for insert, append: allow width and left/right justify
    //      ej: str="123" w=5, left, result= "  123" or right: "123  "
    // TODO add insert signature with iterators

    CStringIterator iterator() const;
    CStringReverseIterator riterator() const;

    void toupper();
    void tolower();

    /**
     * Returns true if the stored string is any sort of number, false otherwise.
     * Examples that would return true include:
     *    "42", "100", "100.000", "2,000", "3,145.87", "-100"
     *    "100,000.00" and "100.100,00" are treated equally,
     *    notice the thousands separator and decimal point are interchangeable
     */
    bool isNumber() const;

    /**
     * Append onto the end of the string, if the resulting size > capacity, a resize will take place.
     * minWidth - for padding spaces on the left or right, depending on leftJustify.
     *            if the str to append is longer than minWidth, or if minWidth is 0,
     *            then no padding will be performed.
     * leftJustify - true: left justify spaces, false: right justify
     * append('a', 4, true, 3) will append " aaa" onto the end of the string
     * Return the number of chars appended
     */
    inline size_type append(const char ch,
                            size_type minWidth = 0,
                            bool leftJustify = true,
                            size_type count = 1)       { return insert(ch, size(), minWidth, leftJustify, count); };
    inline size_type append(int num,
                            size_type minWidth = 0,
                            bool leftJustify = true,
                            size_type count = 1)       { return insert(num, size(), minWidth, leftJustify, count); };
    inline size_type append(long num,
                            size_type minWidth = 0,
                            bool leftJustify = true,
                            size_type count = 1)       { return insert(num, size(), minWidth, leftJustify, count); };
    inline size_type append(float num,
                            size_type numDecimals = 5,
                            size_type minWidth = 0,
                            bool leftJustify = true,
                            size_type count = 1)       { return insert(num, size(), numDecimals, minWidth, leftJustify, count); };
      // if displayText == true, displays "true" or "false" else displays "0" or "1"
    inline size_type append(bool b,
                            bool displayText = false,
                            size_type minWidth = 0,
                            bool leftJustify = true,
                            size_type count = 1)       { return insert(b, size(), displayText, minWidth, leftJustify, count); };
    inline size_type appendHex(int num,
                               size_type minWidth = 0,
                               bool leftJustify = true,
                               size_type count = 1)    { return insertHex(num, size(), minWidth, leftJustify, count); };
    inline size_type append(const char *str,
                            size_type minWidth = 0,
                            bool leftJustify = true)   { return append_(str, strlen(str), minWidth, leftJustify); };
    inline size_type append(const CString &str,
                            size_type minWidth = 0,
                            bool leftJustify = true)   { return append_(str.str(), str.size(), minWidth, leftJustify); };

    /**
     * Insert at the position specified:
     *    if index = NPOS, same as append()
     *    if index > size, CStringOutOfBoundsException will be thrown
     *    if the resulting size > capacity, a resize will take place
     * minWidth - for padding spaces on the left or right, depending on leftJustify.
     *            if the str to append is longer than minWidth, or if minWidth is 0,
     *            then no padding will be performed.
     * leftJustify - true: left justify spaces, false: right justify
     * Return the number of chars inserted
     */
    size_type insert(const char ch,
                     size_type index = 0,
                     size_type minWidth = 0,
                     bool leftJustify = true,
                     size_type count = 1);
    size_type insert(int num,
                     size_type index = 0,
                     size_type minWidth = 0,
                     bool leftJustify = true,
                     size_type count = 1);
    size_type insert(long num,
                     size_type index = 0,
                     size_type minWidth = 0,
                     bool leftJustify = true,
                     size_type count = 1);
    size_type insert(float num,
                     size_type index = 0,
                     size_type numDecimals = 5,
                     size_type minWidth = 0,
                     bool leftJustify = true,
                     size_type count = 1);
      // if displayText == true, displays "true" or "false" else displays "0" or "1"
    size_type insert(bool b,
                     size_type index = 0,
                     bool displayText = false,
                     size_type minWidth = 0,
                     bool leftJustify = true,
                     size_type count = 1);
    size_type insertHex(int num,
                        size_type index = 0,
                        size_type minWidth = 0,
                        bool leftJustify = true,
                        size_type count = 1);
    inline size_type insert(const char *str,
                            size_type index = 0,
                            size_type minWidth = 0,
                            bool leftJustify = true)    { return insert_(str, index, strlen(str), minWidth, leftJustify); };
    inline size_type insert(const CString &str,
                            size_type index = 0,
                            size_type minWidth = 0,
                            bool leftJustify = true) { return insert_(str.str(), index, str.size(), minWidth, leftJustify); };

    /**
     * Replace chars in the string at position index. Size of string may be affected.
     * Number of chars to be replaced is one of the following:
     *    - if length specified then length
     *    - if length = NPOS, from index to end of string
     *    - if length not specified, the size of the string passed in.
     * Examples:
     *   str = "0123456789", replace("xyz", 2, 5) results in "01xyz789", replace 5 chars with 3
     *   str = "0123456789", replace("xyz", 2, 2) results in "01xyz456789", replace 2 chars with 3
     *   str = "0123456789", replace("xyz", 2, NPOS) results in "01xyz", replace to end of string with 3 chars
     *   str = "0123456789", replace("xyz", 2) results in "01xyz56789", replace chars with chars passed in
     *
     * Replacing off the end of the string is allowed, If the resulting size > capacity, a resize will take place.
     * Examples:
     *   capacity =  5, str = "abcd", replace("xyz", 3) results in "abcxyz", capacity will be incremented
     *   capacity = 10, str = "abcd", replace("xyz", 3) results in "abcxyz", capacity not incremented
     * Return the number of chars replaced
     */
    size_type replace(const char ch, size_type index = 0, size_type count = 1, size_type length = 0);
    inline size_type replace(const char *str, size_type index = 0, size_type length = 0)    { return replace_(str, index, length); };
    inline size_type replace(const CString &str, size_type index = 0, size_type length = 0) { return replace_(str.str(), index, length); };

    /**
     * Remove chars from the string starting at index until index+numChars.
     * If numChars is NPOS, remove until end of string
     * Return the number of chars removed
     */
    size_type remove(size_type index, size_type numChars = CString::NPOS);

    /**
     * Return the index of the str/char passed in, or NPOS if not found
     */
    inline size_type find(const char ch, size_type index = 0)      const { return find_(&ch, index, 1); };
    inline size_type find(const char *str, size_type index = 0)    const { return find_(str, index, strlen(str)); };
    inline size_type find(const CString &str, size_type index = 0) const { return find_(str.str(), index, str.size()); };

    /**
     * Return the index of the str/char passed in, or NPOS if not found
     * Starts from the end of the string and goes towards the beginning
     * The index is counted from the end of the string
     * Examples:
     *   str="aabbccaabbcc", rfind("ab") returns 7
     *   str="aabbccaabbcc", rfind("ab", 6) returns 1
     */
    inline size_type rfind(const char ch, size_type index = 0)      const { return rfind_(&ch, index, 1); };
    inline size_type rfind(const char *str, size_type index = 0)    const { return rfind_(str, index, strlen(str)); };
    inline size_type rfind(const CString &str, size_type index = 0) const { return rfind_(str.str(), index, str.size()); };

    /**
     * Return the character at the specified index
     */
    const char index(size_type indeX) const; // returns the same as operator[]

    /**
     * Returns a hash of the string to be used in hash tables, maps, etc
     * The Hash is recalculated each time this method is called.
     */
    unsigned int hash() const;

    /**
     * Return a CString substring starting at index and spanning numChars characters
     * if numChars is NPOS, then return from index to end of string
     */
    CString substr(size_type index, size_type numChars = CString::NPOS) const;

    /**
     * Return true if the str is the same as this CString, false otherwise
     * Does the same as the operator==
     */
    inline bool equals(const char *str)    const { return strcmp(data_->str_, str) == 0 ? true : false; };
    inline bool equals(const CString &str) const { return equals(str.data_->str_); };

#ifndef NO_OPERATORS
    // The += operator is the same as calling append
    inline size_type operator+=(const char ch)       { return append(ch); };
    inline size_type operator+=(int num)             { return append(num); };
    inline size_type operator+=(long num)            { return append(num); };
    inline size_type operator+=(float num)           { return append(num); };
    inline size_type operator+=(const char *str)     { return append(str); };
    inline size_type operator+=(const CString &str)  { return append(str); };

    void operator=(const CString &str);
    inline int operator<(const CString &rhs)      const { return strcmp(str(), rhs.str()); }; // needed for std::map
    inline bool operator==(const char *str)       const { return equals(str); };
    inline bool operator==(const CString &str)    const { return equals(str.data_->str_); };
    inline bool operator!=(const char *str)       const { return ! equals(str); };
    inline bool operator!=(const CString &str)    const { return ! equals(str.data_->str_); };
    inline const char operator[](size_type indeX) const { return index(indeX); };
#endif

    // TODO
    //void operator+()
    //void operator<<()
    //void operator>>()

  protected:
    inline bool checkCapacity(size_type size) const { return (size + data_->size_ > data_->capacity_) ? true : false; };
    void incrementCapacity(size_type size);
    void decrementReference();
    size_type find_(const char *str, size_type index, size_type length) const;
    size_type rfind_(const char *str, size_type index, size_type length) const;
    size_type append_(const char *str, size_type length, size_type minWidth, bool leftJustify);
    size_type insert_(const char *str, size_type index, size_type length, size_type minWidth, bool leftJustify);
    size_type replace_(const char *str, size_type index, size_type length);
    void copy(const CString &copY);

    CStringData *data_;
    char padChar_;

    friend class CStringBaseIterator;
};


// TODO do we want a version that does a copy on write?

/**
 * Tokenize a CString based on any of the chars in the token string.
 * The original CString or char* is not modified. If a CString is
 * passed in for the input string, the tokenizer refers to the same
 * data, so changing the original string in between calls to next()
 * may cause unexpected behaviour. Each call to next() returns a new
 * CString, when the end of the input string is reached, an empty
 * CString is returned.
 */
class CStringTokenizer
{
  public:
    CStringTokenizer(const char *str,     const char *token);
    CStringTokenizer(const char *str,     const CString &token);
    CStringTokenizer(const CString &str,  const char *token);
    CStringTokenizer(const CString &str,  const CString &token);
    virtual ~CStringTokenizer();
    CString next();
#ifndef NO_OPERATORS
    inline CString operator()() { return next(); }; // same as next()
#endif

    static const CString whitespace;

  private:
    // these ctors are disallowed
    CStringTokenizer();
    CStringTokenizer(const CStringTokenizer &cst);

    CString inputStr_;
    CString token_;
    CString::size_type index_;
};

class CStringException
{
	public:
		CStringException(char *msg) : msg_(msg) {};
		inline const CString what() const {return msg_; };

	private:
    CString msg_;
};

// This is just a marker exception to help distinguish
class CStringInvalidArgException : public CStringException
{
	public:
		CStringInvalidArgException(char *msg) : CStringException(msg) {};
};

// This is just a marker exception to help distinguish
class CStringOutOfBoundsException : public CStringException
{
	public:
		CStringOutOfBoundsException(char *msg) : CStringException(msg) {};
};

// This is just a marker exception to help distinguish
class CStringIteratorException : public CStringException
{
	public:
		CStringIteratorException(char *msg) : CStringException(msg) {};
};

class CStringBaseIterator
{
  public:
    CStringBaseIterator();
    CStringBaseIterator(const CString &str);
    CStringBaseIterator(const CStringBaseIterator &copy);
#ifndef NO_OPERATORS
    void operator=(const CStringBaseIterator &);
#endif

    virtual ~CStringBaseIterator();

    inline CString::size_type currentIndex() { return index_; };
    virtual bool hasNext() const = 0;
    virtual const char next() = 0;
    virtual void reset() = 0;
    virtual inline bool isValid() const { return data_ == NULL ? false : true; };
    virtual void isValidThrow() const;

    // TODO these should just compare the index_ if pointing to the same data_
    // operator==()
    // operator<()
    // operator>()

  protected:
    CStringData *data_;
    int index_;
};

class CStringIterator : public CStringBaseIterator
{
  public:
    CStringIterator();
    CStringIterator(const CString &str);
    CStringIterator(const CStringBaseIterator &copy);

    ~CStringIterator();

    virtual bool hasNext() const;
    virtual const char next();
    virtual void reset();
};

class CStringReverseIterator : public CStringBaseIterator
{
  public:
    CStringReverseIterator();
    CStringReverseIterator(const CString &str);
    CStringReverseIterator(const CStringBaseIterator &copy);

    ~CStringReverseIterator();

    virtual bool hasNext() const;
    virtual const char next();
    virtual void reset();
};


#endif // CSTRING_H
