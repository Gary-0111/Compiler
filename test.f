int sum1 = 0, sum2 = 0;
int i = 1;
while(i <= 100) {
    if(i%2 == 1) 
        sum1 = sum1 + i;
    else
        sum2 = sum2 + i;
    i = i + 1;
}
print(sum1);
print(sum2);