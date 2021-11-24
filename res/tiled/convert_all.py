from os import listdir, system
from os.path import isfile, join
import re
pattern = re.compile("^[0-9].json")
mapfiles = [f for f in listdir(".") if isfile(f) and pattern.match(f)]

for f in mapfiles:
    print("converting", f)
    system(f"python3 convert.py {f} map_{f}")

print("copying")
system(f"python3 copy_again.py ")