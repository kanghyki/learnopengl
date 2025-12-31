#!/usr/bin/env bash
set -e

EXCLUDES=(
    imgui
)
EXCLUDE_REGEX=$(printf "^%s/|" "${EXCLUDES[@]}")
EXCLUDE_REGEX="${EXCLUDE_REGEX%|}"

FILES=$(git ls-files \
  '*.c' '*.cpp' '*.cc' '*.cxx' \
  '*.h' '*.hpp' '*.hh' '*.hxx' \
  '*.vert' '*.frag' \
| grep -Ev "$EXCLUDE_REGEX")

[ -z "$FILES" ] && exit 0

echo "Formatting $(echo "$FILES" | wc -l) files..."
echo "$FILES" | xargs -P$(nproc) clang-format -i
