#!/bin/bash

TIPOS=("Exact" "Restricted" "Relaxed")
WIDTHS=(500 1000 2000 5000 10000 20000)
OUTPUT_FILE="../results_rust.csv"

FILES=()
while IFS= read -r -d '' file; do
    FILES+=("$(basename "$file")")
done < <(find ../resources/knapsack/ -type f -print0 | sort -z)

for archivo_path in "${FILES[@]}"; do
    INPUT_FILE="${archivo_path}"

    if [[ -z "$INPUT_FILE" ]]; then
        echo "❌ Empty file — skipping this iteration"
        continue
    fi

    for COMP_TYPE in "${TIPOS[@]}"; do
        if [[ "$COMP_TYPE" == "Restricted" || "$COMP_TYPE" == "Relaxed" ]]; then
            for MAX_WIDTH in "${WIDTHS[@]}"; do
                echo "🚀 Running with file: $INPUT_FILE, COMP_TYPE: $COMP_TYPE, width: $MAX_WIDTH"
                cargo run --example compare_knapsack -- "$INPUT_FILE" "$OUTPUT_FILE" "$COMP_TYPE" "$MAX_WIDTH"
            done
        else 
            # Exact does not need a width; a dummy is passed for compatibility
            echo "🚀 Running with file: $INPUT_FILE, COMP_TYPE: $COMP_TYPE"
            cargo run --example compare_knapsack -- "$INPUT_FILE" "$OUTPUT_FILE" "$COMP_TYPE" 99999
        fi
    done
done

