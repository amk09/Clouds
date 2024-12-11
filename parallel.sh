#!/bin/bash

# Check if the user provided an integer as an argument
if [ $# -ne 1 ]; then
    echo "Usage: $0 <integer>"
    exit 1
fi

# Get the input integer and multiply it by 24
max_num=$(($1 * 24))

# Loop through numbers from 0 to max_num
for ((i=0; i<=max_num; i++)); do
    ./cloud "$i" & # Run each instance in the background
done

# Wait for all background processes to complete
wait

echo "All processes completed."
