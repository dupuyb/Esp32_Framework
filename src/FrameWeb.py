import pathlib
print("current path is : ")
print(pathlib.Path().absolute())
print("//-------- Generated from FrameWeb.html file ")

sub_str = "<!-- const char HTTP_"
espline = ""
mode = 0

# open file in read mode 
fn = open('FrameWeb.html', 'r') 
  
# read the content of the file line by line 
cont = fn.readlines() 
type(cont) 
for i in range(0, len(cont)): 
    if mode==1:
        if (cont[i].strip().find(sub_str) != 0):
            line = cont[i].replace('"', '\\"').replace('\n', '').strip() 
            espline += line
        else:
            espline +='";'
            print(espline)
            espline=""
            mode=0

    if (cont[i].strip().find(sub_str) == 0):
        if (mode==0):
            mode=1
            line = cont[i]
            line = line.replace("<!--", "").replace("-->", "").strip() + ' "'
            espline += line

if (mode==1):
    espline +='";'
    print(espline)
print("//-------------------")

# close all files 
fn.close() 
