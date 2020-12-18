import emb
import os

def main(a,b):
    info()
    filename = "pythonExec.txt"
    code = f"print({a})"
    writeFile(filename, code)
    print(f"start of executed python code")
    execFileCode(filename)  
    print(f"end of executed python code")
    return 0 

def info():
    print("Number of arguments", emb.numargs())#arg_count
    print(emb.test())#arg_values      

def readFile(filename):
    if os.path.isfile(filename):
        file = open(filename, 'r')
        contents = file.read()
        file.close()
        return contents
    else:
        print(f"filename {filename} invalid.")    

def writeFile(filename, contents):
    try:
        file = open(filename, 'w')
        file.write(contents)
        file.close()
    except:
        print(f"failed to write {filename}")     
    
def execFileCode(filename):
    try:
        code = readFile(filename)
        exec(code)
    except:
        print(f"couldn't exec {filename}")