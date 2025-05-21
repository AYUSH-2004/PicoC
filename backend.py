import os
from flask import Flask, request, jsonify
from flask_cors import CORS
import subprocess

app = Flask(__name__)
CORS(app)

@app.route("/evaluate", methods=["POST"])
def evaluate():
    data = request.get_json()
    expression = data.get("expression", "")

    print("Received:", expression)  # Debug

    # Save to input.txt
    with open("input.txt", "w") as f:
        f.write(expression + "\n")

    # Compile C code
    try:
        subprocess.run(["gcc", "picoc_web.c", "-o", "picoc_web.exe"], check=True)
    except subprocess.CalledProcessError as e:
        return jsonify({"output": f"Compilation Error:\n{e}"}), 200

    # Run the exe
    try:
        with open("input.txt", "r") as infile:
            result = subprocess.check_output(["picoc_web.exe"], stdin=infile, stderr=subprocess.STDOUT)
            output = result.decode().strip()
    except subprocess.CalledProcessError as e:
        output = f"Runtime Error:\n{e.output.decode() if e.output else str(e)}"

    print("Sending back:", output)  # Debug
    return jsonify({"output": output}), 200

if __name__ == "__main__":
    app.run(debug=True)



