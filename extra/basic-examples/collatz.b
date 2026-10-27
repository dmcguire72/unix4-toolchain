10 n = 27
20 c = 0
30 if n-1 goto 50
40 print c
45 done
50 if (n/2)-int(n/2) goto 80
60 n = n/2
70 goto 90
80 n = 3*n+1
90 c = c+1
95 goto 30
run
