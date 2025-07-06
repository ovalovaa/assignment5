import subprocess
import threading

class InterpreterSession:
    def __init__(self, path="./interpreter"):
        self.proc = subprocess.Popen(
            [path],
            stdin=subprocess.PIPE,
            stdout=subprocess.PIPE,
            stderr=subprocess.PIPE,
            text=True,
            bufsize=1
        )
        self.lock = threading.Lock()

    def run(self, expr):
        with self.lock:
            self.proc.stdin.write(expr + "\n")
            self.proc.stdin.flush()

            output_lines = []
            while True:
                line = self.proc.stdout.readline()
                if not line:
                    break
                output_lines.append(line.strip())
                if line.strip().startswith(">") or line.strip().isdigit() or "Error:" in line:
                    break

            return "\n".join(output_lines)

    def close(self):
        self.proc.kill()

def test():
    session = InterpreterSession()
    cases = [
        ("5 + 3 * 2", "11"),
        ("max(5, 2)", "5"),
        ("min(3, 4)", "3"),
        ("max(min(3 * 2, 2), 2)", "2"),
        ("a = 2 + 3", "5"),
        ("a + 3", "8"),
        ("def f(x, y) { min(x, y) + max(x, y) }", "0"),
        ("f(3, 4)", "7"),
        ("var b = pow(2, 4)", "16"),
        ("b + 1", "17")
    ]

    for expr, expected in cases:
        out = session.run(expr)
        if expected not in out:
            print(f" FAIL: {expr} => {out}")
        else:
            print(f" OK: {expr} => {out}")

    session.close()

if __name__ == "__main__":
    test()
