#!/usr/bin/python3
import os
import subprocess
import sys

CODE_END = '\033[0m'
CODE_BLUE = '\033[94m'
CODE_YELLOW = '\033[93m'
CODE_GREEN = '\033[92m'
CODE_RED = '\033[91m'

def color_str(str, color):
    return color + str + CODE_END

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
        ret_code_str = ""
        if result.returncode == 0:
            ret_code_str = color_str(str(result.returncode), CODE_GREEN)
        else:
            ret_code_str = color_str(str(result.returncode), CODE_RED)
        print(CODE_BLUE + "directory: " + CODE_END + CODE_YELLOW +  file  + CODE_END + ": input was executed with return code: "  + ret_code_str)

        # ausgabe mit der erwarteten vergleichen
        result_cmp = subprocess.run(["cmp", output_path, expected_path])
        final_message = ""
        if (result_cmp.returncode == 0):
            final_message = "was " + color_str("successfull", CODE_GREEN)
        else:
            final_message = color_str("failed:", CODE_RED) +  "Expected output doesnt match real output"
            success = False

        print(CODE_BLUE + "directory: " + CODE_END + CODE_YELLOW + file + CODE_END + ": " + final_message)

    if (success):
        return 0
    else:
        # TODO Unix Konvention für Rückgabewerte beachten
        return 100





if __name__ == "__main__":
    sys.exit(main())