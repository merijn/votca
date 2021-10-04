#!/bin/bash -le

shopt -s extglob

die () {
  [[ -n $1 ]] && echo "$*" >&2
  exit 1
}

[[ ${INPUT_BASE} ]] || die "No base sha"

filelist=( $(git diff --name-status --diff-filter=AMC "${INPUT_BASE}" | awk '{print $2}') )
new_date="$(date +%Y)"
echo "Updating Copyright date to ${new_date} in ${filelist[@]}"
sed -i 's/Copyright \(.*\)-.* The VOTCA/Copyright \1-'"$new_date"' The VOTCA/' "${filelist[@]}"

[[ ${CI} = 'true' ]] || exit 0
git add -u
git config user.name "Votca Bot"
git config user.email "github@votca.org"
git commit -m "Update copyright" || true
git push
