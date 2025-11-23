# Subject
```txt
Display all subsets of a set whose sum of elements is equal to the first argument.
The numbers of the set are given from the second argument to the last.
A subset must not be displayed more than once.

Example 1:
./a.out 5 2 3 | cat -e
2 3$

Example 2:
./a.out 12 5 7 4 3 2 | cat -e
5 7$
5 4 3$
7 3 2$

They will **not** test invalid sequences, such as:

./a.out 5 12 18 | cat -e

The order of the output lines is not important, but the order of the input **must be preserved**.

This is valid:

./a.out 5 3 2 1 4 | cat -e 
3 2$
1 4$

This is also valid:

./a.out 5 3 2 1 4 | cat -e 
1 4$
3 2$

This is **not valid**:

./a.out 5 3 2 1 4 | cat -e 
4 1$
2 3$

Pay attention to the case when the target number is 0, because "nothing" is considered a valid possibility, so you must print an empty line as a subset.

Example:

./a.out 0 1 -1 2 -2 | cat -e
$
-1 1 2 -2$
-1 1$
2 -2$
```