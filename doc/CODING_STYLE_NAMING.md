# Source Code Formatting

## Comments

### Code Documentation

The code should be documented using Doxygen formatted comments according to their [manual](http://www.stack.nl/~dimitri/doxygen/manual/docblocks.html)

### Comments style

Unless you're working on C code or maintaining a part of a project written in C, always prefer the C++ style comments (//) over the C style comments (/* */) as the former are easier to insert and to remove and don't have nesting issues.
For doxygen comments, use three slashes (///).

### Source files headers

Every source file should begin with a header comment as follow:
```cpp
/// This file is part of Wakanda software, licensed by 4D under
///  ( i ) the GNU General Public License version 3 ( GNU GPL v3 ), or
///  ( ii ) the Affero General Public License version 3 ( AGPL v3 ) or
///  ( iii ) a commercial license.
/// This file remains the exclusive property of 4D and/or its licensors
/// and is protected by national and international legislations.
/// In any event, Licensee's compliance with the terms and conditions
/// of the applicable license constitutes a prerequisite to any use of this file.
/// Except as otherwise expressly stated in the applicable license,
/// such license does not include any other license or rights on this file,
/// 4D's and/or its licensors' trademarks and/or other proprietary rights.
/// Consequently, no title, copyright or other proprietary rights
/// other than those specified in the applicable license is granted.

/// \file      file_name.ext
///
/// \brief     Description of the code inside of this file.
///
/// \author    Author name - <email_address>
/// \date      Creation date (DD-MM-YYYY)
```
You should note that the doxygen \brief directive should only appear in C++ header files.

### Structures and classes

Structures and classes's definitions should have a doxygen comment describing how they work and how they're used.
```cpp
/// \brief Information about persons.
struct Person
{
    int m_age;
    std::string m_name;
};
```
### Functions and methods

Functions and methods's declarations should have a doxygen comment describing what they do, their parameters and their return value.
```cpp
class Product
{
public:
    /// \brief Return information about a specific product.
    /// \param productReference reference of the product.
    ///
    /// \return product information structure.
    ProductInfo getProductInfo(const std::string& productReference);
};
```

```cpp
/// \brief Divide two integers and returns the result.
///
/// \param a dividend.
/// \param b divisor.
/// \param[out] c quotient.
///
/// \return true if division is possible (divisor > 0), false otherwise.
bool divFunction(const int a, const int b, int& c);
```
### class and structs data members

Data members of classes and structs should be followed by a doxygen comment describing what they're used for. Aligning the comments's start at the same column is preferred whenever possible.
```cpp
struct Person
{
    int m_age;          ///< Person's age.
    std::string m_name; ///< Person's name.
};
```
## Source code indentation

### Source code line length

The maximum length of each line of code should be 100 characters.
This limitation makes it easier to read the code on screens with small resolutions, vertical monitors, when doing code diffs and most importantly to read the code without having to scroll horizontally or continue the reading on the next line.

### Do not use tabs

Tabs should be strictly avoided, use 4 spaces instead.
Configure your editor to emit spaces when the tab key is pressed.

### Avoid consecutive empty lines

At most, you may have only one empty line between groups of code lines.

### Allman style for braces

The Allman style AKA BSD style is used. Braces should be put on their own line and be indented to the same level as the control statement.

**Good**
```cpp
ProductInfo getProductInfo(const std::string& productReference)
{
    std::string productName;
    if (productReference.empty())
    {
        productName.assign("unknown product");
    }
    else
    {
        productName = productReference;
    }
    ...
}
```

**Bad**
```cpp
ProductInfo getProductInfo(const std::string& productReference) {
    std::string productName;
    if (productReference.empty()) {
        productName.assign("unknown product");
    }
    else
        {
            productName = productReference;
        }
    ...
}
```

### Braces for single line control statements

Always use braces with control statements even if they only contain a single line.

**Good**
```cpp
if (productReference.empty())
{
    productName.assign("unknown product");
}
else
{
    productName = productReference;
}
```

**Bad**
```cpp
if (productReference.empty())
    productName.assign("unknown product");
else
    productName = productReference;
```

# Naming conventions

In general, one must choose the most descriptive name for a given symbol. The name should give anyone reading the code a clear idea about the usefulness and the role for which the entity was introduced. As such, abbreviations are strongly discouraged especially when they are ambiguous and have no related meaning to the project's nature.

The camel case should be used for naming. Alternation between starting with a lower or an upper case letter is dependent on the symbol's type.

## Type names

Use nouns in upper camel case for all the type names — classes, structs, enums, typedefs, etc...
```cpp
struct Person
{
    int m_age;
    std::string m_name;
};
```

## Variable names

### Non-member variables

Use nouns in lower camel case for all non-member variables — Local variables, function arguments, global and static variables, etc...
```cpp
ProductInfo getProductInfo(const std::string& productReference)
{
    std::string productName;
    const int productNumber = 1324;
    ...
}
```

### Constant variables

Constant variables should be named with a leading **k** followed by an upper camel case noun.
```cpp
const std::string kProductName = "Cool Car";
constexpr float kAngle = degreesToRadians(180.0f);
```

### class and structs data members

The same rules as non-member variables apply here with the addition of **m_** as the members's prefix
```cpp
class ProductInfo
{
public:
    ProductInfo();
    virtual ~ProductInfo();
private:
    int m_number;
    std::string m_name;
};
```

### Function names

Use verbs in lower camel case to represent the action that the function's doing.
```cpp
int getLastAction(const std::string& nameFilter);
bool ProductManager::processProduct(const ProductInfo& prodInfo);
```
### Enumerator Names

Given that enumerators are constant values, following the **Constant variables** naming rules is preferred. However, the following styles are also valid:
```cpp
// Prefix with **e** plus the value's name in upper camel case
enum ValueKind
{
    eString = 1,
    eNumber = 2,
    eBlob = 3
};

// Prefix with the first letters of the enum's name combined with an underscore plus the value's
// name in lower camel case
enum ValueKind
{
    VK_string = 1,
    VK_number = 2,
    VK_blob = 3
};
```

### Macro names

In the rare cases where you need to define a macro, the name should be all capitals and underscores.
```cpp
#define WITH_EXPERIMENTAL_OPTIMIZATION 1
#define SWAP_TWO_INTS(a,b) ...
```

### Namespace names

Namespace names should be all lowercase and _may_ include underscores (_).

# Source files

## Source file names

Source code files should be named in lowercase and _may_ include underscores (_), followed with a **.h** extension for C/C++ header files and a **.cpp** for C++ source files.
The files should be named with the same class's name that they define, whenever possible.

```
shared_storage.h
shared_storage.cpp
```

## Source file templates

### Header file (shared_storage.h)
```cpp
/// This file is part of Wakanda software, licensed by 4D under
///  ( i ) the GNU General Public License version 3 ( GNU GPL v3 ), or
///  ( ii ) the Affero General Public License version 3 ( AGPL v3 ) or
///  ( iii ) a commercial license.
/// This file remains the exclusive property of 4D and/or its licensors
/// and is protected by national and international legislations.
/// In any event, Licensee's compliance with the terms and conditions
/// of the applicable license constitutes a prerequisite to any use of this file.
/// Except as otherwise expressly stated in the applicable license,
/// such license does not include any other license or rights on this file,
/// 4D's and/or its licensors' trademarks and/or other proprietary rights.
/// Consequently, no title, copyright or other proprietary rights
/// other than those specified in the applicable license is granted.

/// \file      shared_storage.h
///
/// \brief     Modern implementation of Wakanda's shared storage Node.js.
///
/// \author    Author - <author@wakanda.io>
/// \date      08-03-2018

#ifndef SHARED_STORAGE_H_
#define SHARED_STORAGE_H_

// Includes.
#include <string>

// Forward declarations.
struct MyStruct;

// Type defs.
using MyPair = std::pair<std::string, MyStruct>;

/// \brief Very sensitive information about a person.
struct Person
{
    int m_age;          ///< Person's age.
    std::string m_name; ///< Person's name.
};

/// \brief Keeps record and manages the persons.
class PersonManager
{
public:
    ...

    /// \brief  Initialize the Person's DB.
    ///
    /// \return true if initialization succeeded.
    bool initDB();

    ...
};

#endif /* SHARED_STORAGE_H_ */
```

### Source file (shared_storage.cpp)

```cpp
/// This file is part of Wakanda software, licensed by 4D under
///  ( i ) the GNU General Public License version 3 ( GNU GPL v3 ), or
///  ( ii ) the Affero General Public License version 3 ( AGPL v3 ) or
///  ( iii ) a commercial license.
/// This file remains the exclusive property of 4D and/or its licensors
/// and is protected by national and international legislations.
/// In any event, Licensee's compliance with the terms and conditions
/// of the applicable license constitutes a prerequisite to any use of this file.
/// Except as otherwise expressly stated in the applicable license,
/// such license does not include any other license or rights on this file,
/// 4D's and/or its licensors' trademarks and/or other proprietary rights.
/// Consequently, no title, copyright or other proprietary rights
/// other than those specified in the applicable license is granted.

/// \file      shared_storage.cpp
///
/// \author    Author - <author@wakanda.io>
/// \date      08-03-2018

// Local includes.
#include "shared_storage.h"
#include "helpers.h"

// Other includes.
#include <vector>

bool PersonManager::initDB()
{
    ...
}

```
