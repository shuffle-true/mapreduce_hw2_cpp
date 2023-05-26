import subprocess
import json
import os

def check_out(condition: bool):
    if not condition:
        raise RuntimeError("Check not passed")
    else:
        print("Passed")

def test(command):
    subprocess.run([command], stdout=subprocess.PIPE, encoding='ascii')

    check_out(str(json.load(open("../build/Debug/reducer_0"))) == "{'A': 1, 'And': 1, 'Answer': 1, 'Between': 1}")
    check_out(str(json.load(open("../build/Debug/reducer_1"))) == "{'I': 1, 'Is': 1, 'Me': 1, 'Ok': 1, 'To': 1}")
    check_out(str(json.load(open("../build/Debug/reducer_2"))) == "{'What': 1, 'You': 1, 'boy': 1, 'bullshit': 1, 'difference': 1}")
    check_out(str(json.load(open("../build/Debug/reducer_3"))) == "{'that': 1, 'the': 1, 'try': 1, 'understand': 1, 'you': 1}")

if __name__ == '__main__':
    test("../build/Debug/mr_main")
