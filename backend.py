import os
from flask import Flask, request, jsonify
from flask_cors import CORS
import subprocess

app = Flask(__name__)
CORS(app)

@app.route("/evaluate", methods=["POST"])
def evaluate():
    if "file" not in request.files:
        return jsonify({"output": "No file uploaded", "input_code": ""}), 400

    file = request.files["file"]
    if file.filename == "":
        return jsonify({"output": "Empty filename", "input_code": ""}), 400

    file.save("input.txt")
    print("Received file and saved as input.txt")

    with open("input.txt", "r") as f:
        input_code = f.read()

    return compile_and_run(input_code)


@app.route("/evaluate_code", methods=["POST"])
def evaluate_code():
    data = request.get_json()
    code = data.get("code")

    if not code.strip():
        return jsonify({"output": "No code provided"}), 400

    with open("input.txt", "w") as f:
        f.write(code)

    print("Received code via editor and saved as input.txt")
    return compile_and_run()


@app.route("/run_c_file", methods=["POST"])
def run_c_file():
    if "file" not in request.files:
        return jsonify({"output": "No file uploaded", "input_code": ""}), 400

    file = request.files["file"]
    filename = "uploaded_code.c"
    file.save(filename)

    with open(filename, "r") as f:
        input_code = f.read()

    try:
        subprocess.run(["gcc", filename, "-o", "user_c_output.exe"], check=True)
        result = subprocess.check_output(["user_c_output.exe"], stderr=subprocess.STDOUT, timeout=5)
        output = result.decode().strip()
    except subprocess.CalledProcessError as e:
        output = f"Compilation/Runtime Error:\n{e.output.decode() if e.output else str(e)}"
    except subprocess.TimeoutExpired:
        output = "Execution timed out."

    return jsonify({"output": output, "input_code": input_code}), 200


def compile_and_run(input_code_from_file=None):
    try:
        subprocess.run(["gcc", "picoc_web.c", "-o", "picoc_web.exe"], check=True)
    except subprocess.CalledProcessError as e:
        return jsonify({"output": f"Compilation Error:\n{e}", "input_code": input_code_from_file or ""}), 200

    try:
        result = subprocess.check_output(["picoc_web.exe"], stderr=subprocess.STDOUT)
        output = result.decode().strip()
    except subprocess.CalledProcessError as e:
        output = f"Runtime Error:\n{e.output.decode() if e.output else str(e)}"

    print("Sending back:", output)

    if input_code_from_file is not None:
        return jsonify({"output": output, "input_code": input_code_from_file}), 200
    else:
        return jsonify({"output": output}), 200


if __name__ == "__main__":
    app.run(debug=True)




