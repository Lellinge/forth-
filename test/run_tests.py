#!/usr/bin/python3
import os
import subprocess
import sys


def main():
    success = True
    bin_path = "../cmake-build-debug/simpleforth"
    for file in os.listdir("."):
        # dieses programm selber überspringen
        if file.endswith(".py"):
            continue


        input_path = file + "/input.txt"
        output_path = file + "/output.txt"
        expected_path = file + "/expected.txt"
        input_file = open(input_path, 'r')
        output_file = open(output_path, 'w')

        # das zu testende programm ausführen
        result = subprocess.run([bin_path], stdout=output_file, stdin=input_file)
        print("directory: " + file + ": input was executed with return code: "  + str(result.returncode))

        # ausgabe mit der erwarteten vergleichen
        result_cmp = subprocess.run(["cmp", output_path, expected_path])
        final_message = ""
        if (result_cmp.returncode == 0):
            final_message = "was successfull"
        else:
            final_message = "failed: Expected output doesnt match real output"
            success = False

        print("directory: " + file + ": " + final_message)

    if (success):
        return 0
    else:
        # TODO Unix Konvention für Rückgabewerte beachten
        return 100





if __name__ == "__main__":
    sys.exit(main())