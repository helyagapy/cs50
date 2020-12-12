from cs50 import get_string
# Calculate Coleman-Liau readability index of text.

# Get text input
text = get_string("Text: ")

# Count only alpha characters for letters. Count spaces and, accounting for final word, add one to count total amount of words. For sentences, count the number of exclamation points, question marks, or periods aka sentence closers.
sentences = 0
for c in text:
    letters = sum(c.isalpha() for c in text)
    words = sum(c.isspace() for c in text) + 1
    if (c == "!" or c == "?" or c == "."):
        sentences += 1

# calculate L and S variables for Coleman-Liau formula and then round the index for an integer grade number.
letters_per_word = float(letters / words)
sentences_per_word = float(sentences / words)

L = float(letters_per_word * 100)
S = float(sentences_per_word * 100)
CL = round(0.0588 * L - 0.296 * S - 15.8)

if (CL < 1):
    print("Before Grade 1")
elif (CL >= 16):
    print("Grade 16+")
else:
    print(f"Grade {CL}")
