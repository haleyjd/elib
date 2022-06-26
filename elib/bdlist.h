/*
  ELib

  Generalized Bidirectional Double-linked List Routines

  The MIT License (MIT)

  Copyright (C) 2016 James Haley

  Permission is hereby granted, free of charge, to any person obtaining a copy
  of this software and associated documentation files (the "Software"), to deal
  in the Software without restriction, including without limitation the rights
  to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
  copies of the Software, and to permit persons to whom the Software is
  furnished to do so, subject to the following conditions:

  The above copyright notice and this permission notice shall be included in all
  copies or substantial portions of the Software.

  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
  OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
  SOFTWARE.
*/

#pragma once

//
// Bidirectional linked list.
//
template<typename T> 
class EBDListItem
{
public:
   EBDListItem<T> *bdNext;
   EBDListItem<T> *bdPrev;
   T             *bdObject = nullptr; // pointer back to object
   unsigned int   bdData   = 0;       // arbitrary data cached at node

   EBDListItem() { bdPrev = bdNext = this; }

   // non-copyable
   EBDListItem(const EBDListItem &) = delete;
   EBDListItem &operator = (const EBDListItem &) = delete;

   inline void insert(T *parentObject, EBDListItem<T> &listHead)
   {
      listHead.bdPrev->bdNext = this;
      this->bdNext = &listHead;
      this->bdPrev = listHead.bdPrev;
      listHead.bdPrev = this;

      this->bdObject = parentObject;
   }

   // WARNING: If you are iterating over the list, you MUST pass your
   // iteration pointer in *myIterator or you will have to restart the
   // iteration from the beginning of the list. Do *NOT* attempt to
   // cache next/prev values from the object around this call yourself.

   inline void remove(EBDListItem<T> **myIterator = nullptr)
   {
      EBDListItem<T> *lnext = this->bdNext;

      if(myIterator)
         *myIterator = this->bdPrev;

      (lnext->bdPrev = this->bdPrev)->bdNext = lnext;
   }
};

//
// EBDList
//
template<typename T, EBDListItem<T> T::* link> 
class EBDList
{
public:
   EBDListItem<T> head;
   inline void insert(T *object) { (object->*link).insert(object, head); }
   inline void insert(T &object) { insert(&object);                      }

   inline void remove(T *object, EBDListItem<T> **myIterator = nullptr) 
   {
      (object->*link).remove(myIterator);
   }

   inline void remove(T &object, EBDListItem<T> **myIterator = nullptr)
   {
      remove(&object, myIterator);
   }

   inline bool empty() const { return (head.bdNext == &head); }
   
   inline EBDListItem<T> *first() const { return head.bdNext;  }
   inline EBDListItem<T> *last()  const { return head.bdPrev;  }
};

// EOF
