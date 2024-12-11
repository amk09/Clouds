#!/bin/bash

# Check if the user provided an integer as an argument
if [ $# -ne 1 ]; then
    echo "Usage: $0 <integer>"
    exit 1
fi

# Get the input integer and multiply it by 24
max_num=$(($1 * 24))

# Initialize a counter for completed tasks
completed=0

# Create a function to handle progress updates
update_progress() {
    completed=$((completed + 1))
    echo -ne "Progress: $completed/$((max_num + 1)) completed\r"
}

# Loop through numbers from 0 to max_num
for ((i=0; i<=max_num; i++)); do
    # Run each instance in the background and update progress on completion
    (
        ./cloud "$i" 
        update_progress
    ) &
done

# Wait for all background processes to complete
wait

# Print final message
echo -e "\nAll processes completed."
