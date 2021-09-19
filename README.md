# Truthtable
Create a program that creates a truthtable based on given logic gates.

The program takes a txt file input. The text file should first include the input and output variable names. Then it should put a list of logic gates with one in each line. Each line should start with the type of logic gate it is (NOT, AND, NAND, OR, XOR) and then list the inputs followed by the outputs. The program will figure out which ones are inputs and which ones are outputs based on the amount of total inputs and outputs given and the type of logic gate.

Example txt file:

INPUT 5   IN1 IN2 IN3 IN4 IN5
OUTPUT 1  OUT1

DECODER 4      IN1 IN2 IN3 IN4   temp1 temp2 temp3 temp4 temp5 temp6 temp7 temp8 temp9 temp10 temp11 temp12 temp13 temp14 temp15 temp16
OR             temp6 temp10      temp17
OR             temp17 temp11     temp18
OR             temp18 temp12     temp19
OR             temp19 temp14     temp20
OR             temp1 temp2       temp21
OR             temp21 temp4      temp22
OR             temp22 temp8      temp23
OR             temp16 temp23     temp24
MULTIPLEXER 3  0 1 0 0 1 1 1 1   temp20 temp24   IN5 OUT1

The program first checks to make sure there is only one input. Then it begins to malloc all the necessary memory that will be used in the program. The program loads all inputs "known inputs" list and each logic gate into a gate struct which saves its inputs, outputs, type, and size. Afterwards, it will load the gates into one of two lists. The "current" list will be loaded with logic gates that already have all inputs "known". This means that at the start of the program, these logic gates can be immediately solved. These logic gates are identified at the start since their inputs will be a subset of the total inputs given at the start of the file. The rest of the logic gates will go into a "waiting" list where they will wait until they can be solved. As the program executes, it will solve all the "current" logic gates and then check to see which logic gates in the "waiting" list have all their inputs known and can be solved. It will repeat this until all the outputs are known and then do it again for each combination of inputs. At the end, it will print out a truth table based on the given logic gates.
