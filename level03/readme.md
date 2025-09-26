# Level03

## Analysis

First of all, we have once again some inputs inside the program, in `main()`, `scanf()` gets our input and passes it to `test()` such as : `test(input, 322424845);`

This function takes both arguments, substracts them and check for cases before passing it to another function called `decrypt()`.

What's important is that we can have some leverage upon how `decrypt()` is executed if the substraction results is between 1 and 21, else it would pass a random value to the function which would not help us at all.

Inside `decrypt()`, there is simply an iteration over the following string : "Q}|u`sfg~sf{}|a3" such as:

```c
strcpy((char *)v4, "Q}|u`sfg~sf{}|a3");
v3 = strlen((const char *)v4);
for ( i = 0; i < v3; ++i )
  *((_BYTE *)v4 + i) ^= a1;
```

After that, a check is made in order to evaluate if this string is equal to another, if that's the case, we pass, else rip.

```c
if ( !strcmp((const char *)v4, "Congratulations!") )
  return system("/bin/sh");
else
  return puts("\nInvalid Password");
```

## "Exploit"

This is a simple "hashing" process, we have the string and the for loop so we can apply the reverse principle to get the right input:

```
Q ^ ? = C

Q ^ C = ?
```

<details>
  <summary>Spoiler</summary>

  "Q" = 81
  "C" = 67
  81 ^ 67 = 18

 322424845 - 18 = 322424827

```bash
level03@OverRide:~$ ./level03 
***********************************
*		level03		**
***********************************
Password:322424827
$ whoami 
level04
$ cat /home/users/level04/.pass
kgv3tkEb9h2mLkRsPkXRfc2mHbjMxQzvb2FrgKkf
$
```

</details>
