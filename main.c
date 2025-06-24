//Name:Amar Havinal
//Date:03/03/2025
//Project:Minishell
#include "main.h"
#define MAX_INPUT_SIZE 100
int status;
int argc;                        // total number of arguments
int pid;                         // pid variable to store the pid of the process
char *cmd;                       // to store the first command
Slist *head = NULL;              // create variable head and stored with NULL
char *external[200];             // 2d array to store external commands
char input_string[100];          // string variable to get the input
char prompt[100] = "minishell$"; // default prompt message
char *input[100];                // 2d array to store the input string
int main()
{

    system("clear");
    scan_input(prompt, input_string);
}

void scan_input(char *prompt, char *input_string)
{
    signal(SIGINT, signal_handler); // signal handling with the help of singnal. for SIGINT
    signal(SIGTSTP, signal_handler);
    signal(SIGCHLD, sigchld_handler);
    // extract_external_command(external)
    extract_external_commands(external);
    int flag = 0;
    while (1)
    {
        // Print the prompt
        printf(ANSI_COLOR_GREEN "%s" ANSI_COLOR_RESET, prompt); // print the prompt message in green color

        // Read user input
        scanf("%[^\n]", input_string);
        __fpurge(stdin);
        flag = 0;
        // Remove trailing newline
        input_string[strcspn(input_string, "\n")] = '\0';

        // Ignore empty input
        if (strlen(input_string) == 0)
        {
            continue;
        }

        // Check if input is "PS1=" and ensure there's NO space after '='
        if (strncmp(input_string, "PS1=", 4) == 0)
        {
            if (input_string[4] != ' ')
            {

                // Copy the new prompt into the existing prompt array
                strcpy(prompt, input_string + 4);
                strcat(prompt,"$");
                prompt[MAX_INPUT_SIZE - 1] = '\0'; // Ensure null termination
                flag = 1;
                continue;
            }
        }
        else
        {

            cmd = get_command(input_string); // to get the first command
            if (strcmp(cmd, "jobs") == 0)    // to check the command is "jobs"
            {
                print_list(head); // function call to print the jobs
                continue;         // to skip further checks
            }
            else if (strncmp(input_string, "fg", 2) == 0) // to check "fg" command or not
            {
                if (head != NULL)
                {
                    printf("%d\n", head->pid);
                    kill(head->pid, SIGCONT); // continue execution in foreground
                    int status1;
                    waitpid(head->pid, &status1, WUNTRACED);
                    remove_job(pid);
                    head = head->link;
                }
                continue; 
            }
            else if (strncmp(input_string, "bg", 2) == 0) // check for "bg" command
            {
                if (head != NULL)
                {
                    printf("%d\n", head->pid);
                    kill(head->pid, SIGCONT); // continue execution in background
                    remove_job(pid);
                    head = head->link;
                }
                continue; // Add this
            }

            int ret = check_command_type(cmd);

            if (ret == EXTERNAL)
            {
                pid = fork();
                if (pid > 0) // parent process
                {

                    waitpid(pid, &status, WUNTRACED); // wait for child process to complete
                }
                else if (pid == 0) // child process
                {
                    signal(SIGINT, SIG_DFL);
                    signal(SIGTSTP, SIG_DFL);
                    signal(SIGCHLD, SIG_DFL);

                    execute_external_commands(input_string); // executive the input string
                }
                else
                {
                    perror("fork");
                }
            }
            else if (ret == BUILTIN)
            {
                // **Execute cd in the parent process**

                execute_internal_commands(input_string, prompt);
            }
            else if (flag != 1)
                printf("NO COMMAND\n");
        }
        memset(input_string, '\0', strlen(input_string)); // to assign '\0' to input_string after the execution of command
    }
}

void extract_external_commands(char **external_commands)
{
    int fd = open("cmd.txt", O_RDONLY);
    if (fd == -1)
    {
        perror("open");
        return;
    }

    char buffer[4096];
    ssize_t bytes_read = read(fd, buffer, sizeof(buffer) - 1);
    if (bytes_read <= 0)
    {
        perror("read");
        close(fd);
        return;
    }
    buffer[bytes_read] = '\0';

    int index = 0;
    char *start = buffer;
    for (int i = 0; i < bytes_read; i++)
    {
        if (buffer[i] == '\n')
        {
            buffer[i] = '\0'; // Replace newline with NULL terminator
            external_commands[index] = strdup(start);

            // Trim any trailing spaces
            external_commands[index][strcspn(external_commands[index], "\r\n ")] = '\0';

            index++;
            if (index >= 200 - 1)
                break;
            start = &buffer[i + 1];
        }
    }
    external_commands[index] = NULL;
    close(fd);
}

char *get_command(char *input_string)
{
    static char input[30];

    sscanf(input_string, "%s", input); // Extract first word
    // printf("DEBUG: Extracted command: [%s]\n", input);  // Debugging

    return input;
}

int check_command_type(char *command)
{
    // Built-in commands
    char *builtins[] = {"fg", "bg", "echo", "printf", "read", "cd", "pwd", "pushd", "popd", "dirs", "let", "eval",
                        "set", "unset", "export", "declare", "typeset", "readonly", "getopts", "source",
                        "exit", "exec", "shopt", "caller", "true", "type", "hash", "bind", "help", NULL};

    int i = 0;

    // Check built-in commands
    while (builtins[i] != NULL)
    {
        if (strcmp(builtins[i], command) == 0)
        {
            // printf("YES, it is a built-in command\n");
            return BUILTIN;
        }
        i++;
    }
    i = 0;
    while (external[i] != NULL)
    {
        if (strcmp(external[i], command) == 0)
        {
            // printf("YES, it is an external command\n");
            return EXTERNAL;
        }
        i++;
    }

    // printf("Command not found\n");
    return NO_COMMAND;
}
void execute_external_commands(char *input_string) // Function definition to execute the external commands
{
    char string[100];
    int l = 0, m = 0, i = 0;
    for (i = 0; input_string[i] != '\0'; i++) // to store the input string in input 2d array
    {
        if (input_string[i] != ' ')
        {
            string[l++] = input_string[i]; // assing the input string to string variable
        }
        else
        {
            if (input_string[i] == ' ' && input_string[i + 1] != ' ')
            {
                string[l] = '\0';
                l = 0;
                input[m] = malloc(strlen(string) * sizeof(char)); // dynamic memory allocation for each string
                strcpy(input[m], string);
                m++; // m variable represent the rows(no. of string)
            }
        }
    }
    if (input_string[i] != ' ' && input_string[i - 1] != ' ')
    {
        string[l] = '\0'; // assing the '\0' at the end of the string
        input[m] = malloc(strlen(string) * sizeof(char));
        strcpy(input[m], string);
        m++;
    }

    int flag = 0;
    for (int i = 0; i < m; i++)
    {
        if (strcmp(input[i], "|") == 0) // to check pipe is present or not in input_string
        {
            flag = 1;
        }
    }
    input[m + 1] = NULL;
    if (flag == 0) // means pipe is not present
    {
        execvp(input[0], input);
    }
    else // if pipe is present
    {
        if (n_pipe(input) == -1)
            printf("FAILURE\n");
    }
}

void execute_internal_commands(char *input_string, char *prompt)
{
    if (strcmp(input_string, "exit") == 0)
    {
        printf("Exiting..!\n");
        exit(0);
    }
    else if (strncmp(input_string, "cd ", 3) == 0) // Check if command starts with "cd "
    {
        char *path = input_string + 3; // Extract the directory path
        if (chdir(path) != 0)
        {
            perror("chdir failed");
        }
        else
        {
            getcwd(prompt, 100);  // Update prompt with new directory
            strcat(prompt, "$ "); // Append '$ ' to indicate shell prompt
        }
    }
    else if (strcmp(input_string, "pwd") == 0)
    {
        char cwd[1024];
        if (getcwd(cwd, sizeof(cwd)) != NULL)
        {
            printf("%s\n", cwd);
        }
        else
        {
            perror("getcwd failed");
        }
    }
    else if (strcmp(input_string, "echo $$") == 0)
    {
        printf("MSH PID=%d\n", getpid());
    }
    else if (strcmp(input_string, "echo $?") == 0)
    {
        printf("The exit status of the last command executed is %d\n", WEXITSTATUS(status));
    }
    else if (strcmp(input_string, "echo $SHELL") == 0)
    {
        char *shell_path = getenv("SHELL");
        if (shell_path)
        {
            printf("%s\n", shell_path);
        }
        else
        {
            printf("SHELL variable not set\n");
        }
    }
}
int n_pipe(char **input_string) // function definition to execute when input contains npipes
{
    int argc = 0;
    for (int i = 0; input_string[i] != NULL; i++)
    {
        argc++;
    }

    int pos = 0; // to find the position of last pipe
    for (int i = 0; i < argc; i++)
    {
        if (strcmp(input_string[i], "|") == 0)
        {
            pos = i;
        }
    }
    if (argc - 1 == pos) // it checks whether some command is present after the last pipe
    {
        printf("command is not present after pipe\n");
        return -1;
    }
    int c_pos[10];
    int c_count = 0;
    c_pos[0] = 0;
    for (int i = 0; i < argc; i++) // loop to make pipe place as null and store in commands position in array
    {
        if (strcmp(input_string[i], "|") == 0)
        {
            input_string[i] = NULL; // pipe position made as null
            c_count++;
            c_pos[c_count] = i + 1;
        }
    }
    c_count++;
    int fd[2];
    int pid;
    for (int i = 0; i < c_count; i++) // it runs upto total number of commands
    {
        if (i != c_count - 1)
            pipe(fd); // if i is not equal to command count-1 then it creates pipe

        pid = fork(); // create the child process
        if (pid > 0)  // parent process
        {
            if (i != c_count - 1)
            {
                close(fd[1]);   // close unused end
                dup2(fd[0], 0); // store fd[0] into STDIN
                close(fd[0]);
            }
            int status;
            wait(&status); // wait for child to complete
        }
        else if (pid == 0) // child process
        {
            if (i != c_count - 1)
            {

                close(fd[0]);   // close the unused end
                dup2(fd[1], 1); // store fd[1] into STDOUT
            }
            execvp(input_string[c_pos[i]], input_string + c_pos[i]); // to execute the command
        }
    }
    exit(0);
    return 0;
}
void signal_handler(int signum) // function definition to handle the singnals
{
    if (signum == SIGINT) // if the singal is SIGINT
    {
        if (strlen(input_string) == 0) // if child is not running then print the prompt message
        {
            write(STDOUT_FILENO, ANSI_COLOR_GREEN "\nminishell$" ANSI_COLOR_RESET, 16);
            fflush(stdout);
        }
    }

    else if (signum == SIGTSTP) // if the signal is SIGTSTP
    {

        if (strlen(input_string) == 0) // if the child is not running just print the prompt message
        {
            fflush(stdout);
        }
        else // else store the command in single linked list
        {
            insert_at_last(pid, cmd); // function call to store the command and pid
        }
    }
}
void sigchld_handler(int signum) // function definition to handle the SIGCHILD signal
{
    int status;
    while (pid = waitpid(-1, &status, WNOHANG) > 0)
    {
        remove_job(pid); // to delete the the command from the linked list
    }
}

void insert_at_last(int pid, char *command) // function definition to insert the command and pid into the linked list
{
    Slist *new = (Slist *)malloc(sizeof(Slist)); // create the new node using malloc function
    new->pid = pid;
    strcpy(new->command, strdup(command));
    new->link = head;
    head = new;
}

void print_list(Slist *head) // function definition.
{
    if (head == NULL) // to check the head is null or not.
    {
        printf("INFO : List is empty\n");
    }
    else // if not null enter the loop.
    {
        while (head) // until head becomes null it runs.
        {
            printf("pid -> %d\t		", head->pid); // to print the data.
            printf("command -> %s", head->command);
            printf("\n");
            head = head->link; // update head with it's next address.
        }
    }
}

void remove_job(int pid) // to delete the command from the linked list
{
    Slist *temp = head; // create the temporary node to traverse
    Slist *prev = NULL;

    while (temp != NULL) // runs until temp becomes NULL
    {
        if (temp->pid == pid) // process pid matches with the pid int the linked list
        {
            if (prev == NULL)      // if prev is NULL then,
                head = temp->link; // then move the head
            else
                prev->link = temp->link; // else assign temp link to prev link
            // free(temp->command);
            // free(temp);
            // temp=NULL;
        }
        prev = temp;       // assing temp to prev node
        temp = temp->link; // move the temp next node
    }
}
