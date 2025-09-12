#!/usr/bin/env python3

### COMMON CONVENTIONS
## al - argument list
## s - string (in any sense of the word, most commonly used in reference to a string argument/parameter)
ROOT=""

import subprocess
import re
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

@shell()
def upload(al):
    system("make upload")
    return True

def build(MAKEOPTS="shell"):
    # I don't want to deal with the output
    s = ["make"] + MAKEOPTS.split(" ")
    subprocess.run(s, stdout=subprocess.DEVNULL)
    system("make "+MAKEOPTS)

def shell_build(al):
    MAKEOPTS = ""
    for a in al:
        MAKEOPTS+=a
        MAKEOPTS+=" "
    system("make "+MAKEOPTS)

def replace_line(fname, line_num, text):
    lines = open(fname, 'r').readlines()
    lines[line_num] = text
    with open(fname, 'w') as out:
        out.writelines(lines)
        out.close()

def setvar(al):
    n = al[0]
    a = ""
    for arg in al[1:]:
        a+=arg
        a+=" "
    a = a[:-1]

    val = re.search('"([^"]*)"', a)
    if val == None:
        val = al[1]
    else:
        val = val.group(0)

    lines = []
    i = 0

    with open(ROOT+"include/config.h", 'r') as fp:
        lines = fp.readlines()

        linethere = False
        for l in lines:
            if "#define" not in l or "_CONFIG_H_" in l:
                i+=1
                continue

            bmw = l.split(" ")
            name = bmw[1]
            if name == n:
                linethere = True
                break
            i+=1
        if not linethere:
            print(f"variable '{n}' not found")
            return False

    print(i)
    replace_line(ROOT+"include/config.h",i,"#define "+n+" "+val+'\n')

    return True

def shell_run(command):
    subprocess.run(["/run/current-system/sw/bin/bash", "-i", "-c", command])

def scream(s):
    shell_run("echo '"+s+"' | figlet | lolcat")

@shell()
def clear(al):
    system("clear")
    return True

@shell()
def pog(al):
    scream("pot of GREED")
    return True

@shell()
def list_commands(al):
    for key in fns.keys():
        print(key+": "+fns[key][1])
    # hacky
    print("exit: exits shell")
    return True

@shell()
def list_vars(al):
    with open(ROOT+'include/config.h') as fp:
        for line in fp:
            if not "#define" in line or "_CONFIG_H_" in line:
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
    global aliases
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
# TODO write to files & real time movement functions
fns = {"help":(list_commands, "list available commands"),
    "clear":(clear, "clear the screen"),
    "ls":(list_vars, "list game variables"),
    "pog":(pog, "together we are pot of greed"),
    "alias":(mkalias, "makes an alias ; usage example: alias name=clear && ls && pog"),
    "aliases":(list_aliases, "lists available aliases"),
    "upload":(upload, "uploads code to bot"),
    "set":(setvar, "sets a variable in config.h"),
    "make":(shell_build, "builds program ; usage: make $MAKE_OPTIONS"),
}

# ALIASES (USER-DEFINED & DEFAULT)
aliases = {
}

def from_vshrc():
    global ROOT
    with open('vsh/vshrc') as fp:
        for line in fp:
            if line[0] == '#':
                continue
            if "=" in line:

                # how tf do you name things
                toyota = line.split("=")

                if toyota[0] == "ROOT":
                    ROOT = toyota[1][:-1]
                if toyota[0].split(" ")[0] == "alias":
                    eval_str(line[:-1])

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
    from_vshrc()
    PS1 = "\033[31;32;6mPot of Greed\033[0m \033[31;95;1m>>>\033[0m "
    while True:
       print(PS1, end='')
       s = input("")
       if s == "exit":
           scream("exiting")
           break
       try:
           eval_str(aliases[s])
       except KeyError:
           eval_str(s)

if __name__ == "__main__":
   main()
