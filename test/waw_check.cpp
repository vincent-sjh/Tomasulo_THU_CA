int main() {
    int x, y, z;

    asm volatile(
        "li %[x], 1234\n"
        "li %[y], 5678\n"
        "mul %[z], %[x], %[y]\n"
        "div %[z], %[y], %[x]\n"
        "add %[z], %[y], %[x]\n"
        "add %[z], %[y], %[x]\n"
        "add %[z], %[y], %[x]\n"
        "add %[z], %[y], %[x]\n"
        "add %[z], %[y], %[x]\n"
        "add %[z], %[y], %[x]\n"
        "add %[z], %[y], %[x]\n"
        "add %[z], %[y], %[x]\n"
        "add %[z], %[y], %[x]\n"
        : [z] "=&r"(z)
        : [x] "r"(x), [y] "r"(y)
        :);
    
    asm volatile(".word 0x0000000b"  // exit mark
    );

    return z;
}
