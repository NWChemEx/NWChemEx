# This script will build the documentation for the sde, which includes:
#
# - Generating Doxgyen documentation for the C++ APIs
#   - This is actually done by Sphinx
# - Parse the test examples into documentation

import os
import subprocess
import urllib.request

if not os.path.exists("make_tutorial.py"):
    make_tutorial_url = "https://raw.githubusercontent.com/NWChemEx-Project/" \
                        "DeveloperTools/master/scripts/make_tutorials.py"
    urllib.request.urlretrieve(make_tutorial_url, "make_tutorials.py")

from make_tutorials import make_tutorials


def main():
    docs_dir = os.path.dirname(os.path.realpath(__file__))
    root_dir = os.path.dirname(docs_dir)
    examples_dir = os.path.join(root_dir, "tests", "examples")
    #tutorial_dir = os.path.join(docs_dir, "source", "tutorials")

    #if not os.path.exists(tutorial_dir):
    #    os.mkdir(tutorial_dir)

    #make_tutorials(examples_dir, tutorial_dir)
    subprocess.call(["make", "html"])


if __name__ == "__main__":
    main()
