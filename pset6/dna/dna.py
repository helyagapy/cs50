import sys
import csv
import re

# load DNA database and unidentified DNA sequence to compare consecutive matches and identify owner of DNA sequence

if len(sys.argv) != 3:
    sys.exit("Usage: python dna.py FILENAME.csv FILENAME.txt")

# load DNA data base and convert only STR counts into ints.
DNAbase = []
with open(sys.argv[1], "r") as STRfile:
    # declare reader to parse the file
    STRreader = csv.DictReader(STRfile)
    for row in STRreader:
        for k, v in row.items():
            if (k != "name"):
                row[k] = int(row[k])
        DNAbase.append(row)

# load unidentified DNA sequence
with open(sys.argv[2], "r") as seqfile:
    seqreader = seqfile.readlines()

# For line 29, using syntax from https://www.geeksforgeeks.org/python-maximum-consecutive-substring-occurrence/. Change array seqreader to str then use regular expressions findall method and max function to find longest consecutive repeat of key STR name.
seqCount = {}
for (k, v) in DNAbase[0].items():
    if (k != "name"):
        repetitions = max(re.findall('((?:' + re.escape(k) + ')*)', str(seqreader)))
        count = int((len(repetitions))/(len(k)))
        # store count and STR sequence in new dictionary
        seqCount.update({k: count})

# set counter to print "No match" if entire list of ordered dicts is traversed with no match
i = 0
for item in DNAbase:
    i += 1
    # line 39 consulted with non-student for help to copy dictionary without name key and value
    DNAbase_without_name = {k: v for k, v in item.items() if k != 'name'}
    # compare nameless dictionary with stored unidentified DNA sequence for match, break out of loop if matched.
    if (DNAbase_without_name == seqCount):
        print(item['name'])
        break
    elif i >= len(DNAbase):
        print("No match")