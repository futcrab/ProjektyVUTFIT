"""File dialog for use in GUI"""

__autor__ = "Peter Ďurica"
__contact__ = "xduric05@vutbr.cz"
__version__ = "1.0"

import tkinter as tk
from tkinter import filedialog

def FileDialog(type, name):
    root = tk.Tk()
    root.withdraw()

    return filedialog.askopenfilename(filetypes=[(name + " files", "*." + type)])
