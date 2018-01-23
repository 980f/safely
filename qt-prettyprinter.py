def qdump__NumericalValue(d, value):
    iss = value['is'].integer()
    val = value['storage']
    if iss == 0:
        img = val.extractInteger(8, True)
    elif iss == 1:
        img = val.extractInteger(32, False)
    elif iss == 2:
        img = val.extractInteger(32, True)
    else:
        img = val.extractSomething('d', 64)
    d.putValue('%s[%d] %s' % (["Truthy", "Whole", "Counting", "Floating"][iss], iss, img))
