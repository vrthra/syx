"
Copyright (c) 2007-2008 Luca Bruno

This file is part of Smalltalk YX.

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the 'Software'), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED 'AS IS', WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
DEALINGS IN THE SOFTWARE.
"

Object subclass: #CStructFieldSpec
       instanceVariableNames: 'name type offset'
       classVariableNames: ''!

!CStructFieldSpec methodsFor: 'accessing'!

name
     ^name
!

type
     ^type
!

offset
     ^offset
!

name: aName
     name := aName
!

type: aCType
     type := aCType
!

offset: anOffset
     offset := anOffset
! !

!CStructFieldSpec methodsFor: 'alignment'!

nextAlignedOffset
     | bytes |
     bytes := type bytes.
     offset to: offset + bytes do: [ :i |
         (i \\ bytes) = 0
              ifTrue: [ ^i ] ]
!

alignAccordingToPreviousField: aFieldSpec
     | diff myBytes prevBytes |
     aFieldSpec
         ifNil: [ offset := 0. ^self ].

     myBytes := type bytes.
     prevBytes := aFieldSpec type bytes.
     offset := aFieldSpec offset + prevBytes.
     myBytes > prevBytes
         ifTrue: [ offset := self nextAlignedOffset ]
! !

!CStructFieldSpec methodsFor: 'generator'!

compileAccessorsOn: aClass
     type compileAccessorsOn: aClass named: name withOffset: offset
! !
