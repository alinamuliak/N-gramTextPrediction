#!/usr/bin/python

import subprocess
import sys
import os
import platform
import configparser as cp

PROGRAM_PATH = os.path.join("..", "bin", "word_counter")

if __name__ == "__main__":
    if len(sys.argv) < 2:
        print("Not enough arguments", file=sys.stderr)
        exit(1)

    number_of_runs = 1
    try:
        number_of_runs = int(sys.argv[1])
    except:
        print("Specify the number of runs", file=sys.stderr)
        exit(2)

    try:
        config_file = sys.argv[2]
    except:
        if os.path.isfile("index.cfg"):
            config_file = "index.cfg"
        else:
            print("Specify the name of config file", file=sys.stderr)
            exit(2)

    parser = cp.ConfigParser()
    with open(config_file, 'r') as f:
        config_str = "[section]\n" + f.read()
    parser.read_string(config_str)
    out_by_a = parser.get("section", "out_by_a").split('"')[1]
    out_by_n = parser.get("section", "out_by_n").split('"')[1]

    number_of_words = {}
    args = [PROGRAM_PATH, config_file]
    min_time = float("inf")
    for i in range(number_of_runs):
        process = subprocess.Popen(args, stdout=subprocess.PIPE)
        process.wait()
        (output, error) = process.communicate()
        # print(output)
        total_time = int(str(output)[2:-3].split('\\n')[0].split('=')[1])
        exit_code = process.wait()

        if exit_code != 0:
            print(str(error)[2:-3], file=sys.stderr)
            exit()
        min_time = min(min_time, total_time)
        with open(out_by_n, 'r') as f:
            numerical = set(f.readlines())
        with open(out_by_a, 'r') as f:
            alphabetical = set(f.readlines())
        if len(numerical) != len(alphabetical) or len(numerical.intersection(alphabetical)) != len(numerical):
            print(f"Results in output files at run {i} do not coincide", file=sys.stderr)
            exit(4)

    print("All results are the same")
    print(min_time)
