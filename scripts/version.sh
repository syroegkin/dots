#!/bin/sh
# Semantic-version helper for the dot commands.
#
# Each command directory owns a VERSION file (the source of truth). This script
# reads it, bumps it, and stamps the value into that command's <name>.zxpkg.toml
# manifest and README.md. The build calls `sync` so the published files never
# drift from VERSION; bumping is an explicit, separate action.
#
# Usage:
#   version.sh current <dir>              print the current version
#   version.sh bump    <dir> patch|minor|major   bump VERSION, then sync
#   version.sh sync    <dir>              stamp VERSION into the toml + README
set -eu

cmd=${1:-}
dir=${2:-}

if [ -z "$cmd" ] || [ -z "$dir" ]; then
	echo "usage: version.sh current|bump|sync <dir> [part]" >&2
	exit 2
fi

name=$(basename "$(cd "$dir" && pwd)")
version_file="$dir/VERSION"
toml="$dir/$name.zxpkg.toml"
readme="$dir/README.md"

read_version() {
	if [ ! -f "$version_file" ]; then
		echo "no VERSION file at $version_file" >&2
		exit 1
	fi
	# trim whitespace
	sed -e 's/[[:space:]]//g' "$version_file"
}

sync_files() {
	ver=$1
	if [ ! -f "$toml" ]; then
		echo "no manifest at $toml" >&2
		exit 1
	fi
	# Manifest: rewrite the single version key under [package].
	sed -i -E "s/^version[[:space:]]*=.*/version     = \"$ver\"/" "$toml"

	# README: rewrite the `vX.Y.Z` badge line (must already exist).
	if [ -f "$readme" ] && grep -qE '^`v[0-9]' "$readme"; then
		sed -i -E "s/^\`v[0-9][^\`]*\`/\`v$ver\`/" "$readme"
	elif [ -f "$readme" ]; then
		echo "warning: no version badge in $readme (expected a line like \`v1.0.0\`)" >&2
	fi
	echo "$name -> v$ver (synced $toml, $readme)"
}

case "$cmd" in
current)
	read_version
	;;
sync)
	sync_files "$(read_version)"
	;;
bump)
	part=${3:-patch}
	ver=$(read_version)
	# split X.Y.Z
	major=${ver%%.*}
	rest=${ver#*.}
	minor=${rest%%.*}
	patch=${rest#*.}
	case "$part" in
	major) major=$((major + 1)); minor=0; patch=0 ;;
	minor) minor=$((minor + 1)); patch=0 ;;
	patch) patch=$((patch + 1)) ;;
	*) echo "unknown bump part '$part' (use patch|minor|major)" >&2; exit 2 ;;
	esac
	new="$major.$minor.$patch"
	printf '%s\n' "$new" >"$version_file"
	sync_files "$new"
	;;
*)
	echo "unknown command '$cmd'" >&2
	exit 2
	;;
esac
