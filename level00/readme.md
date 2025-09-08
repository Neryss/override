# Level00

## Analysis

```bash
   0x080484de <+74>:	call   0x80483d0 <__isoc99_scanf@plt>
   0x080484e3 <+79>:	mov    0x1c(%esp),%eax
   0x080484e7 <+83>:	cmp    $0x149c,%eax
```

The return of `scanf` is moved into `eax` which is then compared to `0x149c` which corresponds to `5276`.

## Solution

<details>
  <summary>Spoiler</summary>

  We can simply answer with the right password:

  ```bash
  level00@OverRide:~$ ./level00 
  ***********************************
  * 	    -Level00 -		 *
  ***********************************
  Password:5276

  Authenticated!
  $ cat /home/users/level01/.pass
  uSq2ehEGT6c9S24zbshexZQBXUGrncxn5sD5QfGL
  $ 
  ```
</details>
