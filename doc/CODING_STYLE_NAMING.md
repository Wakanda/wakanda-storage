## Coding Style Naming

### Conventions
- upper camel case: SharedStorage
- lower camel case: sharedStorage

### File Names
File names should be all lowercase and can include underscores (_).
```
shared_storage.h
main.cpp
```

### Indentation
Use only spaces, and indent 4 spaces at a time. We always use spaces for indentation. **Do not use tabs** in your code. You should set your editor to emit spaces when you hit the tab key.

### Code Documentation
Code should be documented using Doxygen or Javadoc formatted comments.
```cpp
/**
* @brief  product information getter.
*
* @param productReference reference of the product.
*
* @return the product information structure
*/
ProductInfo getProductInfo(const std::string& productReference);
```

### Braces
Opening and closing braces should be the first characters of the line which is not space. Closing brace should be on the same column  that its opening brace.
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

### Type Names
Type names (including classes, structs, enums, typedefs, etc) should be nouns in upper camel case.
```cpp
struct MyStruct
{
    int m_age;
    std::string m_name;
};
```

### Variable Names

#### Common Variable names
Common variable names (local, arguments) should be nouns in lower camel case.
```cpp
ProductInfo getProductInfo(const std::string& productReference)
{
    std::string productName;
    int productNumber = 1324;
    ...
}
```

#### Class and Structure Data Members
Data members of classes and structures, both static and non-static, should be nouns in lower camel case and prefixed with `m_`
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

### Function Names
Fonction names should be verb phrases (as they represent actions). The name should be lower camel case.
```cpp
int getLastAction(const std::string& nameFilter);
```

### Namespace Names
Namespace names should be all lowercase and can include underscores (_).

### Constant Names
Variables declared constexpr or const, and whose value is fixed for the duration of the program, should be named with a leading `k` followed by upper camel case.
```cpp
const int kDaysInAWeek = 7;
```

### Enumerator Names
Enumeration names should be nouns in upper camel case. Enumerators should be named either with a leading `e` followed by upper camel case, or like a constant, or with a prefix corresponding to the enumeration name followed by an underscore and lower camel case.
```cpp
enum ValueKind
{
    eString = 1,
    eNumber = 2,
    eBlob = 3
};

enum ValueKind
{
    kString,
    kNumber,
    kBlob
};

enum ValueKind
{
    VK_string,
    VK_number,
    VK_blob
};
```

### Macro Names
Macro names should be named with all capitals and underscores.
```cpp
#define WITH_EXPERIMENTAL_OPTIMIZATION 1
```

### Header File Template

```cpp
/*
 * This file is part of Wakanda software, licensed by 4D under
 *  ( i ) the GNU General Public License version 3 ( GNU GPL v3 ), or
 *  ( ii ) the Affero General Public License version 3 ( AGPL v3 ) or
 *  ( iii ) a commercial license.
 * This file remains the exclusive property of 4D and/or its licensors
 * and is protected by national and international legislations.
 * In any event, Licensee's compliance with the terms and conditions
 * of the applicable license constitutes a prerequisite to any use of this file.
 * Except as otherwise expressly stated in the applicable license,
 * such license does not include any other license or rights on this file,
 * 4D's and/or its licensors' trademarks and/or other proprietary rights.
 * Consequently, no title, copyright or other proprietary rights
 * other than those specified in the applicable license is granted.
 */

/**
 * \file      shared_storage.h
 *
 * \brief     Modern implementation of Wakanda's shared storage Node.js.
 *
 * \author    Author - <author@wakanda.io>
 * \date      08-03-2018
 */

#ifndef SHARED_STORAGE_H_
#define SHARED_STORAGE_H_

// Includes.
#include <string>

// Forward declarations.
struct MyStruct;

// Type defs.
using MyPair = std::pair<std::string, MyStruct>;

/**
 *  @brief Information about persons.
 */
struct MyStruct
{
    int m_age;
    std::string m_name;
};

#endif /* SHARED_STORAGE_H_ */
```

### Source File Template
```cpp
/*
 * This file is part of Wakanda software, licensed by 4D under
 *  ( i ) the GNU General Public License version 3 ( GNU GPL v3 ), or
 *  ( ii ) the Affero General Public License version 3 ( AGPL v3 ) or
 *  ( iii ) a commercial license.
 * This file remains the exclusive property of 4D and/or its licensors
 * and is protected by national and international legislations.
 * In any event, Licensee's compliance with the terms and conditions
 * of the applicable license constitutes a prerequisite to any use of this file.
 * Except as otherwise expressly stated in the applicable license,
 * such license does not include any other license or rights on this file,
 * 4D's and/or its licensors' trademarks and/or other proprietary rights.
 * Consequently, no title, copyright or other proprietary rights
 * other than those specified in the applicable license is granted.
 */

/**
 * \file      shared_storage.cpp
 *
 * \author    Author - <author@wakanda.io>
 * \date      08-03-2018
 */

// Local includes.
#include "shared_storage.h"
#include "helpers.h"

// Other includes.
#include <vector>
```
