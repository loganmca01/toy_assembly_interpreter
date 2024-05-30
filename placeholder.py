import SystemInfo
import AstInterface
import SystemParser
import sys
import math
from tkinter import *
from tkinter import ttk
from tkinter import scrolledtext
import tkinter.font as tkFont

SystemParser.parse_all(sys.argv[1])

reg_display_vars = []

# todo: figure out how to update register display StringVars whenever a register's value is changed
def display_registers(frame):
    count = 0
    global reg_display_vars
    for reg in SystemInfo.regs:
        reg_display_vars.append(StringVar())
        reg_display_vars[count].set(f"{reg.value:#0{10}x}"[2:])

        ttk.Label(frame, text=reg.name + ":").grid(row=count % 3, column=(count // 3) * 2, padx=5, pady=5)
        e = Entry(frame, state="readonly", textvariable=reg_display_vars[count], width=8, readonlybackground="white").grid(row=count % 3, column=(((count // 3) * 2) + 1), padx=5, pady=5, sticky=(W))

        count += 1

def update_register(num):
    global reg_display_vars

    reg_display_vars[num].set(f"{SystemInfo.regs[num].value:#0{10}x}"[2:])

def process_shell_input(event):
    global lines
    t = shell_input.get()
    textbox.config(state="normal")
    textbox.insert(str(lines) + ".0", ">  " + t + "\n")
    textbox.config(state="disabled")
    lines += 1
    shell_input.set("")

def run_full():
    pass

def run_step():
    pass

def run_reset():
    pass

#
#for i in SystemInfo.instructions:
#    SystemParser.print_instruction(i)

root = Tk()

root.title("Simple Assembly Language Simulator")

root.columnconfigure(0, weight=1)
root.rowconfigure(0, weight=1)

default_font = tkFont.nametofont("TkDefaultFont")
default_font.configure(size=14)
root.option_add("*Font", default_font)

mainframe = ttk.Frame(root)
mainframe.grid(column=0, row=0, sticky=(N, W, E, S))

leftside = ttk.Frame(mainframe)
leftside.grid(column=0, row=0, padx=5, pady=5, sticky=(N, W, S))

textbox = scrolledtext.ScrolledText(leftside)
textbox.grid(column=0, row=0, padx=5, pady=5, columnspan=3, rowspan=3, sticky=(N, W, E, S))

textbox.insert("1.0", "Welcome to SALS, type help for more info\n")

textbox.config(state="disabled")

shell_label = ttk.Label(leftside, text=">")
shell_label.grid(column=0, row=3, padx=5, pady=5, sticky=(W))

shell_input = StringVar()
shell_input_entry = ttk.Entry(leftside, textvariable=shell_input)
shell_input_entry.grid(column=1, row=3, padx=15, pady=5, sticky=(N, E, S, W), columnspan=2)

lines = 2

shell_input_entry.bind("<Return>", process_shell_input)

register_frame = ttk.Frame(leftside)
register_frame.grid(row=4, column=0, padx=5, pady=5, sticky=(N, E, S, W), columnspan=3)

display_registers(register_frame)

button_frame = ttk.Frame(leftside)
button_frame.grid(row=5, column=0, padx=5, pady=5, sticky=(N, E, S, W), columnspan=3)

run_button = Button(button_frame, text="Run", command=run_full, bg="green yellow")
step_button = Button(button_frame, text="Step", command=run_step, bg="green yellow")
reset_button = Button(button_frame, text="Reset", command=run_reset, bg="firebrick1")

run_button.grid(row=0, column=0, padx=5, pady=5, sticky = (W))
step_button.grid(row=0, column=1, padx=5, pady=5, sticky = (W))
reset_button.grid(row=0, column=2, padx=5, pady=5, sticky = (E))


rightside = ttk.Frame(mainframe)
rightside.grid(row=0, column=1, padx=5, pady=5, sticky = (N, E, S))

mem_frame = ttk.Frame(rightside)
mem_frame.grid(row=0, column=0, padx=5, pady=5, sticky = (N, E, S, W))

mem_label = ttk.Label(mem_frame, text="Memory")
mem_label.grid(row=0, column=0, padx=5, pady=5, sticky = (N, E, W))

SystemInfo.memory = bytearray(SystemInfo.memory_size)


#tomorrow: try this with listbox like a normal person instead
#idea: goto command for jumping to specific part of stack/code visually
for i in range(0, math.ceil(SystemInfo.memory_size / 4)):
    memloc = i * 4
    ttk.Label(mem_frame, text=f"{SystemInfo.memory[memloc]:#0{6}x}" + ":", width=7).grid(row= i + 1, column=0, ipadx=2, ipady=2)

mainframe.columnconfigure(0, weight=1)
mainframe.rowconfigure(0, weight=1)

for i in range(0,3):
    register_frame.rowconfigure(i, weight=1)

for j in range(0, math.ceil(SystemInfo.num_regs / 3) * 2):
    register_frame.columnconfigure(j, weight=1)

leftside.rowconfigure(0, weight=1)

leftside.columnconfigure(1, weight=1)
leftside.rowconfigure(1, weight=1)

leftside.columnconfigure(2, weight=1)
leftside.rowconfigure(2, weight=1)

root.mainloop()
