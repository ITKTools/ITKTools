#This script is necessary to run two commands in the same test. We need to do this because the first command will be the executable that produces an output, and the second command will be running the comparison to the baseline output.
execute_process(COMMAND ${command1})
execute_process(COMMAND ${command2})
