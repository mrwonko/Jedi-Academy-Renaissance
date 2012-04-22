#!/bin/python

# runs each test

import os, re

tests = []
for file in os.listdir():
    match = re.match("jar_test_([^\\.]+)\\.exe", file)
    if match:
        if match.span() == (0, len(file)):
            tests.append(match.group(1))

successful = 0
for test in tests:
    print("running test {}... ".format(test), end="")
    code = os.system("jar_test_{}.exe".format(test))
    if code == 0:
        print("success!")
        successful += 1
    elif code == 1: # error should return 1
        print("error!")
    else:
        print("error (code {})!".format(code))
            

print("\n{} out of {} tests were successful.".format(successful, len(tests)))
