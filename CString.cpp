
#include <stdio.h>
#include <string.h>

#include "CString.h"

const CString::size_type CString::DEFAULT_CAPACITY = 64;
const char CString::DEFAULT_PAD_CHAR = ' ';
const CString::size_type CString::NPOS = 0xffffffff;
const CString CStringTokenizer::whitespace = " \t";


//----------------------------------------------------------------------
//
//    Base Iterator implementation
//
//----------------------------------------------------------------------

/** @brief CStringBaseIterator
  *
  */
CStringBaseIterator::CStringBaseIterator() :
    data_(NULL),
    index_(-1)
{
}

/** @brief CStringBaseIterator
  *
  */
CStringBaseIterator::CStringBaseIterator(const CString &str) :
    data_(str.data_),
    index_(0)
{
  if(data_ != NULL)
  {
    data_->references_++;
  }
}

/** @brief CStringBaseIterator
  *
  */
CStringBaseIterator::CStringBaseIterator(const CStringBaseIterator &copy) :
    data_(copy.data_),
    index_(copy.index_)
{
  if(data_ != NULL)
  {
    data_->references_++;
  }
}

CStringBaseIterator::~CStringBaseIterator()
{
  if(data_ != NULL)
  {
    if(data_->references_ < 2)
    {
      delete data_;
    }
    else
    {
      data_->references_--;
    }
  }
}

/** @brief operator=
  *
  */
#ifndef NO_OPERATORS
void CStringBaseIterator::operator=(const CStringBaseIterator &copy)
{
    data_ = copy.data_;
    index_ = copy.index_;
    if(data_ != NULL)
    {
      data_->references_++;
    }
}
#endif

void CStringBaseIterator::isValidThrow() const
{
  if(! isValid())
  {
    throw CStringIteratorException("Invalid iterator");
  }
}


//----------------------------------------------------------------------
//
//    Forward Iterator implementation
//
//----------------------------------------------------------------------

/** @brief CStringIterator
  *
  */
CStringIterator::CStringIterator()
{
}

/** @brief CStringIterator
  *
  */
CStringIterator::CStringIterator(const CString &str) :
    CStringBaseIterator(str)
{
}

/** @brief CStringIterator
  *
  */
CStringIterator::CStringIterator(const CStringBaseIterator &copy) :
    CStringBaseIterator(copy)
{
}

CStringIterator::~CStringIterator()
{
}

/** @brief next
  *
  */
const char CStringIterator::next()
{
  isValidThrow();

  return data_->str_[index_++];
}

/** @brief hasNext
  *
  */
bool CStringIterator::hasNext() const
{
  if(!isValid())
  {
    return false;
  }

  return (index_ < data_->size_);
}

void CStringIterator::reset()
{
  isValidThrow();

  index_ = 0;
}

//----------------------------------------------------------------------
//
//    Reverse Iterator implementation
//
//----------------------------------------------------------------------


/** @brief CStringReverseIterator
  *
  */
CStringReverseIterator::CStringReverseIterator()
{
}


/** @brief CStringReverseIterator
  *
  */
CStringReverseIterator::CStringReverseIterator(const CString &str) :
    CStringBaseIterator(str)
{
  index_ = str.size()-1;
}

/** @brief CStringReverseIterator
  *
  */
CStringReverseIterator::CStringReverseIterator(const CStringBaseIterator &copy) :
    CStringBaseIterator(copy)
{
}

CStringReverseIterator::~CStringReverseIterator()
{
}

/** @brief next
  *
  */
const char CStringReverseIterator::next()
{
  isValidThrow();

  return data_->str_[index_--];
}

/** @brief hasNext
  *
  */
bool CStringReverseIterator::hasNext() const
{
  if(!isValid())
  {
    return false;
  }

  return (index_ > -1);
}

void CStringReverseIterator::reset()
{
  isValidThrow();

  index_ = data_->size_ - 1;
}

//----------------------------------------------------------------------
//
//    CStringTokenizer implementation
//
//----------------------------------------------------------------------

CStringTokenizer::CStringTokenizer(const char *str, const char *token) :
    inputStr_(str),
    token_(token),
    index_(0)
{
}

CStringTokenizer::CStringTokenizer(const char *str, const CString &token) :
    inputStr_(str),
    token_(token),
    index_(0)
{
}

CStringTokenizer::CStringTokenizer(const CString &str, const char *token) :
    inputStr_(str),
    token_(token),
    index_(0)
{
}

CStringTokenizer::CStringTokenizer(const CString &str, const CString &token) :
    inputStr_(str),
    token_(token),
    index_(0)
{
}

// virtual
CStringTokenizer::~CStringTokenizer()
{
}

CString CStringTokenizer::next()
{
  // Check if there are no more tokens
  if(index_ == CString::NPOS)
  {
    return CString();
  }

  CString::size_type startIndex = index_;
  CString::size_type endIndex = index_;
  bool foundit = false;

  // Find the end of the current string and null terminate it
  for(int i = startIndex; i < inputStr_.size() && ! foundit; i++)
  {
    CString::size_type index = token_.find(inputStr_.index(i));
    if(index != CString::NPOS)
    {
      //inputStr_.replace('\0', i, 1, 1);
      endIndex = i;
      foundit = true;
    }
  }

  // If the token wasnt found, either we're at the end of the string,
  // or it didnt exist. Return from prevIndex to end of string
  if(!foundit)
  {
    index_ = CString::NPOS;
    return inputStr_.substr(startIndex, CString::NPOS);
  }

  // Now try to find the beginning of the next string and set index_ there
  foundit = false;
  for(int i = endIndex; i < inputStr_.size() && ! foundit; i++)
  {
    CString::size_type index = token_.find(inputStr_.index(i));
    if(index == CString::NPOS)
    {
      foundit = true;
      index_ = i;
    }
  }

  if(!foundit)
  {
    index_ = CString::NPOS;
  }

  return inputStr_.substr(startIndex, endIndex-startIndex);
}


//----------------------------------------------------------------------
//
//    CStringData implementation
//
//----------------------------------------------------------------------

CStringData::CStringData(CStringData::size_type initialCapacity, bool autoCapacity) :
    references_(1),
    initialCapacity_(initialCapacity),
    capacity_(initialCapacity),
    autoCapacity_(autoCapacity),
    size_(0)
{
    // always make it 1 char larger for the end of line
    str_ = new char[capacity_+1];
    str_[0] = '\0';
}

CStringData::CStringData(const char *str,
                         CStringData::size_type initialCapacity,
                         bool autoCapacity) :
    references_(1),
    initialCapacity_(initialCapacity),
    capacity_(initialCapacity),
    autoCapacity_(autoCapacity)
{
  size_ = strlen(str);
  if(size_ > capacity_)
  {
    // TODO what if autoCapacity_ is false and the initial string is > than capacity?
    capacity_ = size_;
  }

  // always make it 1 char larger for the end of line
  str_ = new char[capacity_+1];
  memcpy(str_, str, size_);
  str_[size_] = '\0';
}

CStringData::CStringData(const char *str,
                         CStringData::size_type length,
                         CStringData::size_type initialCapacity,
                         bool autoCapacity) :
    references_(1),
    initialCapacity_(initialCapacity),
    capacity_(initialCapacity),
    autoCapacity_(autoCapacity)
{
  size_ = length;
  if(size_ > capacity_)
  {
    // TODO what if autoCapacity_ is false and the initial string is > than capacity?
    capacity_ = size_;
  }

  // always make it 1 char larger for the end of line
  str_ = new char[capacity_+1];
  memcpy(str_, str, size_);
  str_[size_] = '\0';
}

CStringData::~CStringData()
{
  delete [] str_;
}

//----------------------------------------------------------------------
//
//    CString implementation
//      Never allow a CString to have a null CStringData member
//----------------------------------------------------------------------

CString::CString(size_type initialCapacity, bool autoCapacity) : // defaults to CString::INITIAL_CAPACITY, true
  padChar_(DEFAULT_PAD_CHAR)
{
  data_ = new CStringData(initialCapacity, autoCapacity);
}

CString::CString(const char *str, size_type initialCapacity, bool autoCapacity) : // defaults to CString::INITIAL_CAPACITY, true
  padChar_(DEFAULT_PAD_CHAR)
{
  data_ = new CStringData(str, initialCapacity, autoCapacity);
}

CString::CString(const char *str, size_type length, size_type initialCapacity, bool autoCapacity) : // defaults to true
  padChar_(DEFAULT_PAD_CHAR)
{
  data_ = new CStringData(str, length, initialCapacity, autoCapacity);
}

CString::CString(const CString &copY) :
  data_(NULL),
  padChar_(DEFAULT_PAD_CHAR)
{
  copy(copY);
}

#ifndef NO_OPERATORS
void CString::operator=(const CString &copY)
{
  copy(copY);
}
#endif

CString::~CString()
{
  decrementReference();
}

void CString::copy(const CString &copy)
{
  // Stop referring to the current data and refer to the data of another CString

  // update the existing CStringData
  if(data_ != NULL)
  {
    decrementReference();
  }

  // now setup a "new" CStringData
  data_ = copy.data_;
  data_->references_++;

  padChar_ = copy.padChar_;
}


// private
void CString::decrementReference()
{
  if(data_->references_ < 2)
  {
    delete data_;
    data_ = NULL;
  }
  else
  {
    data_->references_--;
  }
}

// private
void CString::incrementCapacity(size_type size)
{
  if(!data_->autoCapacity_)
  {
    throw CStringOutOfBoundsException(
        "Trying to increment capacity with autoCapacity set false");
  }

  size_type origCap = data_->capacity_;
  // Check that we increment enough to hold the new string
  data_->capacity_ += ((size > data_->initialCapacity_) ? size : data_->initialCapacity_);

  if(data_ != NULL)
  {
    // TODO whats better: this way or using realloc?

    // always make it 1 char larger for the end of line
    char *ptr = new char[data_->capacity_+1];
    memcpy(ptr, data_->str_, origCap);
    delete [] data_->str_;
    data_->str_ = ptr;
  }
}

void CString::toupper()
{
  for(int i = 0; i < size(); i++)
  {
    // ascii  hex   decimal
    //   'a'  0x61   97
    //   'z'  0x7a   122
    //   'A'  0x41   65
    //   'Z'  0x5a   90
    //
    if(data_->str_[i] >= 'a' && data_->str_[i] <= 'z')
    {
      data_->str_[i] -= 32;
    }
  }
}

void CString::tolower()
{
  for(int i = 0; i < size(); i++)
  {
    // ascii  hex   decimal
    //   'a'  0x61   97
    //   'z'  0x7a   122
    //   'A'  0x41   65
    //   'Z'  0x5a   90
    //
    if(data_->str_[i] >= 'A' && data_->str_[i] <= 'Z')
    {
      data_->str_[i] += 32;
    }
  }
}

bool CString::isNumber() const
{
  if(empty())
  {
    return false;
  }

  // Get past the leading spaces
  int i = 0;
  while(data_->str_[i++] == ' ' && i < size()) {}

  bool trailingSpaces = false;
  for(int j = i; j < size(); j++)
  {
    const char c = data_->str_[j];
    if(c == ' ')
    {
      trailingSpaces = true; // "1234 "
    }
    else if(c < '0' || c > '9')
    {
      if(trailingSpaces)
      {
        return false; // "123 4"
      }

      if(c == '-' && j == i) // leading minus sign
      {
        continue;
      }
      else if(c == ',' || c == '.')
      {
        // no checking as to number of ',' or '.' since in Europe they
        // use the '.' as a thousands marker, whereas in many other places
        // they use the ',' as a thousands marker.
        continue;
      }
      else
      {
        return false;
      }
    }
    else
    {
      if(trailingSpaces)
      {
        return false; // "123 4"
      }
    }
  }

  return true;
}

/** @brief substr
  *
  */
CString CString::substr(size_type index, size_type numChars) const
{
  // TODO should we do this so that the returned substr references the same
  //      data to do that, we'd need a way to store an index into the data

  if(index > size())
  {
    throw CStringOutOfBoundsException("CString::substr index > size");
  }

  if(numChars == CString::NPOS)
  {
    numChars = size() - index;
  }

  if(index+numChars > size())
  {
    throw CStringOutOfBoundsException("CString::substr index+numChars > size");
  }

  CString returnStr(str()+index, numChars, DEFAULT_CAPACITY);

  return returnStr;
}

/** @brief begin
  *
  */
CStringIterator CString::iterator() const
{
  return CStringIterator(*this);
}

/** @brief rbegin
  *
  */
CStringReverseIterator CString::riterator() const
{
  return CStringReverseIterator(*this);
}

const char CString::index(size_type indeX) const
{
  if(indeX > size())
  {
    throw CStringOutOfBoundsException("CString::index index > size");
  }

  return data_->str_[indeX];
}

/** @brief hash
  *
  */
unsigned int CString::hash() const
{
  const char *ptr = str();
  CString::size_type length = size();
  unsigned int hash = 0;

  while(length--)
  {
    hash = (hash << 5) - hash + *ptr;
    ptr++;
  }

  return hash;
}

/** @brief find_
  *
  */
// private
CString::size_type
CString::find_(const char *str, CString::size_type index, CString::size_type length) const
{
  if(index > size())
  {
    throw CStringOutOfBoundsException("CString::find index > size");
  }

  if(index+length > size())
  {
    throw CStringInvalidArgException("CString::find index+length > size");
  }

  bool foundit = false;
  int i;
  int endpoint = size() - length + 1;
  for(i = index; i < endpoint && foundit == false; i++)
  {
    if(data_->str_[i] == str[0])
    {
      foundit = true;
      for(int j = 1; j < length && foundit == true; j++)
      {
        if(data_->str_[i+j] != str[j])
        {
          foundit = false;
        }
      }
    }
  }

  return foundit ? i-1 : CString::NPOS;
}


/** @brief find_
  *
  */
// private
CString::size_type
CString::rfind_(const char *str, CString::size_type index, CString::size_type length) const
{
  if(index > size())
  {
    throw CStringOutOfBoundsException("CString::rfind index > size");
  }

  if(index+length > size())
  {
    throw CStringInvalidArgException("CString::rfind index+length > size");
  }

  bool foundit = false;
  int i;
  int startpoint = size() - index - length + 1;
  for(i = startpoint; i >= 0 && foundit == false; i--)
  {
    if(data_->str_[i] == str[0])
    {
      foundit = true;
      for(int j = 1; j < length && foundit == true; j++)
      {
        if(data_->str_[i+j] != str[j])
        {
          foundit = false;
        }
      }
    }
  }

  return foundit ? i+1 : CString::NPOS;
}

/** @brief append_
  *
  */
// private
CString::size_type CString::append_(const char *strData,
                                    CString::size_type length,
                                    CString::size_type minWidth,
                                    bool leftJustify)
{
  size_type totalLength = (length < minWidth) ? minWidth : length;

  if(checkCapacity(totalLength))
  {
    incrementCapacity(totalLength);
  }

  size_type appendIndex = size();

  // pad spaces
  if( length < minWidth)
  {
    size_type padStart;
    size_type padLength;
    if(leftJustify)
    {
      padStart = size();
      padLength = size() + (minWidth - length);
      appendIndex = padLength;
    }
    else
    {
      padStart = size() + length;
      padLength = size() + minWidth;
    }

    memset(data_->str_ + padStart, padChar_, padLength);
  }

  memcpy(data_->str_ + appendIndex, strData, length);
  data_->size_ += totalLength;
  data_->str_[size()] = '\0';

  return totalLength;
}

/** @brief insert
  *
  */
CString::size_type
CString::insert(const char ch,
                CString::size_type index,
                CString::size_type minWidth,
                bool leftJustify,
                CString::size_type count)
{
  char temp[count];
  memset(temp, ch, count);

  return insert_(temp, index, count, minWidth, leftJustify);
}

CString::size_type
CString::insert(int num,
                CString::size_type index,
                CString::size_type minWidth,
                bool leftJustify,
                CString::size_type count)
{
  char temp[10*count]; // a max int wont be more than 10 digits
  int numDigits = 0;

  for(int i = 0; i < count; i++)
  {
    numDigits += sprintf(temp + numDigits, "%d", num);
  }

  return insert_(temp, index, numDigits, minWidth, leftJustify);
}

CString::size_type
CString::insert(long num,
                CString::size_type index,
                CString::size_type minWidth,
                bool leftJustify,
                CString::size_type count)
{
  char temp[20*count]; // a max long wont be more than 20 digits
  int numDigits = 0;

  for(int i = 0; i < count; i++)
  {
    numDigits += sprintf(temp + numDigits, "%ld", num);
  }

  return insert_(temp, index, numDigits, minWidth, leftJustify);
}

CString::size_type
CString::insert(float num,
                CString::size_type index,
                CString::size_type numDecimals,
                CString::size_type minWidth,
                bool leftJustify,
                CString::size_type count)
{
  char temp[20*count]; // a max float wont be more than 30 digits
  int numDigits = 0;

  for(int i = 0; i < count; i++)
  {
    numDigits += sprintf(temp + numDigits, "%.*f", numDecimals, num);
  }

  return insert_(temp, index, numDigits, minWidth, leftJustify);
}

CString::size_type
CString::insert(bool b,
                size_type index,
                bool displayText,
                CString::size_type minWidth,
                bool leftJustify,
                size_type count)
{
  char temp[5*count]; // max str size will be "false"
  int numDigits = 0;

  for(int i = 0; i < count; i++)
  {
    if(displayText)
    {
      numDigits += sprintf(temp + numDigits, (b ? "true" : "false"));
    }
    else
    {
      temp[numDigits++] = (b ? '1' : '0');
    }
  }

  return insert_(temp, index, numDigits, minWidth, leftJustify);
}

// TODO allow to specify format string
CString::size_type
CString::insertHex(int num,
                   CString::size_type index,
                   CString::size_type minWidth,
                   bool leftJustify,
                   CString::size_type count)
{
  // a max int wont be more than 10 digits, and 2 hex digits per int digit plus 2 leading chars 0x
  char temp[22*count];
  int numDigits = 0;

  for(int i = 0; i < count; i++)
  {
    numDigits += sprintf(temp + numDigits, "%X", num);
  }

  return insert_(temp, index, numDigits, minWidth, leftJustify);
}

/** @brief insert_
  *
  */
// private
CString::size_type
CString::insert_(const char *strData,
                 CString::size_type index,
                 CString::size_type length,
                 CString::size_type minWidth,
                 bool leftJustify)
{
  if(index == CString::NPOS)
  {
    index = size();
  }

  if(index > size())
  {
    throw CStringOutOfBoundsException("CString::insert index > size");
  }

  size_type totalLength = (length < minWidth) ? minWidth : length;

  if(checkCapacity(totalLength))
  {
    incrementCapacity(totalLength);
  }

  //
  // If inserting in the middle of the string, copy from index to
  // end of str to a temp holding area, then insert the new string,
  // then copy from the temp str to the end of the string.
  //

  size_type substrLen = size()-index;
  char temp[substrLen];
  // If index = size(), then its an append and this temp copy isnt necessary
  if(substrLen > 0)
  {
    memcpy(temp, str()+index, substrLen);
  }

  size_type insertIndex = index;

  // pad spaces
  if( length < minWidth)
  {
    size_type padStart;
    size_type padLength;
    if(leftJustify)
    {
      padStart = index;
      padLength = index + (minWidth - length);
      insertIndex = padLength;
    }
    else
    {
      padStart = index + length;
      padLength = index + minWidth;
    }

    memset(data_->str_ + padStart, padChar_, padLength);
  }

  // Insert the string
  memcpy(data_->str_+insertIndex, strData, length);
  data_->size_ += totalLength;

  // Again, if index = size(), then its an append and this final copy isnt necessary
  if(substrLen > 0)
  {
    memcpy(data_->str_+index+totalLength, temp, substrLen);
  }

  // Always terminate the string
  data_->str_[size()] = '\0';

  return totalLength;
}

/** @brief remove
  *
  */
CString::size_type
CString::remove(CString::size_type index, CString::size_type numChars)
{
  if(index > size())
  {
    throw CStringOutOfBoundsException("CString::remove index > size");
  }

  // Remove to end of string
  if(numChars == CString::NPOS)
  {
    numChars = size() - index;
  }

  if(numChars+index > size())
  {
    throw CStringInvalidArgException("CString::remove numChars+index > size");
  }

  // If we're removing until end of string, we dont need the memcpy
  if(numChars+index != size())
  {
    memcpy(data_->str_+index, str()+index+numChars, size()-index-numChars);
  }

  data_->size_ -= numChars;
  data_->str_[size()] = '\0';

  return numChars;
}

/** @brief replace
  *
  */
CString::size_type
CString::replace(const char ch, CString::size_type index, CString::size_type count, CString::size_type length)
{
  char temp[count];
  for(int i = 0; i < count; i++)
  {
    temp[i++] = ch;
  }

  return replace_(temp, index, length);
}

/** @brief replace
  *
  */
// private
CString::size_type
CString::replace_(const char *strData, CString::size_type index, CString::size_type length)
{
  if(index > size())
  {
    throw CStringOutOfBoundsException("CString::replace index > size");
  }

  if(length == CString::NPOS)
  {
    length = size();
  }

  int strDataLength = strlen(strData);
  if(length == 0)
  {
    length = strDataLength;
  }

  // Replacing off end of string
  if((length+index) > size())
  {
    if(checkCapacity((length+index) - size()))
    {
      incrementCapacity((length+index) - size());
    }
  }

  // if(length > strDataLength)
  //     str="abcdefgh", replace("xyz", 2, 4) => str="abxyzgh", size reduced
  // if(length < strDataLength)
  //     str="abcdefgh", replace("xyz", 2, 2) => str="abxyzefgh", size increased

  int tempSize = size() - (length + index);
  if(tempSize > 0)
  {
    char temp[tempSize];
    memcpy(temp, data_->str_+index+length, tempSize);
    memcpy(data_->str_+index, strData, strDataLength);
    memcpy(data_->str_+index+strDataLength, temp, tempSize);
    data_->size_ += (strDataLength-length);
  }
  else
  {
    // This is either the case of replacing off the end of the string
    // or the str to replace is the same length as strData
    memcpy(data_->str_+index, strData, strDataLength);
    data_->size_ = index + strDataLength;
  }

  data_->str_[size()] = '\0';

  return length;
}
