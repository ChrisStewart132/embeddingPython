import emb

def main(a,b):
    info()
    result = multiply(a,b)
    return result 

def info():
    print("Number of arguments", emb.numargs())#arg_count
    print(emb.test())#arg_values      

def multiply(a,b):   
    return a * b

