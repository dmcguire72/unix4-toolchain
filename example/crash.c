level_two() {
    return level_one();
}

level_one() {
    return level_two();
}

main() {
    level_one();
}