#!/bin/bash

TIPOS=("Restricted" "Relaxed")
WIDTHS=(500 1000 2000 5000 10000 20000)
OUTPUT_FILE="../results_rust_scheduler.csv"

FILES=()
while IFS= read -r -d '' file; do
    FILES+=("$(basename "$file")")
done < <(find ../resources/scheduler/ -type f -print0 | sort -z)

for archivo_path in "${FILES[@]}"; do
    INPUT_FILE="${archivo_path}"

    if [[ -z "$INPUT_FILE" ]]; then
        echo "❌ Archivo vacío — se salta esta iteración"
        continue
    fi

    for COMP_TYPE in "${TIPOS[@]}"; do
        if [[ "$COMP_TYPE" == "Restricted" || "$COMP_TYPE" == "Relaxed" ]]; then
            for MAX_WIDTH in "${WIDTHS[@]}"; do
                echo "🚀 Ejecutando con archivo: $INPUT_FILE, COMP_TYPE: $COMP_TYPE, width: $MAX_WIDTH"
                cargo run --example compare_scheduler -- "$INPUT_FILE" "$OUTPUT_FILE" "$COMP_TYPE" "$MAX_WIDTH"
            done
        else
            # Exact no necesita width, se entrega un dummy por compatibilidad
            echo "🚀 Ejecutando con archivo: $INPUT_FILE, COMP_TYPE: $COMP_TYPE"
            cargo run --example compare_scheduler -- "$INPUT_FILE" "$OUTPUT_FILE" "$COMP_TYPE" 99999
        fi
    done
done
