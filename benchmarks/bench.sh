#!/bin/bash

# LIST OF SIZES
SIZES="100 200 300 400 500 600 700 800 900 1000 1100 1200 1300 1400 1500 1600 1700 1800 1900 2000 2100 2200 2300 2400 2500 2600 2700 2800 2900 3000 3100 3200 3300 3400 3500 3600 3700 3800 3900 4000"

# COMMAND TO RUN
COMMAND="./mat_mul"  # Replace with your command

# OUTPUT FILE
OUTPUT_FILE="output.txt"

# CLEAR OUTPUT FILE IF IT EXISTS
> "$OUTPUT_FILE"

# Declare arrays to hold results
declare -a SIZE_ARRAY
declare -a CPU_TIME_ARRAY
declare -a GPU_TIME_ARRAY

# LOOP THROUGH EACH SIZE
for SIZE in $SIZES; do
    echo "Running command with size: $SIZE"

    # RUN COMMAND AND CAPTURE OUTPUT
    OUTPUT=$($COMMAND $SIZE 2>&1)  # Pass SIZE as an argument to your command

    # CHECK IF COMMAND EXECUTED SUCCESS
    if [ $? -eq 0 ]; then
        echo "Command executed successfully for size $SIZE!"
    else
        echo "Command failed with error for size $SIZE:"
        echo "$OUTPUT"
        continue  # Skip to the next size
    fi

    # FILTER OUTPUT FOR CPU AND GPU TIMINGS
    CPU_TIME=$(echo "$OUTPUT" | grep "CPU matrix multiplication took:" | awk '{print $(NF-1)}')  # Get the second last field
    GPU_TIME=$(echo "$OUTPUT" | grep "GPU matrix multiplication took:" | awk '{print $(NF-1)}')  # Get the second last field

    # Store results in arrays, ensuring to capture as floats
    SIZE_ARRAY+=($SIZE)
    CPU_TIME_ARRAY+=($CPU_TIME)
    GPU_TIME_ARRAY+=($GPU_TIME)

    # PRINT AND WRITE OUTPUT
    if [ -n "$CPU_TIME" ]; then
        echo "CPU Time for size $SIZE: $CPU_TIME seconds"
        echo "CPU Time for size $SIZE: $CPU_TIME seconds" >> "$OUTPUT_FILE"
    fi

    if [ -n "$GPU_TIME" ]; then
        echo "GPU Time for size $SIZE: $GPU_TIME seconds"
        echo "GPU Time for size $SIZE: $GPU_TIME seconds" >> "$OUTPUT_FILE"
    fi

    echo "" >> "$OUTPUT_FILE"  # Add a newline for separation
done

# Generate table at the end
echo -e "\nSize\tCPUTime[s]\tGPUTime[s]" > summary_table.txt
for i in "${!SIZE_ARRAY[@]}"; do
    echo -e "${SIZE_ARRAY[$i]} \t${CPU_TIME_ARRAY[$i]} \t${GPU_TIME_ARRAY[$i]}" >> summary_table.txt
done

echo "All relevant outputs have been written to $OUTPUT_FILE"
echo "Summary table has been written to summary_table.txt"