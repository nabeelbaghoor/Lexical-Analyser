int: num;
char: my_char;
\\ lets assign variable my_char a value
my_char = 'd';
print("my char contains: ");
println(my_char);
/*
The program here onwards is an iterative algorithm
for fibonacci numbers
*/
println("enter a number");
input -> num;
int: a=0, b=1, c=0;
println("The fibonacci seq is: ");
println(a);
println(b);
while c < num:
{
int: temp = a + b;
a = b;
b = temp;
println(temp);
c++;
}