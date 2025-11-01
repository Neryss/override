int secret_backdoor()
{
  char s[128]; // [rsp+0h] [rbp-80h] BYREF

  fgets(s, 128, stdin);
  return system(s);
}

int handle_msg()
{
  _BYTE v1[140]; // [rsp+0h] [rbp-C0h] BYREF
  __int64 v2; // [rsp+8Ch] [rbp-34h]
  __int64 v3; // [rsp+94h] [rbp-2Ch]
  __int64 v4; // [rsp+9Ch] [rbp-24h]
  __int64 v5; // [rsp+A4h] [rbp-1Ch]
  __int64 v6; // [rsp+ACh] [rbp-14h]
  int v7; // [rsp+B4h] [rbp-Ch]

  v2 = 0;
  v3 = 0;
  v4 = 0;
  v5 = 0;
  v6 = 0;
  v7 = 140;
  set_username((__int64)v1);
  set_msg((__int64)v1);
  return puts(">: Msg sent!");
}

char *__fastcall set_msg(__int64 a1)
{
  char s[1024]; // [rsp+10h] [rbp-400h] BYREF

  memset(s, 0, sizeof(s));
  puts(">: Msg @Unix-Dude");
  printf(">>: ");
  fgets(s, 1024, stdin);
  return strncpy((char *)a1, s, *(int *)(a1 + 180));
}

//----- (00000000000009CD) ----------------------------------------------------
int __fastcall set_username(__int64 a1)
{
  char s[140]; // [rsp+10h] [rbp-90h] BYREF
  int i; // [rsp+9Ch] [rbp-4h]

  memset(s, 0, 0x80u);
  puts(">: Enter your username");
  printf(">>: ");
  fgets(s, 128, stdin);
  for ( i = 0; i <= 40 && s[i]; ++i )
    *(_BYTE *)(a1 + i + 140) = s[i];
  return printf(">: Welcome, %s", (const char *)(a1 + 140));
}

int __fastcall main(int argc, const char **argv, const char **envp)
{
  puts(
    "--------------------------------------------\n"
    "|   ~Welcome to l33t-m$n ~    v1337        |\n"
    "--------------------------------------------");
  handle_msg();
  return 0;
}

