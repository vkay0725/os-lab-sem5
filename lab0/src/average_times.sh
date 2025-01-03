#!/bin/bash

# Initialize variables to accumulate times
total_read_time=0
total_s1_time=0
total_s2_time=0
total_s3_time=0
total_write_time=0

# Run the process 5 times
for i in {1..5}; do
    # Run the program and capture the output
    output=$(./a.out ../images/$1.ppm ../images/$2.ppm)

    # Extract the times from the output using grep and awk
    read_time=$(echo "$output" | grep "File Read" | awk '{print $3}')
    s1_time=$(echo "$output" | grep "S1 Smoothen" | awk '{print $3}')
    s2_time=$(echo "$output" | grep "S2 Find Details" | awk '{print $4}')
    s3_time=$(echo "$output" | grep "S3 Sharpen" | awk '{print $3}')
    write_time=$(echo "$output" | grep "File Write" | awk '{print $3}')

    # Ensure the times are valid numbers before accumulating
    total_read_time=$(echo "$total_read_time + ${read_time:-0}" | bc)
    total_s1_time=$(echo "$total_s1_time + ${s1_time:-0}" | bc)
    total_s2_time=$(echo "$total_s2_time + ${s2_time:-0}" | bc)
    total_s3_time=$(echo "$total_s3_time + ${s3_time:-0}" | bc)
    total_write_time=$(echo "$total_write_time + ${write_time:-0}" | bc)
done

# Calculate averages
average_read_time=$(echo "scale=2; $total_read_time / 5" | bc)
average_s1_time=$(echo "scale=2; $total_s1_time / 5" | bc)
average_s2_time=$(echo "scale=2; $total_s2_time / 5" | bc)
average_s3_time=$(echo "scale=2; $total_s3_time / 5" | bc)
average_write_time=$(echo "scale=2; $total_write_time / 5" | bc)

# Print the average times
echo "Average Time taken (ms):"
echo "File Read: $average_read_time ms"
echo "S1 Smoothen: $average_s1_time ms"
echo "S2 Find Details: $average_s2_time ms"
echo "S3 Sharpen: $average_s3_time ms"
echo "File Write: $average_write_time ms"
