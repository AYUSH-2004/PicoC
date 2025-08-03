# Web-Based  C Compiler

This project is a modern, web-based interface for compiling and running both a simplified version of C  and standard C code. It features a sleek, interactive frontend with a live code editor and a Python Flask backend that handles the compilation logic by invoking the GCC compiler.
Users can Gain Understanding of Compiler Design through it by various steps involved in this .



## ✨ Features

* **Dual-Mode Compilation**:
    * **PicoC Interpreter**: Write and run a simplified, custom version of C directly in the browser's code editor.
    * **Standard C Compiler**: Upload `.c` files to compile and run them using a standard `gcc` compiler.
* **Interactive Frontend**:
    * A modern, responsive user interface with a glassmorphism effect and an animated video background.
    * An embedded CodeMirror editor with C syntax highlighting.
    * Drag-and-drop support for uploading `.txt` (PicoC) and `.c` (Standard C) files.
* **Light & Dark Mode**: A theme toggle for user preference.
* **Clear Output**: Displays compilation/runtime output, input code, and execution time.

## 🛠️ Tech Stack

* **Frontend**: HTML, CSS, JavaScript
    * **Code Editor**: CodeMirror
    * **Syntax Highlighting**: Prism.js
* **Backend**: Python
    * **Framework**: Flask
    * **Compiler**: GCC (invoked via Python's `subprocess` module)
* **Interpreter Logic**: The `picoc_web.c` file contains the logic for the custom PicoC interpreter.

## 🚀 Getting Started

To run this project on your local machine, you will need to have Python and a C compiler (GCC) installed.

### Prerequisites

* Python 3.x
* `pip` for installing Python packages
* GCC Compiler (MinGW for Windows, or `build-essential` for Ubuntu/Debian)

### Installation & Setup

1.  **Clone the repository:**
    ```bash
    git clone [https://github.com/your-username/your-repo-name.git](https://github.com/your-username/your-repo-name.git)
    cd your-repo-name
    ```

2.  **Install Python dependencies:**
    ```bash
    pip install Flask flask-cors
    ```

3.  **Run the Backend Server:**
    Open a terminal and run the Flask application. This will start the backend server, typically on `http://127.0.0.1:5000`.
    ```bash
    python backend.py
    ```

4.  **Open the Frontend:**
    In a separate terminal or file explorer, open the `index.html` file directly in your web browser. The application should now be fully functional on your local machine.

## Usage

There are two ways to use the compiler:

### 1. PicoC Interpreter (In-Editor)

* **What it is**: A simplified version of C. It supports basic variable declarations (`int`, `float`), `if`/`while` statements, and a `print` command. Variables must be single lowercase letters.
* **How to use**:
    1.  Type your PicoC code directly into the editor.
    2.  Click the **"Run PicoC Code"** button.
    3.  The output will appear in the "Output" box below.

**Example C Code:**
```c
int a = 5;
int b = 7;
int s = a + b;

print s;

2. Standard C Compiler (File Upload)
What it is: The standard C language. Your code must be a complete program with a main function.

How to use:

Write your code in a text editor and save it with a .c extension (e.g., my_program.c).

Drag and drop this file onto the designated area on the webpage.

The backend will compile it with gcc and display the output.

Example Standard C Code:

#include <stdio.h>

int main() {
    int a = 5;
    int b = 7;
    int sum = a + b;
    printf("The sum is: %d\n", sum);
    return 0;
}



Once deployed, you will get a public URL for your backend (e.g., https://your-compiler-backend.onrender.com).

Crucially, update the fetch URLs in your index.html file to point to this new public backend URL.

Push this change to GitHub, and Netlify will automatically redeploy your frontend with the correct API endpoint.
