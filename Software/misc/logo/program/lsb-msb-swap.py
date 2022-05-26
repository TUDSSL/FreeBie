import argparse
import re

def flipBitOrder(n):
  ret = 0
  for i in range(8):
    ret <<= 1
    ret |= (n & 1)
    n >>= 1
  return ret

parser = argparse.ArgumentParser(description='Convert .rom to c array')
parser.add_argument("-f", required=True, type=str, help="Filename")
args = parser.parse_args()

filepath = args.f
with open(filepath,"r") as fp:
  content = fp.readlines()
  reversed_lines = []
  # Go through each line in the array
  for line in content[6:]:
    # Break at the end of the array
    if not "0x" in line:
      break
    # Read the line and add the string hex numbers to a list
    hex_numbers = line.strip().replace(',','').split(' ')
    #print(hex_numbers)
    reversed_numbers = []
    # For each number change the bit order and add the them to a list
    for number in hex_numbers:
      reversed_numbers.append(hex(flipBitOrder(int(number, 16))))    
    #print(reversed_numbers)
    # Convert the list back to csv format
    reversed_lines.append(", ".join(reversed_numbers))
  with open(filepath.replace(".c","") + "_swapped.c","w") as wfp:
    # Copy the starting lines to the new file
    for line in content[0:6]:
      wfp.write(line)
    # Write the corrected lines to the file
    for line in reversed_lines:
      wfp.write(line + ',\r\n')
    # Close the array
    wfp.write("};\r\n")
    wfp.write("// Array bits flipped by lsb-msb-swap.py\r\n")
    
