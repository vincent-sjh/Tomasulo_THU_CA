int main() {
    int x, y, z;

    volatile unsigned *p = (unsigned *) 0x80400000;

    *p = 0x12345678u;

    *p += 10;

    asm volatile(
        "li %[x], 32832\n"
        "li %[y], 64\n"
        "mul %[z], %[x], %[y]\n"
        "li %[y], 32\n"
        "mul %[z], %[z], %[y]\n"
        "mul %[z], %[z], %[y]\n"
        "sw %[x], 0(%[z])\n"
        "lw %[y], 0(%[p])\n"
        "addi %[p], %[p], -16\n"
        "lw %[y], 16(%[p])\n"
        "addi %[p], %[p], -16\n"
        "lw %[y], 32(%[p])\n"
        : [z] "=&r"(z), [p] "=&r"(p)
        : [x] "r"(x), [y] "r"(y) 
        :);

    asm volatile(".word 0x0000000b"  // exit mark
    );

    return z;
}
