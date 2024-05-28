import SystemInfo
import AstInterface
import SystemParser
import sys
import tkinter as tk

def main():

    SystemParser.parse_all(sys.argv[1])

    for i in SystemInfo.instructions:
        SystemParser.print_instruction(i)

    root = tk.Tk()

    l = tk.Label(root, text="test label").grid()

    root.mainloop()

main()