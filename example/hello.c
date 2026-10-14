main(argc, argv)
int argv[]; {
     int i;
     i = 1;
     while(i<argc) {
          printf("%s ", argv[i]);
          i++;
     }
}