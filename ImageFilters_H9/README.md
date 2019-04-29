# Image Filters

Modify the image manipulation program done before to add a Convolution Filter.

This kind of filter can be used to produce effects on an image, like blur or edge detection. Here are some links that explain in more detail:

- [https://docs.gimp.org/2.8/en/plug-in-convmatrix.html](https://docs.gimp.org/2.8/en/plug-in-convmatrix.html)
- [https://en.wikipedia.org/wiki/Kernel_(image_processing)](https://en.wikipedia.org/wiki/Kernel_(image_processing))

Use getopt with the following options to indicate the operation to perform:
- i {input_file} : Indicate the file to read
- o {output_file} : Indicate the name of the image to create with the result
- f {filter_file} : Indicate the name of the file containing the convolution filter

The third file will contain a square matrix, where the first two values represent the sizes of the matrix, and then come the values in the matrix. The matrix must be square (both sides equal), and the sides must be odd numbers greater than 1 (ex.: 3, 5, 7, 9, etc.). Note: I use a blur matrix of size 15x15 for testing

The program should use parallelization to get the result faster. You can use OpenMP in the correct places to make your program parallel.

Remember to test to make sure that the parallel version is actually faster than the sequential one.

## How to view binary files in hexadecimal:

xxd file
xxd -b file     # Will show the bytes as binary numbers

hexdump -C file


Another option:
    Install the program hexedit
