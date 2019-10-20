//On Linux, running on a modern x86_64 processor:

int main() {
  char *s = "Hello, World!\n";
  long l = 14;
  long fd = 1;
  long syscall = 1;
  long ret = 0;
  __asm__ ( "syscall"
          : "=a" (ret)
          : "a" (syscall),
            "D" (fd),
            "S" (s),
            "d" (l)
          );
  return 0;
}


//Compile it with gcc:

//$ gcc -o test test.c
