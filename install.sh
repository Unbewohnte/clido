program=clido
target=/usr/local/bin
todo_target=/usr/local/share/clido


if [ -f "$program" ]; then
    echo "Creating $todo_target ..."
    mkdir -p $todo_target && \
    touch $todo_target/TODOS.bin && \
    echo "Setting rights ..." && \
    chmod -R a+rw $todo_target && \
    echo "Moving $program to $target ..." && \
    mv $program $target && \
    echo "Successfully installed!"
else
    echo "$program does not exist!"
fi