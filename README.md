Romeo is going to be a programming language inspired by C, Fortran and Julia, with an intermediate control over memory.

Usage example:
```
user@pcname:~$ Romeo
Welcome to Romeo Programming Language
> x=3
> y=3*x
> print(2+y)
11
> exit  
```

It is also possible to execute scripts
```
user@pcname:~/Romeo/$ Romeo Examples/variable_initialization.rm
```

New features are getting implemented day by day. Single Dispatch has been implemented to optimize function search. Operations are now call to functions, that will allow foroverloading of operators for user defined types. A reference operator has been implemented for passing arguments by ref to functions.
