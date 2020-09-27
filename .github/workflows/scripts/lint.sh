#!/bin/sh

################################################################################
#
# The master lint.sh lives at NWChemEx-Project/DeveloperTools. The lint.sh file
# contained in all other NWChemEx-Project repositories is a synchronized
# copy of the master lint.sh. Thus to make changes to the lint.sh file please
# make them to the master .gitignore file.
#
################################################################################

# Wraps the process of running clang-format on the source code
#
# Usage:
#   lint [<dir2lint1> [<dir2lint2> [...]]]
# Optional Arguments:
#   dir2lint1: Path (either absolute or relative) to the first directory to
#              lint. Contents will be linted in place.
#   dir2lint2: Path (either absolute or relative) to the second directory to
#              lint. Contents will be linted in place.

set -e # Exit with error if any command fails

clang_format_cmd="clang-format-9"
for dir2lint in "$@"; do
  find "${dir2lint}" -name '*.hpp' -or -name '*.cpp' | \
  xargs "${clang_format_cmd}" -style=file -i -fallback-style=none
done
