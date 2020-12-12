from cs50 import get_int

# request height until it's [1,8]
height = 0
while (height < 1 or height > 8):
    height = get_int("Height: ")

# for each line, print the calculated amount of spaces and #. Number of # is equivalent to the row number (which begins with 0 b/c of range). Spaces is equivalent to remainder.
for row in range(height):
    print(" "*(height - row - 1) + "#"*(row + 1))

