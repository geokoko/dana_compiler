#!/usr/bin/env bash
set -euo pipefail

repo_root="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"

baseline="${1:-}"
candidate="${2:-"$repo_root/src/danac"}"
programs_dir="${PROGRAMS_DIR:-"$repo_root/tests/programs"}"
repeats="${REPEATS:-7}"
warmups="${WARMUPS:-2}"
opt_level="${OPT_LEVEL:--O0}"

if [[ -z "$baseline" ]]; then
	echo "usage: $0 <baseline-compiler> [candidate-compiler]" >&2
	exit 1
fi

if [[ ! -x "$baseline" ]]; then
	echo "baseline compiler is missing or not executable: $baseline" >&2
	exit 1
fi

if [[ ! -x "$candidate" ]]; then
	echo "candidate compiler is missing or not executable: $candidate" >&2
	exit 1
fi

mapfile -t programs < <(find "$programs_dir" -maxdepth 1 -type f -name '*.dana' | sort)

if [[ "${#programs[@]}" -eq 0 ]]; then
	echo "no .dana programs found in $programs_dir" >&2
	exit 1
fi

baseline_dir="$(mktemp -d)"
candidate_dir="$(mktemp -d)"

cleanup() {
	rm -rf "$baseline_dir" "$candidate_dir"
}
trap cleanup EXIT

stage_programs() {
	local workspace="$1"
	local source
	for source in "${programs[@]}"; do
		ln -sf "$source" "$workspace/$(basename "$source")"
	done
}

cleanup_outputs() {
	local workspace="$1"
	local stem="$2"
	rm -f \
		"$workspace/$stem.ll" \
		"$workspace/$stem.asm" \
		"$workspace/$stem.o" \
		"$workspace/a.out"
}

run_corpus() {
	local binary="$1"
	local workspace="$2"
	local start end elapsed
	local source stem linked

	start="$(date +%s%N)"
	for source in "${programs[@]}"; do
		stem="$(basename "${source%.dana}")"
		linked="$workspace/$(basename "$source")"
		cleanup_outputs "$workspace" "$stem"
		"$binary" "$opt_level" "$linked" >/dev/null 2>/dev/null
	done
	end="$(date +%s%N)"
	elapsed=$((end - start))
	echo "$elapsed"
}

median_ns() {
	printf '%s\n' "$@" | sort -n | awk '
		{
			values[NR] = $1
		}
		END {
			if (NR == 0) {
				exit 1
			}
			mid = int((NR + 1) / 2)
			if (NR % 2 == 1) {
				print values[mid]
			} else {
				print int((values[mid] + values[mid + 1]) / 2)
			}
		}
	'
}

format_ns() {
	awk -v ns="$1" 'BEGIN { printf "%.6f", ns / 1000000000 }'
}

percent_speedup() {
	awk -v base="$1" -v cand="$2" 'BEGIN {
		if (base == 0) {
			print "0.00"
		} else {
			printf "%.2f", ((base - cand) / base) * 100.0
		}
	}'
}

stage_programs "$baseline_dir"
stage_programs "$candidate_dir"

for ((i = 0; i < warmups; ++i)); do
	run_corpus "$baseline" "$baseline_dir" >/dev/null
	run_corpus "$candidate" "$candidate_dir" >/dev/null
done

baseline_samples=()
candidate_samples=()

for ((i = 0; i < repeats; ++i)); do
	if (( i % 2 == 0 )); then
		baseline_samples+=("$(run_corpus "$baseline" "$baseline_dir")")
		candidate_samples+=("$(run_corpus "$candidate" "$candidate_dir")")
	else
		candidate_samples+=("$(run_corpus "$candidate" "$candidate_dir")")
		baseline_samples+=("$(run_corpus "$baseline" "$baseline_dir")")
	fi
done

baseline_median="$(median_ns "${baseline_samples[@]}")"
candidate_median="$(median_ns "${candidate_samples[@]}")"
delta_ns=$((candidate_median - baseline_median))

echo "Programs benchmarked: ${#programs[@]}"
echo "Optimization level:   $opt_level"
echo "Warmups:              $warmups"
echo "Measured repeats:     $repeats"
echo
echo "Baseline median:      $(format_ns "$baseline_median")s"
echo "Candidate median:     $(format_ns "$candidate_median")s"
echo "Median delta:         $(format_ns "$delta_ns")s"
echo "Median speedup:       $(percent_speedup "$baseline_median" "$candidate_median")%"
echo
echo "Raw samples (seconds):"
printf '  baseline:'
for sample in "${baseline_samples[@]}"; do
	printf ' %ss' "$(format_ns "$sample")"
done
printf '\n'
printf '  candidate:'
for sample in "${candidate_samples[@]}"; do
	printf ' %ss' "$(format_ns "$sample")"
done
printf '\n'
