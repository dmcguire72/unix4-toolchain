
void (*bad_func)();

crash_here()
{
    return crash_here();
}

level_two() {
    return crash_here();
}

level_one() {
    return level_two();
}

main() {
    level_one();
}