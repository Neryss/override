int main (int argc, char **argv)
{
    char s3 = argv[1][0];
    int v4;

    v4 = (s3 ^ 0x1337) + 6221293;


    int i = 0;
    while (i < 6)
    {
        printf("%d\n", v4);
        v4 += (v4 ^ (unsigned int)s3) % 0x539;
        i++;
    }
    printf("\nresult = %d", v4);
}
