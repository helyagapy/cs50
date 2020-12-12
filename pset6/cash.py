from cs50 import get_float

quarter = 25
dime = 10
nickel = 5
penny = 1

# Get change amount until it's a non-negative value
owed = -1.0
while (owed < 0.0):
    owed = get_float("Change owed: ")

# To mitigate rounding errors convert owed to hundreds
owed = owed*100

# For total cents remaining, count every loop when one coin is subtracted from the amount owed. Subtract coins in descending value, from largest to smallest denomination.
counter = 0
while (owed > 0.0):
    if (owed >= quarter):
        owed = owed - quarter
    elif (owed >= dime):
        owed = owed - dime
    elif (owed >= nickel):
        owed = owed - nickel
    else:
        owed = owed - penny
    counter += 1

print(counter)