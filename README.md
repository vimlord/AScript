#AScript

AScript (ArduinoScript, or An AScript) is my attempt to create a custom language that can be converted into assembly for use by the Arduino UNO's ATmega328P processor. I decided to do this while in CS383 (Computer Organization and Programming), in which we had to learn assembly language for use on an Arduino UNO with an ATmega328P processor.

##Syntax

####Variables

Currently, the only datatype supported is byte. They can be instantiated like so:

```
byte foo = 12;
```

If a variable is not given an initial value, then it will automatically default to 0 in the compiler. Every program should only initialize variables at the start of the program, or outside of any loops. It is important to note that one should not name variables after tokens or numbers, since the compiler will treat those cases incorrectly. For example,

```
byte 123;
byte byte;
byte if;
```

are all extremely terrible things that you should not do in any program. One can also create arrays of values. These can be doing the following:

```
byte[6] byteArray;
byte i = 4;
byte j = byteArray[i];
```

In the segment above, the first line creates an array of length 6. Arrays can only be instantiated with a constant length, meaning that an array cannot be created with a length equal to the result of a computation. However, an array can be accessed based on the result of a computation. In the example, one can access the ith element of an array in order to assign it to a variable. This allows for a user to iterate over a list, which enables actions such as sorting.

####Arithmetic

The compiler is written so that it will have the program run through the correct order of operations. The program will use the same order of operations used by the C family of operations. That is, ```() -> * -> + or - -> < or > -> == or != -> & -> |```. Currently, division is not supported, but that and other operations will eventually be supported.

#
####Conditionals

AScript allows for if-else statements and while loops. Both must be terminated with a semicolon when present.

```
byte factorial = 1;
byte n = 2;
while(n <= 6) {
    factorial = factorial * n;
    n = n + 1;
};
```

This code snippet will iterate and compute n!. In the function, n will continuously decrement until it equals zero, in which case the loop will terminate.

```
byte input = 6;
if(input) {
    input = 0;
} else {
    input = 1;
};
```

This segment of code takes the value of input and sets it to zero if it is not already zero. Otherwise, it will set the value equal to 1. In a sense, this statement is equivalent to the ! operation, which negates the truth of a statement.

#Functions
AScript also has the ability to allow users to write functions. Like with conditional statements, functions should be terminated with a semicolon. A function can be written like so:

```
function <TYPE> <NAME>(<TYPE> x, <TYPE> y) {
    ...
};
```

Here, <TYPE> represents any variable type that can be instantiated, and <NAME> is the name of the function. A function can also be called doing something like 

```
byte x = byteFunc(2);
```

provided that there exists a function called byteFunc that takes a number. One can also call functions inside of other functions. So, one function that could be written is

```
function byte factorial(byte n) {
    if(n > 1) {
        return n * factorial(n-1);
    } else {
        return 1;
    };
};
```

Note that factorial takes a byte as a parameter, and returns a byte. In its code, it also calls a function that takes a byte as a parameter and returns a byte.

#Usage

Currently, there is no available builds of the compiler. However, the compiler source can be downloaded from here and compiled. To build the compiler, run the included makefile with 

```
make all
```

This will compile the source into an executable. Then, run the executable with your sourcefile, which must be a .scr file, as a parameter:

```
./ascript srcfile.scr
```

Note that the source file must be a .scr file. The program will output the result into a .asm file in the current directory as long as it compiled correctly. The asm file can then be used in your IDE of choice. Compilation can go wrong in several instances. For example, if one does not provide a valid input file, the program will indicate this and immediately terminate.

The executable also provides several flags that can be enabled to customize compilation. They are as follows:

| Flag | Name | Description |
| ---- | ---- | ----------- |
| o | Output Selector |Sets the output file to compile to (must be an asm file). |
| O | Optimization Flag | Enables optimization. The compiler will give extra output to indicate the number of changes. |
| w | Warning Error Flag | Interprets all warnings as errors. |

For example, if I wanted to compile the sorting demo (examples/sorting.scr) to test.asm, using the O and w flags, I would run:

```
./ascript -o test.asm examples/sorting.asm -Ow
```

Note that the -o flag is separate from the other flags, and that test.asm immediately succeeds it. This rule must be followed when using the output flag, although -o does not have to be immediately after the program name.

##Future

In the future, I hope to add more functionality to the language. At this point, the language still needs to undergo a lot of testing and improvement. Future changes will hopefully include optimization and I/O through the Arduino's I/O ports.
