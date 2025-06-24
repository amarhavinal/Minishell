By definition, the Operating System (OS) acts as an interface between the user and the hardware. For the user the interface can be given either in form of Graphical User Interface (GUI) or Command Line Interface (CLI). Windows OS is a typical example for GUI example. Before the GUI supported by operating system, it was mainly operated by CLI. It was given different names in different Operating Systems. Some popular examples being DOS prompt of Windows or BASH prompt of Linux. 

 

These interfaces typically run in a text window, allowing the user to type commands which cause actions. Upon receiving commands the OS will parse the commands for its correctness and invoke appropriate application to perform a specific action. These action can range from opening a file to terminating an application. In case of Linux the BASH shell reads commands from a file, called a script. Like all Unix shells, it supports piping and variables as well, and eventually provides an output. 

 

The goal of this Linux Internals Projects for Beginners is to implement a mini-shell that mimics the BASH shell by using Linux Kernel System calls and IPC mechanisms like signals. It will handle a set of commands as called out below and also handle special keyboard actions (ex: Control C), can be extended for advanced functionalities (ex: Command history) as well. By implementing this Linux Internals Projects for Beginners as a developer you will get a real-time perspective about using Linux system calls to develop an important utility like CLI.



✅ Features / Requirements
Prompt Customization

Default shell prompt is msh >

Prompt can be changed using: PS1=NEW_PROMPT

Prompt change must not allow spaces (e.g., PS1 = NEW_PROMPT is invalid and treated as a regular command)

Command Execution

Executes external commands using child processes

Parent waits for child to finish before showing prompt again

Pressing Enter with no command simply redisplays the prompt

Special Shell Variables

echo $? → Displays exit status of the last executed command

echo $$ → Displays the PID of the msh shell

echo $SHELL → Displays the path of the msh executable

Signal Handling

Ctrl + C (SIGINT) → Sends signal to running foreground process; if none, redisplays prompt

Ctrl + Z (SIGTSTP) → Stops foreground process and prints its PID

Built-in Commands

exit → Terminates the msh shell

cd <directory> → Changes current working directory

pwd → Prints the current working directory

