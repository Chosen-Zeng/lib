#include <stdio.h>

int main(void)
{
    unsigned hex;
    while (1)
    {
        printf("Start with 0x: ");
        scanf("0x%X", &hex);
        printf("%f", *(float *)&hex);

        getchar();
    }
}