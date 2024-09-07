# Advanced Operating Systems  
## POSIX Shell Implementation

### Project Overview
This project is a POSIX-compliant shell implemented in C++. The primary goal is to create a user-defined interactive shell that can manage processes effectively. The shell is capable of handling system programs (like `emacs`, `vi`) and user-defined executables, managing both background and foreground processes. Additionally, it supports input/output redirections and pipes.

### Shell Prompt Display
When you execute the code, the shell will display a prompt in the following format:

`username@system_name:current_directory>`


### Built-in Commands: 

- **`cd` (Change Directory)**:
  - Changes the current working directory of the shell.
  - The prompt will update to reflect the new directory.
  - **Basic Usage**:
    - `cd <directory>`: Changes to the specified directory.
    - `cd ..`: Moves up one directory level. 
        -  If this command is executed from the home directory, the absolute path will be displayed in the prompt.
    - `cd -`: Changes to the previous directory.
    - `cd ~`: Changes to the home directory.
    - `cd .`: Stays in the current directory.
  - **Error Handling**:
    - If more than one argument is provided, then it will display: `Invalid arguments`.
    - If no argument is provided, it will change to the home directory.

- **`echo`**:
  - Outputs the provided string to the terminal.
  - **Note**:
    - Multi-line strings and environment variable handling are not supported.
    - Escape flags and quotes are not supported.
    - Tabs and spaces are supported.
    - Example:
      ```bash
      echo "abc 'ac' abc"  # Outputs: abc 'ac' abc
      ```

- **`pwd` (Print Working Directory)**:
  - Prints the absolute path of the current working directory.

- **`ls`**:

    - This command lists directory contents.
    
    - **Flags**:
      - `-a`: Lists all entries including hidden files (those starting with a dot).
      - `-l`: Uses a long listing format to display detailed information about each file.
    
    - **Supported Cases**:
      - `ls`: Lists contents of the current directory.
      - `ls -a`: Lists all files, including hidden files.
      - `ls -l`: Lists files with detailed information (permissions, number of links, owner, group, size, and time of last modification).
      - `ls .`: Lists contents of the current directory.
      - `ls ..`: Lists contents of the parent directory.
      - `ls ~`: Lists contents of the home directory.
      - `ls -a -l`: Lists all files with detailed information.
      - `ls -la` or `ls -al`: Lists all files with detailed information (alternative flags order).
      - `ls <Directory/File_name>`: Lists contents of the specified directory or details of the specified file.
      - `ls -<flags> <Directory/File_name>`: Supports combining flags with a directory or file name.

- **`pinfo`**:

    - This command prints process-related information for the shell program or a specified process.
    
    - **Usage**:
      - **`pinfo`**: Displays information about the shell process.
        - **Example**:
          ```bash
          <Name@UBUNTU:~>
          pinfo
          pid -- 231
          Process Status -- {R/S/S+/Z}
          memory -- 67854 {Virtual Memory}
          Executable Path -- ~/a.out
          ```
    
      - **`pinfo <pid>`**: Displays information about the process with the given PID.
        - **Example**:
          ```bash
          <Name@UBUNTU:~>
          pinfo 7777
          pid -- 7777
          Process Status -- {R/S/S+/Z}
          memory -- 123456 {Virtual Memory}
          Executable Path -- /usr/bin/gcc
          ```
    
    - **Process Status Codes**:
      - `R`/`R+`: Running
      - `S`/`S+`: Sleeping in an interruptible wait
      - `Z`: Zombie
      - `T`: Stopped (on a signal)
      - **Note**: Displays `+` status code if the process is in the foreground.

- **`search`**:

    - This command searches for a specified file or folder under the current directory recursively. The output indicates whether the file or folder exists.
    
    - **Usage**:
      - **`search <filename_or_directory>`**: Searches for the specified file or directory recursively starting from the current directory.
        - **Example**:
          ```bash
          <Name@UBUNTU:~> search xyz.txt
          True
          ```
        - If the file or directory is found, outputs `True`.
        - If the file or directory is not found, outputs `False`.
        
- **`history`**:

    - The `history` command displays the command history of the shell, tracking commands across all sessions.
    
    - **Usage**:
      - **`history`**: Displays the last 10 commands executed.
        - **Example**:
          ```bash
          <Name@UBUNTU:~> ls
          <Name@UBUNTU:~> cd
          <Name@UBUNTU:~> cd
          <Name@UBUNTU:~> history
          ls
          cd
          history
          ```
    
      - **`history <num>`**: Displays the last `<num>` commands, where `<num>` is a number less than or equal to 20.
        - **Example**:
          ```bash
          <Name@UBUNTU:~> history 3
          ls
          cd
          history
          ```
    
    - **Command Storage**:
      - The shell stores up to 20 commands. Older commands are overwritten when this limit is exceeded.
      - Commands will be tracked across all shell sessions.
    
    - **Up Arrow Key Functionality**:
      - Pressing the UP arrow key cycles through previous commands in the shell’s history.
      - The prompt displays the previous commands one by one.
      - If the UP arrow is pressed when the prompt is not empty, the command should not be affected.
      - If the UP arrow is pressed beyond the first command, the prompt remains on the first command.

### System Commands: Foreground and Background Processes

The shell can execute system commands in either foreground or background.

- **Foreground Processes**:
  - A command executed without `&` will run as a foreground process.
  - The shell will wait for the foreground process to complete before accepting new commands.
  - **Example**:
    ```bash
    <Name@UBUNTU:~> vi
    # Shell waits until 'vi' is exited, then regains control.
    ```

- **Background Processes**:
  - A command followed by `&` will run as a background process.
  - The shell will not wait for the background process to complete and will immediately accept new commands.
  - Upon starting a background process, the shell will print the Process ID (PID) of the new process.
  - **Example**:
    ```bash
    <Name@UBUNTU:~> gedit &
    456  # PID of the background process
    <Name@UBUNTU:~> ls -l -a
    ```

- **Key Notes**:
  - Does not support background processing for built-in commands such as `ls`, `echo`, `cd`, `pwd`, and `pinfo`.
  - It supports running **multiple background processes** simultaneously.
  - Running `pinfo` on any of these background processes will display their information correctly.
  - The shell will continue to handle both foreground and background processes smoothly, without interruptions.

  ### I/O Redirection

The shell supports input and output redirection using the symbols `<`, `>`, and `>>`. This allows the output of commands (usually written to `stdout`) to be redirected to a file, or input to be taken from a file other than `stdin`. Both input and output redirection can be used simultaneously.

- **Input Redirection (`<`)**:
  - Redirects the input of a command from a specified file.
  - If the input file does not exist, an error message will be displayed.

- **Output Redirection (`>` and `>>`)**:
  - `>`: Redirects the output of a command to a specified file, overwriting the file if it already exists.
  - `>>`: Redirects the output of a command to a specified file, appending to the file if it already exists.
  - If the output file does not exist, it wil be created with permissions `0644`.

- **Simultaneous Input and Output Redirection**:
  - The shell handles cases where both input and output redirection are used together.

- **Examples**:
  - **Output Redirection**:
    ```bash
    <Name@UBUNTU:~> echo "hello" > output.txt
    ```
  - **Input Redirection**:
    ```bash
    <Name@UBUNTU:~> cat < example.txt
    ```
  - **Input and Output Redirection**:
    ```bash
    <Name@UBUNTU:~> sort < file1.txt > lines_sorted.txt
    ```

### Pipelines

The shell supports the use of pipelines, allowing the output of one command to be used as the input to another command using the pipe symbol (`|`). This enables the chaining of commands to perform complex operations. Your shell must support any number of pipes in a single command.

- **Usage**:
  - A pipe (`|`) takes the output of the command on the left and uses it as the input for the command on the right.
  - You can chain multiple commands together using multiple pipes.

- **Examples**:
  - **Two Commands**:
    ```bash
    <Name@UBUNTU:~> cat file.txt | wc
    ```
    - This command outputs the contents of `file.txt` and pipes it to `wc`, which counts the lines, words, and characters.

  - **Three Commands**:
    ```bash
    <Name@UBUNTU:~> cat sample2.txt | head -7 | tail -5
    ```
    - This command outputs the contents of `sample2.txt`, pipes it to `head -7` to get the first 7 lines, and then pipes that output to `tail -5` to get the last 5 lines of the first 7 lines.

### Simple Signals

The shell supports the following simple signal controls:

1. **CTRL-Z (`SIGTSTP`)**:
   - When pressed, this should push any currently running foreground job into the background and change its state from running to stopped.
   - If no foreground process is running, pressing `CTRL-Z` will have no effect on the shell.

2. **CTRL-C (`SIGINT`)**:
   - When pressed, this should interrupt any currently running foreground job by sending it the `SIGINT` signal.
   - If no foreground process is running, pressing `CTRL-C` will have no effect on the shell.

3. **CTRL-D (End-of-File)**:
   - When pressed, `CTRL-D` will log you out of the shell and terminate the shell session.

### Autocomplete

The shell supports tab-completion for all files and directories under the current directory. When the `TAB` key is pressed, the shell will either auto-complete the command or output a list of matching files or directories if there are multiple matches.

- **Functionality**:
  - **Single Match**:
    - If there is only one file or directory that matches the current input, pressing `TAB` will auto-complete the name.
    - **Example**:
      ```bash
      <Name@UBUNTU:~> cat alp<TAB>
      # Auto-completes to:
      <Name@UBUNTU:~> cat alpha.txt
      ```

  - **Multiple Matches**:
    - If there are multiple files or directories that match the current input, pressing `TAB` will display a space-separated list of all matching names.
    - The shell will not auto-complete until the user types enough characters to reduce the matches to a single option.
    - **Example**:
      ```bash
      <Name@UBUNTU:~> cat al<TAB>
      # Outputs:
      alpha.txt alnum.txt
      ```
