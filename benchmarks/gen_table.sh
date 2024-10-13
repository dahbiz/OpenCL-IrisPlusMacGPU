#!/bin/bash

# Define the input file containing the results
INPUT_FILE=$1  # Ensure this matches your actual input file name

# Output file for the formatted table
OUTPUT_FILE="formatted_table.txt"

# Initialize the output file with headers
echo -e "Size\tCPUTime[s]\tGPUTime[s]" > "$OUTPUT_FILE"

# Read the input file line by line
while IFS= read -r line; do
    # Debug: print the current line being processed
    echo "Processing line: $line"  # This will show what line is being read

    # Extract size, CPU time, and GPU time using pattern matching
    if [[ $line =~ CPU\ Time\ for\ size\ ([0-9]+):\ ([0-9.]+)\ seconds ]]; then
        SIZE="${BASH_REMATCH[1]}"
        CPU_TIME="${BASH_REMATCH[2]}"
    fi

    if [[ $line =~ GPU\ Time\ for\ size\ ([0-9]+):\ ([0-9.]+)\ seconds ]]; then
        GPU_TIME="${BASH_REMATCH[2]}"
    fi

    # If all three values are extracted, append them to the output file
    if [[ -n "$SIZE" && -n "$CPU_TIME" && -n "$GPU_TIME" ]]; then
        echo -e "$SIZE\t$CPU_TIME\t$GPU_TIME" >> "$OUTPUT_FILE"
        # Reset the variables for the next loop iteration
        SIZE=""
        CPU_TIME=""
        GPU_TIME=""
    fi
done < "$INPUT_FILE"

echo "Table has been written to $OUTPUT_FILE."