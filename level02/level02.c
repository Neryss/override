int32_t main(int32_t argc, char** argv, char** envp)
{
    int32_t argc_1 = argc;
    char** argv_1 = argv;
    char var_78[0x64];
    *__builtin_memset(&var_78, 0, 0x60) = 0;
    char buf_1[0x30];
    *__builtin_memset(&buf_1, 0, 0x28) = 0;
    char buf[0x40];
    *__builtin_memset(&buf, 0, 0x60) = 0;
    int64_t var_10 = 0;
    int32_t var_14 = 0;
    FILE* fp = fopen("/home/users/level03/.pass", "r");
    
    if (!fp)
    {
        fwrite("ERROR: failed to open password file\n", 1, 0x24, stderr);
        exit(1);
        /* no return */
    }
    
    int32_t rax = fread(&buf_1, 1, 0x29, fp);
    buf_1[strcspn(&buf_1, "\n")] = 0;
    
    if (rax != 0x29)
    {
        fwrite("ERROR: failed to read password file\n", 1, 0x24, stderr);
        fwrite("ERROR: failed to read password file\n", 1, 0x24, stderr);
        exit(1);
        /* no return */
    }
    
    fclose(fp);
    puts("===== [ Secure Access System v1.0 ] =====");
    puts("/***************************************\\");
    puts("| You must login to access this system. |");
    puts("\**************************************/");
    printf("--[ Username: ");
    fgets(&var_78, 0x64, __bss_start);
    var_78[strcspn(&var_78, "\n")] = 0;
    printf("--[ Password: ");
    fgets(&buf, 0x64, __bss_start);
    buf[strcspn(&buf, "\n")] = 0;
    puts("*****************************************");
    
    if (!strncmp(&buf_1, &buf, 0x29))
    {
        printf("Greetings, %s!\n", &var_78, &var_78);
        system("/bin/sh");
        return 0;
    }
    
    printf(&var_78);
    puts(" does not have access!");
    exit(1);
    /* no return */
}
