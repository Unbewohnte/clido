program=clido
target=/usr/local/bin

if [ -f "$program" ]; then
    echo "Moving $program to $target ..."
    mv $program $target && echo "Successfully installed!"
else
    echo "$program does not exist!"
fi