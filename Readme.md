All of these programs can be compiled by using:
	~$g++ --std=c++11 *.cpp 
within the respective folder of the project. 
Now if you choose to run programs 3&4 with the test files provided there will be descrepencies on line numbers in errors. This is more/less okay and not really much of an error due to how I measuered line numbers in project 2 (which wasn't checked at that time) to how Prof. Ryan did in his code for project 2. And these "errors" carried over to 3&4. 

In Project 1 its a complete mess and doesn't look very good at all but it was soupposed to be a learning expereince and something to not make the same mistake again. 

The syntax for the language is as follows.
set x 21;			//will set the value of x to 21
set y "hello";		//will set the value of y to "hello"
print y+"world";	//will concat y and "world" then print
print y*x;			//will reurn y repeated x times, and print in this case
set z y*x;			//will do the same as above but set the return value to z instead of printing it. 
print x+5;			//prints the sum of x and 5 
x*6;				//computes the product of x and 6
y[1;2];				//substring of y from 1 to 2
y[2];				//substring from 2 till end of string
10*(5+2);			//parens may be used for order of operations
				
Error examples:

set y 51			//missing semicolon;

print "hello
	world";			//This is an error since strings may not have a line break in them

set xx 5*"string"; 	//an error since there is no definition of a number *(STAR) string		
set yy 7+"String"; 	//or for adding a string and a number

print xx;			//xx not set yet
set xx 5;			//its set down here

