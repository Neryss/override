# Level06

## Analysis

This program looks like another one we had to deal with earlier in this project. It asks for a login then for a serial number.
If we pass the check done in `auth()`, we gain access to the password, simple as that.

So what are the checks we need to deal with:

- First of all, our login needs to be longer that 5 characters, else the function returns
- There is also a `ptrace()` call that we will need to dodge as we will explain further down
- After that, during the hashing loop, each character ASCII value needs to be higher than 31

Our input serial is then compared to the desired hash result and returns if they are equal or not, if they are, then we get access to the password.

As usual you can find the decompilation [here](./level06.c)

## How to find the desired output

IMPORTANT NOTE: since the hashing method uses the login value during its loop, keep the same login name during both execution!

Since out input is directly compared to the desired result, we can use GDB to get it, circumventing the call to `ptrace` is quite easy, we can either break before it and jump after it, or just modify its return value which is compared in the same line. Here I used the later approach:

```bash
(gdb) b *auth+114 
Breakpoint 1 at 0x80487ba
(gdb) b *auth+286
Breakpoint 2 at 0x8048866
(gdb) r
Starting program: /home/users/level06/level06 
***********************************
*		level06		 *
***********************************
-> Enter Login: ZZZZZZZZ
***********************************
***** NEW ACCOUNT DETECTED ********
***********************************
-> Enter Serial: 444

Breakpoint 1, 0x080487ba in auth ()
(gdb)
```

I also put a second breakpoint on the comparison instruction between our input and the desired value.

Actually, we're located on the `ptrace()` line, if we take a look at the registers, we can check its return value:

```bash
(gdb) i r
eax            0xffffffff	-1
```

We can then set its value to 1 for example, skip the return case and land on the second bp:

```bash
(gdb) set $eax=1
(gdb) c
Continuing.

Breakpoint 2, 0x08048866 in auth ()
```

We're now located on this instruction: `=> 0x08048866 <+286>:	cmp    -0x10(%ebp),%eax`

We can then simply print the value at `$ebp-0x10`:

<details>
  <summary>Spoiler</summary>

```bash
(gdb) p *(int *)($ebp-0x10)
$1 = 6234974
```

You can then use the program normally:

```bash
level06@OverRide:~$ ./level06 
***********************************
*		level06		 *
***********************************
-> Enter Login: ZZZZZZZZ
***********************************
***** NEW ACCOUNT DETECTED ********
***********************************
-> Enter Serial: 6234974
Authenticated!
$ cat /home/users/level07/.pass
GbcPDRgsFK77LNnnuh7QyFYA2942Gp8yKj9KrWD8
```

</details>

