#!/usr/bin/env python3

### COMMON CONVENTIONS
## al - argument list
## s - string (in any sense of the word, most commonly used in reference to a string argument/parameter)

import subprocess
from os import system
from typing import TypeAlias

# shell decorator function
# anum - argument number
def shell(anum=0):
    def shell_decorator(fun):
        def wrapper(al):
            if len(al) != anum:
                    print("ERROR: argument # doesn't match function")
                    return False
            result = fun(al)
            return result
        return wrapper
    return shell_decorator


# TODO PROS ABSTRACTIONS
def shell_build(al):
    MAKEOPTS = ""
    for a in al:
        MAKEOPTS+=a
        MAKEOPTS+=" "
    build(MAKEOPTS)

@shell()
def upload(al):
    system("make upload")
    return True

def build(MAKEOPTS):
    shell_run("make "+MAKEOPTS)


def shell_run(command):
    subprocess.run(["/run/current-system/sw/bin/bash", "-i", "-c", command])

@shell()
def clear(al):
    system("clear")
    return True

@shell()
def pog(al):
    shell_run("echo 'pot of GREED' | figlet | lolcat")
    return True

@shell()
def list_commands(al):
    for key in fns.keys():
        print(key+": "+fns[key][1])
    return True

@shell()
def list_vars(al):
    with open('src/include/config.hpp') as fp:
        for line in fp:
            if not "#define" in line or "_CONFIG_HPP_" in line:
                continue
            print(line.split(" ")[1])
    return True

# TODO these
@shell(anum=1)
def move(al):
    return True

@shell(anum=1)
def turn(al):
    return True

# special case - doesn't use decorator
def mkalias(al):
    arg = ""
    for s in al:
        arg+=s
        arg+=" "
    arg = arg[:-1]
    l1 = arg.split("=")
    aliases[l1[0]] = l1[1]

@shell()
def list_aliases(al):
    for key in aliases.keys():
        print(key)

# DICTIONARY OF ALL AVAILABLE SHELL COMMANDS
fns = {
    "help":(list_commands, "list available commands"),
    "clear":(clear, "clear the screen"),
    "ls":(list_vars, "list game variables"),
    "pog":(pog, "together we are pot of greed"),
    "alias":(mkalias, "makes an alias ; usage example: alias name=clear && ls && pog"),
    "aliases":(list_aliases, "lists available aliases"),
    "upload":(upload, "uploads code to bot"),
    "make":(shell_build, "buidls program"),
}

# ALIASES (USER-DEFINED & DEFAULT)
aliases = {
    "pid":"move 10 && turn 180",
    "hype":"clear && pog",
}

def eval_str(s:str):
    if " && " in s and not "alias" in s:
        for sub_command in s.split(" && "):
            sp = sub_command.split(" ")
            try:
                if not fns[sp[0]][0](sp[1:]):
                    return False
            except KeyError:
                print("ERROR: argument # doesn't match function")
                return False
        return True
    try:
        if s == '':
            return False
        sp = s.split(" ")
        if s in aliases.keys():
            return eval_str(aliases[sp])
        return fns[sp[0]][0](sp[1:])
        # hack
    except KeyError:
        print(f"ERROR: command '{sp[0]}' not found")
        return False

def main():
    PS1 = "\033[31;32;6mPot of Greed\033[0m \033[31;95;1m>>>\033[0m "
    while True:
       print(PS1, end='')
       s = input("")
       try:
           eval_str(aliases[s])
       except KeyError:
           eval_str(s)

if __name__ == "__main__":
   main()
