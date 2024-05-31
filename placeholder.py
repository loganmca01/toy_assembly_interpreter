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
    textbox.see(str(lines) + ".0")
    textbox.config(state="disabled")
    lines += 1
    shell_input.set("")

def run_full():
    pass

def run_step():
    pass

# todo: implement, using for testing stringVar at the moment
def run_reset():
    global code_list, code_list_var
    code_list.append("hello")
    code_list_var.set(code_list)

#
#for i in SystemInfo.instructions:
#    SystemParser.print_instruction(i)

root = Tk()

root.title("Simple Assembly Language Simulator")

root.columnconfigure(0, weight=1)
root.rowconfigure(0, weight=1)

default_font = tkFont.nametofont("TkDefaultFont")
default_font.configure(size=14)
default_font.configure(family="DejaVu Sans Mono")
root.option_add("*Font", default_font)

mainframe = ttk.Frame(root)
mainframe.grid(column=0, row=0, sticky=(N, W, E, S))

leftside = ttk.Frame(mainframe)
leftside.grid(column=0, row=0, padx=5, pady=5, sticky=(N, W, S))

top_label = ttk.Label(leftside, text="Console")
top_label.grid(column = 0, padx = 5, pady = 5, row = 0, columnspan = 3, sticky = (N, E, S, W))

textbox = scrolledtext.ScrolledText(leftside)
textbox.grid(column=0, row=1, padx=5, pady=5, columnspan=3, rowspan=3, sticky=(N, W, E, S))

textbox.insert("1.0", "Welcome to SALS, type help for more info\n")

textbox.config(state="disabled")

shell_label = ttk.Label(leftside, text=">")
shell_label.grid(column=0, row=4, padx=5, pady=5, sticky=(W))

shell_input = StringVar()
shell_input_entry = ttk.Entry(leftside, textvariable=shell_input)
shell_input_entry.grid(column=1, row=4, padx=15, pady=5, sticky=(N, E, S, W), columnspan=2)

lines = 2

shell_input_entry.bind("<Return>", process_shell_input)

register_frame = ttk.Frame(leftside)
register_frame.grid(row=5, column=0, padx=5, pady=5, sticky=(N, E, S, W), columnspan=3)

display_registers(register_frame)

button_frame = ttk.Frame(leftside)
button_frame.grid(row=6, column=0, padx=5, pady=5, sticky=(N, E, S, W), columnspan=3)

run_button = Button(button_frame, text="Run", command=run_full, bg="green yellow")
step_button = Button(button_frame, text="Step", command=run_step, bg="green yellow")
reset_button = Button(button_frame, text="Reset", command=run_reset, bg="firebrick1")

run_button.grid(row=0, column=0, padx=5, pady=5, sticky = (W))
step_button.grid(row=0, column=1, padx=5, pady=5, sticky = (W))
reset_button.grid(row=0, column=2, padx=5, pady=5, sticky = (E))


rightside = ttk.Frame(mainframe)
rightside.grid(row=0, column=1, padx=5, pady=5, sticky = (N, W, S))

#, borderwidth=2, relief="raised" for testing borders

mem_frame = ttk.Frame(rightside)
mem_frame.grid(row=0, column=0, padx=(5, 25), pady=5, sticky = (N, S, W))

mem_label = ttk.Label(mem_frame, text="Memory", anchor="center")
mem_label.grid(row=0, column=0, padx=5, pady=5, sticky = (N, E, W))

SystemInfo.memory = bytearray(SystemInfo.memory_size)

#idea: goto command for jumping to specific part of stack/code visually
#implementation: use listbox.see() function
#note: string[-1] is last character (for checking newline at end)

addr_list = []
for i in range(0, math.ceil(SystemInfo.memory_size / 4)):
    memstring = f"{i * 4:#0{6}x}"[2:] + ": "
    for j in range(0, 4):
        memstring += " " + f"{SystemInfo.memory[(i * 4) + j]:#0{4}x}"[2:]
    addr_list.append(memstring)

mem_list_var = StringVar(value = addr_list)

mem_address_list = Listbox(mem_frame, width=18, listvariable = mem_list_var)
mem_address_list.grid(row=1, column=0, sticky = (N, W, S))




code_list = []
code_list.append("test")

code_list_var = StringVar(value = code_list)

code_frame = ttk.Frame(rightside)
code_frame.grid(row=0, column=1, padx=(25, 5), pady=5, sticky = (N, S, W))

code_address_list = Listbox(code_frame, width=18, listvariable = code_list_var)
code_address_list.grid(row = 1, column = 1, sticky = (N, E, S))

code_label = ttk.Label(code_frame, text="Code", anchor="center")
code_label.grid(row=0, column=1, padx=5, pady=5, sticky = (N, E, W))






mainframe.columnconfigure(0, weight=1)
mainframe.rowconfigure(0, weight=1)

mainframe.columnconfigure(1, weight=1)

rightside.rowconfigure(0, weight=1)
rightside.columnconfigure(0, weight=1)
rightside.columnconfigure(1, weight=1)

mem_frame.columnconfigure(0, weight=1)
mem_frame.rowconfigure(1, weight=1)

code_frame.columnconfigure(0, weight=1)
code_frame.rowconfigure(1, weight=1)

for i in range(0,3):
    register_frame.rowconfigure(i, weight=1)

for j in range(0, math.ceil(SystemInfo.num_regs / 3) * 2):
    register_frame.columnconfigure(j, weight=1)

leftside.rowconfigure(1, weight=1)

leftside.columnconfigure(1, weight=1)
leftside.rowconfigure(2, weight=1)

leftside.columnconfigure(2, weight=1)
leftside.rowconfigure(3, weight=1)

root.mainloop()