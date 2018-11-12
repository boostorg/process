#!/usr/bin/python
#

import sys
import subprocess

b2 = subprocess.Popen(sys.argv[1:], stdout=subprocess.PIPE, stderr=subprocess.STDOUT, shell=True)

while not b2.returncode:
    line = b2.stdout.readline()
    if not line: break;
    # If line is a 'noisy' warning, don't print it or the following two lines.
    if ('warning: section' in line and 'is deprecated' in line
            or 'note: change section name to' in line):
        next(sys.stdin)
        next(sys.stdin)
    else:
        sys.stdout.write(line)
        sys.stdout.flush()

exit(b2.returncode)