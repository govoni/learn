def func1 () :
    print 'func1'

def fillList (num) :
    a = []
#    a.append (num)
    for i in range (num) :
        a.append (i)
    return a

print 'test01'
func1 ()
b = fillList (6)
for el in b : print el
